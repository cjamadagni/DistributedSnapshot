#!/bin/sh

# Moving into data directory
cd Data

# Removing stray text files
if ls -A *.txt >/dev/null 2>/dev/null
then
    rm *.txt >/dev/null 2>/dev/null
fi

# Returning to root directory
cd ..

# Removing stray object files
if ls *.out >/dev/null 2>/dev/null
then
    rm *.out >/dev/null 2>/dev/null
fi

# Compiling client
g++ client.cpp socket-wrapper.cpp -o client.out

# Compiling server
g++ server.cpp socket-wrapper.cpp -o server.out

# Compiling and running the initialization script
g++ initialize.cpp -o initialize
./initialize

# Deleting initialization script object file
rm initialize

echo "\n\nSetup successful. Check README.md for usage.\n\n"
