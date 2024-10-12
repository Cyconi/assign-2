#!/usr/bin/env bash

# Function to display script usage
usage()
{
    echo "Usage: $0 <target_directory>"
    echo "  <target_directory> - The directory to which .flags will point"
    exit 1
}

# Check if exactly one argument is provided
if [ "$#" -ne 1 ]; then
    usage
fi

target_dir=$1

# Verify that the target directory exists
if [ ! -d "$target_dir" ]; then
    echo "Error: Target directory '$target_dir' does not exist."
    exit 1
fi

# If a symbolic link or directory already exists as .flags, remove it
if [ -L ".flags" ]; then
    echo "Removing existing symbolic link .flags"
    rm .flags
elif [ -d ".flags" ]; then
    echo "Error: A directory named .flags already exists. Please remove or rename it before proceeding."
    exit 1
fi

# Create the symbolic link
ln -s "$target_dir" .flags

echo "Symbolic link created: .flags -> $target_dir"
