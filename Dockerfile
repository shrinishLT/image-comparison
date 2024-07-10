FROM amazonlinux:2

# Install dependencies
RUN yum update -y && \
    yum install -y \
    gcc \
    gcc-c++ \
    make \
    git \
    libcurl-devel \
    tar \
    gzip \
    which && \
    yum clean all

# Download and install CMake
RUN cd /opt && \
    curl -O https://cmake.org/files/v3.21/cmake-3.21.0-linux-x86_64.tar.gz && \
    tar -zxvf cmake-3.21.0-linux-x86_64.tar.gz && \
    rm cmake-3.21.0-linux-x86_64.tar.gz && \
    ln -s /opt/cmake-3.21.0-linux-x86_64/bin/cmake /usr/bin/cmake

# Install GTest
RUN cd /opt && \
    git clone https://github.com/google/googletest.git && \
    cd googletest && \
    cmake . && \
    make && \
    make install

# Download and install nlohmann_json
RUN cd /opt && \
    git clone https://github.com/nlohmann/json.git && \
    cd json && \
    mkdir build && cd build && \
    cmake .. && \
    make install

# Install OpenCV
RUN yum install -y opencv-devel

# Set the working directory
WORKDIR /app

# Copy the entire project to the container, excluding files specified in .dockerignore
COPY . .

# Create build directory and compile the project
RUN mkdir -p build && cd build && \
    cmake -DCMAKE_PREFIX_PATH=/usr/local -DCMAKE_CXX_STANDARD=17 .. && \
    make

# Create a tarball of /usr/lib64
RUN tar -czvf /tmp/lib64.tar.gz /usr/lib64


