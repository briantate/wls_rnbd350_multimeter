# Use the latest version of Ubuntu as the base image
FROM ubuntu:22.04

# Set the maintainer label
LABEL maintainer="brian.c.tate@gmail.com"

# Set environment variables to non-interactive (this will prevent some prompts)
ENV DEBIAN_FRONTEND=non-interactive

# Update package lists, install basic tools, toolchains, stlink-tools, and clean up
RUN apt-get update -y && \
    apt-get install -y --no-install-recommends \
    autoconf \
    automake \
    curl \
    build-essential \
    git \
    libtool \
    make \
    pkg-config \
    ca-certificates \
    software-properties-common \
    clang-format \
    clang-tidy \
    pmccabe \
    python3 \
    python3-pip \
    stlink-tools \
    cmake \
    xz-utils \
    ninja-build && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/* && \
    pip3 install setuptools

# Install some necessary tools and libraries
RUN apt-get update && \
    apt-get install -y wget libssl-dev netcat-openbsd

# Install additional analysis tools
RUN pip install gcovr

# Install Ruby and Ceedling (C unit testing framework)
RUN apt-get update && \
    apt-get install -y ruby-full && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/* && \
    gem install ceedling --no-document

# Download and install the latest CMake
# This is an example of how to download and install the latest version of cmake. The linux
# packages often are behind on versions. 
# Be warned! This will take a while to do. . .
#WORKDIR /tmp
#RUN wget "https://github.com/Kitware/CMake/releases/download/v3.27.7/cmake-3.27.7.tar.gz" \
#    && tar -xzf cmake-3.27.7.tar.gz \
#    && cd cmake-3.27.7 \
#    && ./bootstrap \
#    && make \
#    && make install

# Setup a tool directory in /home/dev and download the ARM toolchain
WORKDIR /home/dev

# Install GNU Arm toolchain (Version 14.2.Rel1)
RUN cd /home/dev && \
    curl -LO https://developer.arm.com/-/media/Files/downloads/gnu/14.2.rel1/binrel/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi.tar.xz && \
    tar xf arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi.tar.xz && \
    rm arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi.tar.xz

# Add the GNU Arm toolchain to the PATH and set ARMGCC_DIR environment variable
ENV PATH="/home/dev/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi/bin:${PATH}"
ENV ARMGCC_DIR="/home/dev/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi"

# Install Renode (embedded systems emulator) via .deb package
WORKDIR /tmp
RUN curl -LO https://github.com/renode/renode/releases/download/v1.15.3/renode_1.15.3_amd64.deb && \
    dpkg -i renode_1.15.3_amd64.deb || true && \
    apt-get update && \
    apt-get install -f -y && \
    rm renode_1.15.3_amd64.deb

# Clone, build, and install CppUTest
WORKDIR /home
RUN git clone https://github.com/cpputest/cpputest.git && \
    cd cpputest && \
    ./autogen.sh && \
    ./configure && \
    make install

# Set the CPPUTEST_HOME environment variable
ENV CPPUTEST_HOME=/home/cpputest

# Create and set the working directory to /home/app
WORKDIR /home/app

# Set the default command to bash
CMD ["/bin/bash"]