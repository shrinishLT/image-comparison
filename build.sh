# remove existing docker images
docker rm -f compare-engine-container

# Build the Docker image (if not already built)
docker build -t compare-engine .

# run the container in detached mode 
docker run -d --name compare-engine-container compare-engine

# Copy the binary from the Docker container to the local machine
docker cp compare-engine-container:/app/build/image_comparison ./lib/compare-engine-bin

docker cp compare-engine-container:/tmp/lib64.tar.gz ./lib64.tar.gz
mkdir -p ./lib/lib64
sudo tar -xzvf ./lib64.tar.gz -C ./lib/lib64 --strip-components=2


# Stop the Docker container
docker stop compare-engine-container

# Remove the Docker container
docker rm -f compare-engine-container

