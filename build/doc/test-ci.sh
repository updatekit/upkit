#!/bin/bash -e

# This script will fail until all the Doxygen warnings will be solved

# Teach Doxygen to treat warnings as errors
sed -i -e 's/WARN_AS_ERROR          = NO/WARN_AS_ERROR          = YES/g' source/Doxyfile

# Execute Doxygen
cd source && doxygen
