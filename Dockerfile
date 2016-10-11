FROM ubuntu:xenial
RUN apt-get update && apt-get install -y \
   cmake \
   git \
   libboost-all-dev \
   libeigen3-dev \
   libhdf5-dev \
   libtbb-dev \
   qt5-default \
   libqwt-dev \
   doxygen
RUN apt-get install -y \
   build-essential \
   libqt5svg5-dev \
   qtdeclarative5-dev \
   qtscript5-dev \
   qtpositioning5-dev \
   libinsighttoolkit4-dev

ADD . /build
WORKDIR /build
RUN ln -s  /usr/lib/x86_64-linux-gnu/hdf5/serial  /usr/include/hdf5/lib
RUN cmake -DDREAM3D_USE_QtWebEngine=OFF -DSIMPLib_USE_MULTITHREADED_ALGOS=OFF \
          -DQWT_LIBRARY=/usr/lib/libqwt.so -DQWT_INCLUDE_DIR=/usr/include/qwt \
          -DSIMPLib_USE_ITK=OFF -DHDF5_LIBRARY_DIRS=/usr/lib/x86_64-linux-gnu/hdf5/serial \
          -DCMAKE_CXX_STANDARD=11 -DEIGEN_INCLUDE_DIR=/usr/include/eigen3 . && make
