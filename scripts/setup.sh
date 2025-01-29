#!/bin/bash

# Set error handling
set -e

echo "Setting up the Chess Game project..."

# Navigate to the server directory and build
echo "Building server..."
cd server
make
cd ..

# Navigate to the client directory and build
echo "Building client..."
cd client
make
cd ..

echo "Setup complete!"
echo "To run the server, navigate to the 'server' directory and run: ./server"
echo "To run the client, navigate to the 'client' directory and run: ./client <server_ip> <port>"
