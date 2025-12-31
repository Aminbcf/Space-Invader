#!/bin/bash

echo "Running Space Invaders Tests..."
echo "================================"

# Build and run tests
make clean
make test

if [ $? -eq 0 ]; then
    echo "All tests passed!"
else
    echo "Some tests failed!"
    exit 1
fi

echo ""
echo "Running Valgrind memory check..."
echo "================================"
make valgrind