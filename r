#!/bin/bash

# Quick build and run script

cd build && make -j$(nproc) && ./terrain3d