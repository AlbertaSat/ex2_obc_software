# sudo docker build --tag docgs  .
FROM ubuntu

# build this repo: EDIT THIS IF THE BUILD COMMAND CHANGES DUE TO ADDED FILES ETC.


# General environment setup
WORKDIR /
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update
RUN apt-get install build-essential -y
RUN apt-get install wget -y
RUN apt-get install gcc-multilib g++-multilib -y


RUN apt install git -y

# install python
RUN apt-get update \
  && apt-get install -y python3-pip python3-dev \
  && cd /usr/local/bin \
  && ln -s /usr/bin/python3 python \
  && pip3 install --upgrade pip

# install zmq
RUN wget https://github.com/zeromq/libzmq/releases/download/v4.2.2/zeromq-4.2.2.tar.gz
RUN tar xvzf zeromq-4.2.2.tar.gz
RUN apt-get install -y libtool pkg-config build-essential autoconf automake uuid-dev
WORKDIR /zeromq-4.2.2
RUN ./autogen.sh
RUN ./configure
RUN make && make install
RUN apt-get install libzmq5 -y

WORKDIR /home/
RUN git clone https://github.com/AlbertaSat/SatelliteSim.git
WORKDIR /home/SatelliteSim
# TODO: change this if we ever move away form the 64 bit branch:
RUN git checkout 64bit
RUN git submodule init
RUN git submodule update

# build CSP
WORKDIR /home/SatelliteSim/libcsp
RUN git checkout satellite_sim
RUN python3 waf configure --with-os=posix --enable-rdp --enable-hmac --enable-xtea --with-loglevel=debug --enable-debug-timestamp --enable-python3-bindings --with-driver-usart=linux --enable-if-zmqhub --enable-examples
RUN python3 waf build

# build this codebase
WORKDIR /home/ex2_command_handling_demo
COPY . .
RUN gcc *.c Platform/demo/*.c Platform/demo/hal/*.c Services/*.c -c -I . -I Platform/demo -I Platform/hal -I Services/ -I ../upsat-ecss-services/services/ -I ../SatelliteSim/Source/include/ -I ../SatelliteSim/Project/ -I ../SatelliteSim/libcsp/include/ -I ../SatelliteSim/Source/portable/GCC/POSIX/ -I ../SatelliteSim/libcsp/build/include/ -lpthread -std=c99 -lrt && ar -rsc client_server.a *.o

WORKDIR /home/SatelliteSim
RUN make clean && make all
CMD ./libcsp/build/zmqproxy & ./SatelliteSim
