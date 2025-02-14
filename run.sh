#!/bin/bash
export SCRIPT_DIR="$(cd "$(dirname "$0")"; pwd -P)"
cd "$SCRIPT_DIR"
source .env
set -e
set -o pipefail
# set -o nounset

export SAM_IMG_BASE=${SAM_IMG_BASE:-$SAM_URL}
export SAM_IMG_TAG=${SAM_IMG_TAG:-fsw_$(git rev-parse --abbrev-ref HEAD | sed 's/\//_/g')}
export SAM_IMG="$SAM_IMG_BASE:$SAM_IMG_TAG"

check_port() {
    local port=$1
    if netstat -ano | grep ":$port "; then
        echo "Port $port is in use. Checking its state..."
        if netstat -ano | grep ":$port.*TIME_WAIT"; then
            echo "Port $port is in TIME_WAIT state. Wait $(cat /proc/sys/net/ipv4/tcp_fin_timeout) seconds, or use a different port."
        else
            echo "Port $port is actively in use by another process. Use lsof -i :$port to find the process."
        fi
        return 1
    fi
    return 0
}

find_board_port() {
    local port=""
    case "$OSTYPE" in
        darwin*) port=$(ls /dev/cu.usbmodem* 2>/dev/null | head -n 1) ;;
        linux*)  port=$(ls /dev/ttyACM* 2>/dev/null | head -n 1) ;;
        *) echo "Unsupported OS for automatic port detection"; return 1 ;;
    esac

    if [ -z "$port" ]; then
        echo "Board port ($port) not found."
        return 1
    fi

    echo "$port"
}

show_help() {
  cat << EOF
Usage: $(basename "$0") [OPTIONS] COMMAND
Options:
  --clean              Clean build
  --daemon             Run as daemon
  --standalone         Run the command without starting any implied dependencies
  --help               Show this help message
Commands:
  mbd-to-xml           Convert MBD to XML
  build base           Build the base application
  build docker         Build the Docker image
  exec test            Run the yamcs studio editor
  inspect              Inspect the development container
  teardown             Tear down the environment
EOF
}

# Default values
DEFAULT_SERVICE="sam"
DEFAULT_FLAGS="-it"
BASE_FLAGS="--rm --user $(id -u):$(id -g) --remove-orphans"
CLEAN=0
STANDALONE=0
DAEMON=0

# Process flags
for arg in "$@"; do
    case $arg in
        --daemon) DAEMON=1 ;;
        --clean) CLEAN=1 ;;
        --standalone) STANDALONE=1 ;;
        --help) show_help; exit 0 ;;
    esac
done

exec_cmd() {
    local cmd="${1}"
    echo "Executing: $cmd"
    eval "$cmd"
    exit_code=$?
    if [ $exit_code -eq 1 ] || [ $exit_code -eq 2 ]; then
        echo "Failed cmd with error $exit_code"
        exit $exit_code;
    fi
}

run_docker_compose() {
    local service="$1"
    local cmd="$2"
    # Always kill the container after executing the command
    # by default run the command with an interactive tty
    local flags="$BASE_FLAGS ${3:$DEFAULT_FLAGS}"

    if [ "${DAEMON}" -eq "1" ]; then
        flags="${flags//-it/}"   # Remove standalone "-it"

        flags+=" --detach"
    fi

    [ "$STANDALONE" -eq 1 ] && flags="--no-deps"

    exec_cmd "docker compose run $flags $service $cmd"
}

try_docker_exec() {
    local service="$1"
    local cmd="$2"
    local container_name="fprime-${service}"  # assuming your container naming convention
    local flags="$3"

    # Check if container is running
    if docker container inspect "$container_name" >/dev/null 2>&1; then
        echo "Container $container_name is running, using docker exec..."
        exec_cmd "docker exec $flags $container_name $cmd"
    else
        echo "Container $container_name is not running, using docker compose run..."
        run_docker_compose "$service" "$cmd" "$flags"
    fi
}

try_stop_container() {
  local container_name="$1"
  local timeout=${2:-10}  # default 10 seconds timeout

  if docker container inspect "$container_name" >/dev/null 2>&1; then
      echo "Stopping container $container_name (timeout: ${timeout}s)..."
      if ! docker container stop -t "$timeout" "$container_name"; then
          echo "Failed to stop container $container_name gracefully"
          return 1
      fi
      echo "Container $container_name stopped successfully"
  else
      echo "Container $container_name is not running"
  fi
}

build_docker() {
  if ! git diff-index --quiet HEAD --; then
      read -p "You have unstaged changes. Continue? (y/n) " -n 1 -r
      echo
      [[ $REPLY =~ ^[Yy]$ ]] || { echo "Build cancelled."; exit 1; }
  fi

  # Fetch from remote to ensure we have latest refs
  git fetch -q origin

  # Get current commit hash
  CURRENT_COMMIT=$(git rev-parse HEAD)

  # Check if current commit exists in any remote branch
  if ! git branch -r --contains "$CURRENT_COMMIT" | grep -q "origin/"; then
      read -p "Current commit not found in remote repository. Continue? (y/n) " -n 1 -r
      echo
      [[ $REPLY =~ ^[Yy]$ ]] || { echo "Build cancelled."; exit 1; }
  fi

  CMD="docker compose --progress=plain --env-file=${SCRIPT_DIR}/.env build fsw"
  [ "$CLEAN" -eq 1 ] && CMD+=" --no-cache"
  CMD+=" --build-arg GIT_COMMIT=$(git rev-parse HEAD) --build-arg GIT_BRANCH=$(git rev-parse --abbrev-ref HEAD)"
  exec_cmd "$CMD"
}

update_build_env() {
  build_json_path=$1
  clangd_cmd="sed -i \"s|CompilationDatabase: .*|CompilationDatabase: \"${build_json_path}\"|\" .clangd"
  exec_cmd "$clangd_cmd"

  mod_dict_cmd="sed -i \"s|${SAM_WDIR}|${SCRIPT_DIR}|g\" \"${build_json_path}/compile_commands.json\""

  exec_cmd "$mod_dict_cmd"
}

build_cmsis_startup() {
    cmsis_path="fprime-cmsis/cmake/toolchain/support/sources/samv71q21b"
    flags="-w $SAM_WDIR/$cmsis_path $DEFAULT_FLAGS"
    # NOTE we often get stuck on trivial schema errors.
    # prevent this with -n
    # cmd="csolution -v -d convert blinky.csolution.yml"
    # cmd="cbuild -v -p blinky.csolution.yml"
    # cmd="cbuild setup blinky.csolution.yml --context-set"
    cmd="cbuild blinky.csolution.yml -d --context-set --packs --rebuild"
    try_docker_exec "sam" "$cmd" "$flags"

    update_build_env "${SCRIPT_DIR}/${cmsis_path}/out/blinky/SamV71-Xplained-Board/Debug"
}

wine_exec() {
    BASE_COMMAND=${1:-}
    [ -z "$BASE_COMMAND" ] && { echo "Error: must specify base command"; exit 1; }

    # Base flags ensure we start the container with the host username, UID, GID and home path
    # and the host network
    # FIXME the network doesn't seem to connect,
    # this was not a problem when previously following the "KeilDocker" tutorial which used MDK v5.31
    flags="--as-me "
    flags+="--volume=$SCRIPT_DIR:$HOME$SAM_WDIR "

    # NOTE This seemed to be required after switching users however after doing so keil was unable to
    # connect to the internet. It's not well understood why this was the case but the issue was resolved
    # after removing this flag, removing the volume/docker image and re-building fresh.
    # flags+="--force-owner"

    base_cmd="bash ${SCRIPT_DIR}/deps/docker-wine/docker-wine ${flags} "
    cmd="$base_cmd $BASE_COMMAND"

    exec_cmd "$cmd"
    # exit_code=$?
    # if [ exit_code != 0 ]; then
    #     echo "Command failed with error code: $exit_code"
    #     exit $exit_code
    # fi
}

keil_exec() {
    BASE_COMMAND=${1:-}
    [ -z "$BASE_COMMAND" ] && { echo "Error: must specify base command"; exit 1; }

    # Retrieve the keil exe, v5.41
    mdk_exe="MDK541.EXE"
    # mdk_exe="MDK520.EXE"
    # NOTE for keil's license
    # License Agreement MDK v5.41 and above
    # "Non-Commercial Use License" means an evaluation, preview, beta, pre-release, pilot, academic, educational or community use only license.
    user="$USER"
    mdk_path="/home/$user/.wine/drive_c/users/$user/Downloads"
    uvision_path="/home/$user/.wine/drive_c/users/$user/AppData/Local/Keil_v5/UV4/UV4.exe"

    case $BASE_COMMAND in
      "uv")
        wine_exec "wine $uvision_path"
      ;;
      "build")
        # If we don't have micro vision then we need to install it
        if [ ! -f "$uvision_path" ]; then
          # In order to install micro vision we need to use the MDK exe
          if [ ! -f "$mdk_path/$mdk_exe" ]; then
            download_mdk_cmd="wget https://armkeil.blob.core.windows.net/eval/${mdk_exe} -P $mdk_path"
            wine_exec "$download_mdk_cmd"
          fi

          wine_exec "wine $mdk_path/$mdk_exe"
        fi
      ;;
      *)
      echo "Invalid exec command: ${EXEC_TARGET}"
      exit 1
      ;;
    esac
}

ateml_exec() {
    BASE_COMMAND=${1:-}
    [ -z "$BASE_COMMAND" ] && { echo "Error: must specify base command"; exit 1; }

    atmel_installer_exe="as-installer-7.0.2594-full.exe"
    user="$USER"
    download_path="/home/$user/.wine/drive_c/users/$user/Downloads"
    # uvision_path="/home/$user/.wine/drive_c/users/$user/AppData/Local/Keil_v5/UV4/UV4.exe"
    mc_url="https://ww1.microchip.com/downloads/aemDocuments/documents/DEV/ProductDocuments/SoftwareTools/"

    case $BASE_COMMAND in
      "studio")
        echo "Not yet supported"
        exit 1
      ;;
      "build")
        # If we don't have micro vision then we need to install it
        if [ ! -f "$uvision_path" ]; then
          # In order to install micro vision we need to use the MDK exe
          if [ ! -f "$mdk_path/$mdk_exe" ]; then
            download_atmel_cmd="curl -fSL -A \"Mozilla/4.0\" -o $download_path/$atmel_installer_exe $mc_url"
            wine_exec "$download_atmel_cmd"
          fi

          wine_exec "bash"
        fi
      ;;
      *)
      echo "Invalid exec command: ${EXEC_TARGET}"
      exit 1
      ;;
    esac
}

mplab_exec() {
    BASE_COMMAND=${1:-}
    [ -z "$BASE_COMMAND" ] && { echo "Error: must specify base command"; exit 1; }

    xhost +local:docker
    run_docker_compose $BASE_COMMAND --service="mplab"
}

case $1 in
  "format")
    if [[ "$2" == *.fpp ]]; then
      container_file="${2/$SCRIPT_DIR/$SAM_WDIR}"
      echo "Formatting FPP file: $container_file"
      cmd="fpp-format $container_file"
      # Create a temporary marker that's unlikely to appear in normal code
      marker="@ COMMENT_PRESERVE@"
      # Chain the commands:
      # 1. Transform comments to temporary annotations
      # 2. Run fpp-format
      # 3. Transform back to comments
      # 4. Write back to the original file
    tmp_file="${container_file/.fpp/_tmp.fpp}"

    # Create a multi-line command with error checking
    read -r -d '' cmd <<EOF
    set -e  # Exit on any error

    # Create backup
    cp "$container_file" "${container_file}.bak"

    # Attempt formatting pipeline
    if sed 's/^\\([ ]*\\)#/\\1${marker}#/' "$container_file" \
       | fpp-format \
       | sed 's/^\\([ ]*\\)${marker}#/\\1#/' > "$tmp_file"; then

        # If successful, verify tmp file exists and has content
        if [ -s "$tmp_file" ]; then
            mv "$tmp_file" "$container_file"
            rm "${container_file}.bak"
            echo "Format successful"
        else
            echo "Error: Formatted file is empty"
            mv "${container_file}.bak" "$container_file"
            exit 1
        fi
    else
        echo "Error during formatting"
        mv "${container_file}.bak" "$container_file"
        [ -f "$tmp_file" ] && rm "$tmp_file"
        exit 1
    fi
EOF
      flags="-w $SAM_WDIR $DEFAULT_FLAGS"
      try_docker_exec "sam" "$cmd" "$flags"
    else
      fprime_root="${2:-$SCRIPT_DIR/deps/fprime}"  # Get the path provided or use current directory
      fprime_root="${fprime_root/$SCRIPT_DIR/$SAM_WDIR}"
      echo "Formatting from $fprime_root"
      cmd="git diff --name-only --relative | fprime-util format --no-backup --stdin"
      try_docker_exec "sam" "bash -c \"$cmd\""
    fi
    ;;

  "build")
    EXEC_TARGET=${2:-}
    [ -z "$EXEC_TARGET" ] && { echo "Error: must specify target to exec"; exit 1; }

    case $EXEC_TARGET in
      "Base")
        cmd="cmake -DENABLE_DEBUGGING=ON -DHOST_PATH=${SCRIPT_DIR} .. && make"
        [ "$CLEAN" -eq 1 ] && cmd="rm -rf ../build/* && $cmd"

        flags="-w $SAM_BASE_DIR/build $DEFAULT_FLAGS"

        run_docker_compose $cmd --service="sam" -- $flags

        MOD_DICT_CMD="sed -i \"s|${SAM_WDIR}|${SCRIPT_DIR}|g\" \"${SCRIPT_DIR}/Base/build/compile_commands.json\""

        exec_cmd "$MOD_DICT_CMD"
      ;;
      "cmsis-startup")
        build_cmsis_startup
      ;;
      "keil-cfg")
        keil_exec "build"
      ;;
      "atmel-studio")
        ateml_exec "build"
      ;;
      "docker")
        build_docker
      ;;
      "test")
        echo "Not yet supported"
        exit 1
      ;;
      *)
      echo "Invalid exec command: ${EXEC_TARGET}"
      exit 1
      ;;
    esac
    ;;

  "load")
    EXEC_TARGET=${2:-}
    [ -z "$EXEC_TARGET" ] && { echo "Error: must specify target to exec"; exit 1; }

    case $EXEC_TARGET in
      "cmsis-startup")
        cfg_path=".vscode/atsamv71_xplained_edbg.cfg"
        bin_path="./fprime-cmsis/cmake/toolchain/support/sources/samv71q21b/out/blinky/SamV71-Xplained-Board/Debug/blinky.elf"
        # bin_path="./Base/build/base_app_SAMV71Q21B.elf"
        program_cmd="init; reset halt; program $bin_path preverify verify reset exit"
        # program_cmd="init; reset halt; exit"
        load_cmd="openocd -d1 -f $cfg_path -c \"$program_cmd\""

        # run_docker_compose $cmd --service="sam" -- $flags
        exec_cmd "$load_cmd"
      ;;
      *)
      echo "Invalid operation."
      exit 1
      ;;
    esac
    ;;

  "exec")
    EXEC_TARGET=${2:-}
    [ -z "$EXEC_TARGET" ] && { echo "Error: must specify target to exec"; exit 1; }

    case $EXEC_TARGET in
      "keil-cfg")
        keil_exec "uv"
      ;;
      "mplab-cfg")
        mplab_exec "mplab_ide"
      ;;
      "gdb-server")
        bin_path="./fprime-cmsis/cmake/toolchain/support/sources/samv71q21b/out/blinky/SamV71-Xplained-Board/Debug/blinky.elf"
        serve_cmd="pyocd gdbserver --elf $bin_path -t atsamv71q21b"

        # run_docker_compose $cmd --service="sam" -- $flags
        exec_cmd "$serve_cmd"
      ;;
      "base")
        echo "Not yet supported"
        exit 1
      ;;
      "test")
        echo "Not yet supported"
        exit 1
      ;;
      *)
      echo "Invalid operation."
      exit 1
      ;;
    esac
    ;;

    "inspect")
        INSPECT_TARGET=${2:-}
        [ -z "$INSPECT_TARGET" ] && { echo "Error: must specify target to inspect"; exit 1; }
        case $INSPECT_TARGET in
            "wine")
              wine_exec "bash"
          ;;
            "mplab")
              # Fall through, all these case are the same.
          ;&
            "sam")
            try_docker_exec $INSPECT_TARGET "bash" "-it"
          ;;
          *)
          echo "Invalid inspect target: ${INSPECT_TARGET}"
          exit 1
          ;;
        esac
        ;;
    "teardown")
        echo "Tearing down services..."
        exec_cmd "docker compose down"
        ;;
    *)
        echo "Invalid operation. Not a valid run.sh argument."
        show_help
        exit 1
        ;;
esac
