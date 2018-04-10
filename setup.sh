#!/bin/sh

# Removing stray text files
if ls -A *.txt >/dev/null 2>/dev/null
then
    rm *.txt >/dev/null 2>/dev/null
fi

# Removing stray object files
if ls *.out >/dev/null 2>/dev/null
then
    rm *.out >/dev/null 2>/dev/null
fi

# Compiling client
g++ client.cpp -o client.out

# Compiling server
g++ server.cpp -o server.out

# Compiling and running the initialization script
g++ initialize.cpp -o initialize
./initialize

# Deleting initialization script object file
rm initialize

echo "\n\nSetup successful. Check README.md for usage.\n\n"