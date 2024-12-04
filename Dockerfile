FROM ubuntu:24.04 AS freesam-base

# Set non-interactive installation mode for apt packages
ENV TZ='America/Montreal'
ARG DEBIAN_FRONTEND=noninteractive

# Install all necessary packages
RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y \
    sudo build-essential git cmake wget curl rsync libpython3-dev \
    python3-full python3-dev python3-wheel python3-pip pipx \
    udev gdbserver && \
    apt-get clean

# Set the working directory for software
ARG WDIR=/freeSAM
ARG HOST_UID=1000
ARG HOST_GID=1000

# Create a non-root user
RUN userdel -r ubuntu || true && \
    getent group 1000 && getent group 1000 | cut -d: -f1 | xargs groupdel || true && \
    groupadd -g ${HOST_GID} user && \
    useradd -u ${HOST_UID} -g ${HOST_GID} -m user && \
    echo 'user ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

# Add user to dialout group for serial access
RUN groupadd -f dialout && usermod -a -G dialout user

# Configure USB permissions
RUN echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="2341", ATTR{idProduct}=="003d", MODE="0666", GROUP="dialout"' > /etc/udev/rules.d/99-arduino.rules

# Set ownership
WORKDIR $WDIR
RUN chown -R user:user $WDIR

# Switch to non-root user
USER user

# Add Python install path to PATH
ENV PATH="/home/user/.local/bin:${PATH}"

# Example use of pipx to manage python bin installs in a virtual env
# RUN python3 -m pipx install yamcs-client xlrd --include-deps

# FROM freesam-src AS freesam-base
# # Clone the repository
RUN git clone https://github.com/ReggieMarr/fprime-atmel-sam-references.git $WDIR
RUN git fetch
RUN git checkout $GIT_BRANCH
RUN git reset --hard $GIT_COMMIT
RUN cd ..
RUN git submodule update --init --recursive --depth 1 --recommend-shallow

# Cleanup to reduce image size
USER root
RUN apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

#Ensure we start up with the "user" user
USER user
