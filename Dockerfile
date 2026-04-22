FROM ubuntu:22.04

RUN apt-get -qq update && \
    apt-get -qq upgrade && \
    apt-get -qq -y install bc build-essential curl git python3 python3-pip unzip wget && \
    python3 -m pip install conan

# Install CMake 3.28
RUN version=3.28 && \
    build=1 && \
    limit=3.20 && \
    result=$(echo "$version >= $limit" | bc -l) && \
    url="https://cmake.org/files/v$version/cmake-$version.$build-linux-x86_64.sh" && \
    mkdir /temp && \
    cd /temp && \
    wget --no-verbose $url && \
    mkdir /opt/cmake && \
    sh cmake-$version.$build-linux-x86_64.sh --prefix=/opt/cmake --skip-license && \
    ln -s /opt/cmake/bin/cmake /usr/local/bin/cmake
   
ADD . /zorita
WORKDIR /zorita

# Build
RUN conan profile detect --force && \
    conan build . -pr:a=conan/profiles/tests-debug-gcc-linux-x64 -b missing
