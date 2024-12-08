# Base image with common system packages
FROM ubuntu:24.04 AS base
ARG DEBIAN_FRONTEND=noninteractive
ENV TZ='America/Montreal'
RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y \
        sudo build-essential git cmake wget curl rsync libpython3-dev \
        python3-full python3-dev python3-wheel python3-pip pipx \
        udev gdbserver libusb-1.0-0 && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

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

# MPLAB installation layer
FROM user-setup AS mplab-setup
ENV MPLABX_VERSION=6.20
WORKDIR /tmp

# Download and extract MPLAB X
RUN curl -qgb "" -fLC - --retry 3 --retry-delay 3 \
    -e "https://www.microchip.com/en-us/tools-resources/develop/mplab-x-ide" \
    -o mplabx-installer.tar \
    "https://ww1.microchip.com/downloads/aemDocuments/documents/DEV/ProductDocuments/SoftwareTools/MPLABX-v${MPLABX_VERSION}-linux-installer.tar" && \
    tar xf mplabx-installer.tar && \
    rm mplabx-installer.tar

# Install MPLAB X
RUN USER=root ./MPLABX-v${MPLABX_VERSION}-linux-installer.sh --nox11 \
    -- --unattendedmodeui none --mode unattended && \
    rm ./MPLABX-v${MPLABX_VERSION}-linux-installer.sh

# Final layer with project setup
FROM mplab-setup AS final
ARG WDIR=/fprime-atmel-sam-reference
ARG GIT_BRANCH
ARG GIT_COMMIT

WORKDIR $WDIR
RUN chown -R user:user $WDIR

USER user
RUN git clone https://github.com/ReggieMarr/fprime-atmel-sam-reference.git $WDIR && \
    git fetch && \
    git checkout $GIT_BRANCH && \
    git reset --hard $GIT_COMMIT && \
    git submodule update --init --recursive --depth 1 --recommend-shallow

USER root
RUN apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

USER user
WORKDIR $WDIR
