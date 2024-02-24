#!/bin/bash

# Specify the folder path
folder_path="."

# Change to the specified folder
cd "$folder_path" || exit

# Delete all files except .cpp files
find . -type f ! -name "*.cpp" ! -name "*.sh" -exec rm -f {} +

echo "Deleted all files except .cpp files in $folder_path"
