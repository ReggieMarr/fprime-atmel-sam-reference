#!/bin/bash
source .env
export SCRIPT_DIR="$(cd "$(dirname "$0")"; pwd -P)"
cd "$SCRIPT_DIR"
set -e
set -o pipefail
set -o nounset

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
DEFAULT_FLAGS="-it --rm"
CLEAN=0

exec_cmd() {
    cmd="$1"
    echo "$cmd"
    eval "$cmd"
    exit_code=$?
    if [ $exit_code -ne 0 ]; then
        echo "Failed cmd with error $exit_code"
        exit $exit_code
    fi
}

# A DWIM function, interprets the arguments passed to it contextually
run_docker_compose() {
    # Get list of services from docker-compose.yml
    # Get list of services from docker-compose.yml and clean up the output
    SERVICES=$(docker compose config --services | sed 's/^[0-9]*://')
    service="$DEFAULT_SERVICE"
    command=""
    flags="$DEFAULT_FLAGS"

    # Process arguments
    parsing_flags=false
    found_service=false

    for arg in "$@"; do
        # If we haven't found a service yet, check if this is a valid service
        if [ "$found_service" = false ] && echo "$SERVICES" | grep -q "^${arg}$"; then
            service="$arg"
            found_service=true
            continue
        fi

        # If we get here, this must be part of the command
        if [ -z "$command" ]; then
            command="$arg"
        else
            command="$command $arg"
        fi

        # Check if this argument starts with a dash
        if [[ "$arg" == -* ]]; then
            parsing_flags=true
            flags="$arg"
            continue
        fi

        # Check if we're already parsing flags
        if [ "$parsing_flags" = true ]; then
            flags="$flags $arg"
            continue
        fi
    done

    # Construct and execute the command
    docker_cmd="docker compose run $flags $service $command"
    exec_cmd "$docker_cmd"
}

wine_exec() {
    BASE_COMMAND=${1:-}
    [ -z "$BASE_COMMAND" ] && { echo "Error: must specify base command"; exit 1; }

    # Base flags ensure we start the container with the host username, UID, GID and home path
    # and the host network
    # FIXME the network doesn't seem to connect,
    # this was not a problem when previously following the "KeilDocker" tutorial which used MDK v5.31
    flags="--as-me "
    flags+="--volume=$SCRIPT_DIR:$HOME$WDIR "

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

mplab_exec() {
    BASE_COMMAND=${1:-}
    [ -z "$BASE_COMMAND" ] && { echo "Error: must specify base command"; exit 1; }
    BASE_COMMAND="mplab $BASE_COMMAND"

    xhost +local:docker
    run_docker_compose $BASE_COMMAND
}

# Process flags
for arg in "$@"; do
    case $arg in
        --clean) CLEAN=1 ;;
        --help) show_help; exit 0 ;;
    esac
done

case $1 in

  "build")
    EXEC_TARGET=${2:-}
    [ -z "$EXEC_TARGET" ] && { echo "Error: must specify target to exec"; exit 1; }

    case $EXEC_TARGET in
      "base")
        echo "Not yet supported"
        exit 1
      ;;
      "keil-cfg")
        keil_exec "build"
      ;;
      "docker")
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

        CMD="docker compose --progress=plain --env-file=${SCRIPT_DIR}/.env build sam"
        [ "$CLEAN" -eq 1 ] && CMD+=" --no-cache"
        CMD+=" --build-arg GIT_COMMIT=$(git rev-parse HEAD) --build-arg GIT_BRANCH=$(git rev-parse --abbrev-ref HEAD)"
        exec_cmd "$CMD"
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
            run_docker_compose $INSPECT_TARGET "bash"
          ;;
            "sam")
            run_docker_compose $INSPECT_TARGET "bash"
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
