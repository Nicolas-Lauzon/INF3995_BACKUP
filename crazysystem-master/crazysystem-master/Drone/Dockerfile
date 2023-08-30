FROM ubuntu:18.04
LABEL drone-sim.version="0.1"

# Install common dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    dpkg \ 
    git \
    pkg-config \
    python \
    nano \
    python-dev \
    python-numpy \
    sudo \
    software-properties-common \
    gcc-arm-none-eabi \   
    gdb \ 
    && rm -rf /var/lib/apt/lists/*

# Install ARGoS dependencies
RUN apt-get update && apt-get install -y \
    wget \
    freeglut3-dev \
    qt5-default \
    libxi-dev \
    libxmu-dev \
    libfreeimage-dev \
    libfreeimageplus-dev \
    liblua5.2-dev \
    lua5.2 \
    liblua5.3-dev \
    lua5.3 \
    libboost-filesystem-dev \
    cmake \
    sdcc \
    binutils \
    && rm -rf /var/lib/apt/lists/*

# Add dummy argument to force rebuild starting from that point
ARG UPDATE_ARGOS=unknown
ARG UPDATE_CODE=unknown

# Install Argos from source
RUN cd /root/ &&\
    git clone https://github.com/MISTLab/argos3.git &&\
    cd argos3 &&\
    git checkout inf3995 &&\
    mkdir build_simulator &&\
    cd build_simulator &&\
    cmake ../src -DCMAKE_BUILD_TYPE=Debug \
     -DARGOS_BUILD_FOR=simulator \
     -DARGOS_THREADSAFE_LOG=ON \
     -DARGOS_DYNAMIC_LOADING=ON &&\
    make -j $(nproc)
RUN touch /root/argos3/build_simulator/argos3.1.gz &&\
    touch /root/argos3/build_simulator/README.html &&\
    cd /root/argos3/build_simulator &&\
    make install
    
RUN chmod +x /root/argos3/build_simulator/argos_post_install.sh &&\
    ./root/argos3/build_simulator/argos_post_install.sh &&\
    echo "\nsource /root/argos3/build_simulator/setup_env.sh\n" >> /.bashrc

ARG FILES_TO_COPY_SIM

ARG FILES_TO_COPY_LIBRARY

# Copy source files to container
COPY ${FILES_TO_COPY_SIM} /root/sim
COPY ${FILES_TO_COPY_LIBRARY} /root/sim/tools

# Compile source files
RUN cd /root/sim/ &&\
	rm -r -f build &&\
	mkdir build &&\
	cd build &&\
	cmake -DCMAKE_BUILD_TYPE=Debug .. &&\
	make
