FROM ubuntu:18.04
LABEL drone-make-load2.version="0.1"

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
    wget \
    unzip \
    software-properties-common \
    gcc-arm-none-eabi \    
    && rm -rf /var/lib/apt/lists/*

RUN cd /root/ && \
    git clone https://github.com/bitcraze/crazyflie2-nrf-firmware.git && \
    cd crazyflie2-nrf-firmware && \
    git checkout 6f4b16a452665583821ef0586d24855f2ec4ac76

RUN pip3 install --upgrade pip && \
    pip3 install pip setuptools

RUN cd /root/ && \
    git clone https://github.com/bitcraze/crazyflie-clients-python && \
    cd crazyflie-clients-python && \
    git checkout 94504c1b6de85d0695b84590ceb6b074f66e5c6d && \
    python3 -m pip install -e .

RUN pip3 install -Iv cflib==0.1.13.1

WORKDIR /root/crazyflie2-nrf-firmware

RUN ./tools/build/download_deps

RUN make clean &&\
	make BLE=0

CMD make cload BLE=0
