FROM ubuntu:18.04
LABEL drone-make-load.version="0.1"

RUN apt-get update && apt-get install -y \
    build-essential \
    dpkg \ 
    git \
    pkg-config \
    python3.8 \
    python3-pip \
    usbutils \
    nano \
    python-dev \
    python-numpy \
    sudo \
    software-properties-common \
    gcc-arm-none-eabi \    
    && rm -rf /var/lib/apt/lists/*

RUN pip3 install --upgrade pip && \
    pip3 install pip setuptools

RUN cd /root/ &&\
    git clone https://github.com/bitcraze/crazyflie-firmware.git && \
    cd /root/crazyflie-firmware && \
    git checkout 6e87f6ed5b99a0d0d1ffdf691f9f55676137726c && \
    git submodule init && \
    git submodule update

RUN cd /root/ && \
    git clone https://github.com/bitcraze/crazyflie-clients-python && \
    cd crazyflie-clients-python && \
    git checkout 94504c1b6de85d0695b84590ceb6b074f66e5c6d && \
    python3 -m pip install -e .

RUN pip3 install -Iv cflib==0.1.13.1

COPY Drone/embedded /root/embedded
COPY Drone/tools /root/embedded/tools

WORKDIR /root/embedded/src

RUN make clean &&\
	make

CMD make cload
