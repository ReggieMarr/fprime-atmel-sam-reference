# Base image with common system packages
FROM ubuntu:24.04 AS base
ARG DEBIAN_FRONTEND=noninteractive
ENV TZ='America/Montreal'
RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y \
        sudo build-essential udev git wget curl rsync make cmake openocd \
        libpython3-dev python3-full python3-dev pipx

# Set the version of the ARM toolchain
ENV ARM_TOOLCHAIN_VERSION="10.3-2021.10"
ENV ARM_TOOLCHAIN_PATH="/opt/arm-none-eabi"

# Download and install ARM toolchain
RUN wget -q https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-rm/${ARM_TOOLCHAIN_VERSION}/gcc-arm-none-eabi-${ARM_TOOLCHAIN_VERSION}-x86_64-linux.tar.bz2 && \
    mkdir -p ${ARM_TOOLCHAIN_PATH} && \
    tar xf gcc-arm-none-eabi-${ARM_TOOLCHAIN_VERSION}-x86_64-linux.tar.bz2 --strip-components=1 -C ${ARM_TOOLCHAIN_PATH} && \
    rm gcc-arm-none-eabi-${ARM_TOOLCHAIN_VERSION}-x86_64-linux.tar.bz2

RUN apt-get install -y \
    clang-format libtinfo6 libncurses-dev

#NOTE this is a bit sketchy but seems to allow use to use gdb
RUN ln -s /lib/x86_64-linux-gnu/libncurses.so.6 /usr/lib/libncurses.so.5
RUN ln -s /lib/x86_64-linux-gnu/libtinfo.so.6  /usr/lib/libtinfo.so.5

# Add ARM toolchain to PATH
ENV PATH="${ARM_TOOLCHAIN_PATH}/bin:${PATH}"

# User setup layer
FROM base AS user-setup
ARG WDIR=/fprime-atmel-sam-reference
ARG HOST_UID=1000
ARG HOST_GID=1000
ENV PATH="/home/user/.local/bin:${PATH}"

RUN userdel -r ubuntu || true && \
    getent group 1000 && getent group 1000 | cut -d: -f1 | xargs groupdel || true && \
    groupadd -g ${HOST_GID} user && \
    useradd -u ${HOST_UID} -g ${HOST_GID} -m user && \
    echo 'user ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

# USB and permissions setup
RUN groupadd -f dialout && \
    usermod -a -G dialout user && \
    echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="2341", ATTR{idProduct}=="003d", MODE="0666", GROUP="dialout"' > /etc/udev/rules.d/99-arduino.rules

# Grant permissions to /dev/tty* devices (required to avoid sudo for serial access)
RUN sudo chown user:dialout /dev/tty* || true
COPY ./deps/50-cmsis-dap.rules /etc/udev/rules.d/50-cmsis-dap.rules

USER user
RUN pipx install pyocd
RUN pyocd pack install ATSAMV71Q21B

# MPLAB installation layer
# FROM user-setup AS mplab-setup
# ENV MPLABX_VERSION=6.20
# ENV XC32_VERSION=4.45
# WORKDIR /tmp

# RUN apt-get update && \
#     apt-get upgrade -y && \
#     apt-get install -y \
#     libusb-1.0-0 \
#     default-jdk \
#     default-jre \
#     libxtst6 \
#     libxrender1 \
#     libxi6

# # Download and extract MPLAB X
# RUN curl -qgb "" -fLC - --retry 3 --retry-delay 3 \
#     -e "https://www.microchip.com/en-us/tools-resources/develop/mplab-x-ide" \
#     -o mplabx-installer.tar \
#     "https://ww1.microchip.com/downloads/aemDocuments/documents/DEV/ProductDocuments/SoftwareTools/MPLABX-v${MPLABX_VERSION}-linux-installer.tar" && \
#     tar xf mplabx-installer.tar && \
#     rm mplabx-installer.tar

# # Install MPLAB X
# RUN USER=root ./MPLABX-v${MPLABX_VERSION}-linux-installer.sh --nox11 \
#     -- --unattendedmodeui none --mode unattended && \
#     rm ./MPLABX-v${MPLABX_VERSION}-linux-installer.sh

# # Download and install XC32 compiler
# # https://www.microchip.com/en-us/tools-resources/develop/mplab-xc-compilers/xc32
# # RUN curl -fSL -A "Mozilla/4.0" -o /tmp/xc32.run "https://ww1.microchip.com/downloads/aemDocuments/documents/DEV/ProductDocuments/SoftwareTools/xc32-v${XC32_VERSION}-full-install-linux64-installer.run" \
# #     && chmod a+x /tmp/xc16.run \
# #     && /tmp/xc32.run --mode unattended --unattendedmodeui none \
# #         --netservername localhost --LicenseType FreeMode \
# #     && rm /tmp/xc32.run

# WORKDIR /home/user/tools/
# COPY ./deps/.mplab/tools/xc32-v4.45-full-install-linux-x64-installer.run .
# RUN ./xc32-v4.45-full-install-linux-x64-installer.run --mode unattended \
#     --unattendedmodeui none --netservername localhost --LicenseType FreeMode

# ENV PATH /opt/microchip/xc32/v${XC32_VERSION}/bin:$PATH

# RUN chown -R user:user /opt/microchip/

# # Final layer with project setup
# FROM mplab-setup AS project
FROM user-setup AS project-setup
ARG WDIR=/fprime-atmel-sam-reference
ARG GIT_BRANCH
ARG GIT_COMMIT

WORKDIR $WDIR
USER root
RUN chown -R user:user $WDIR

USER user
RUN git clone https://github.com/ReggieMarr/fprime-atmel-sam-reference.git $WDIR && \
    git fetch && \
    git checkout $GIT_BRANCH && \
    git reset --hard $GIT_COMMIT && \
    git submodule update --init --depth 1 --recommend-shallow

WORKDIR $WDIR/deps/FreeRTOS
# Bring in FreeRTOS sources and demo's
RUN git submodule update --init --recommend-shallow FreeRTOS/Source
RUN git submodule update --init --recommend-shallow FreeRTOS/Demo/ThirdParty/Community-Supported-Demos
RUN git submodule update --init --recommend-shallow FreeRTOS/Demo/ThirdParty/Partner-Supported-Demos

WORKDIR $WDIR/deps
RUN git submodule update --init  --recommend-shallow mplab_dev_packs
RUN git submodule update --init --recommend-shallow fprime

# Create virtual environment
USER root
RUN chown -R user:user $WDIR/deps

ENV VIRTUAL_ENV=/home/user/venv
RUN python3 -m venv $VIRTUAL_ENV
ENV PATH="$VIRTUAL_ENV/bin:$PATH"
# ENV PYTHONPATH="$VIRTUAL_ENV/lib/python$(python3 -c 'import sys; print(".".join(map(str, sys.version_info[:2])))')/site-packages:$PYTHONPATH"

# Set ownership
WORKDIR $WDIR
RUN chown -R user:user $VIRTUAL_ENV

RUN apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

USER user

# Activate virtual environment in various shell initialization files
RUN echo "source $VIRTUAL_ENV/bin/activate" >> ~/.bashrc && \
    echo "source $VIRTUAL_ENV/bin/activate" >> ~/.profile

# Upgrade pip in virtual environment
RUN pip install --upgrade pip

# Install Python packages (now using pip directly in virtualenv)
RUN pip install setuptools_scm fprime-tools && \
    pip install -r $WDIR/deps/fprime/requirements.txt

WORKDIR $WDIR
