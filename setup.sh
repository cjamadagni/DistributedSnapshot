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

# Running the initialization script
./initialize

# Deleting initialization script object file
rm initialize

echo "\n\nSetup successful. Check README.md for usage.\n\n"
