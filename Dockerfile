FROM ubuntu:20.04

# Install dependencies
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    # build-essential \
    cmake \
    libopencv-dev \
    libcurl4-openssl-dev \
    libspdlog-dev \
    git && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Set the working directory
WORKDIR /app

# Copy the entire project to the container
COPY . .

# Create build directory and compile the project
RUN mkdir -p build && cd build && \
    cmake .. && \
    make
