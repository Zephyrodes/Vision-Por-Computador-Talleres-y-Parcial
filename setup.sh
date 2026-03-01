#!/bin/bash

echo "===================================="
echo "Instalando dependencias..."
echo "===================================="

sudo apt update

sudo apt install -y \
    build-essential \
    cmake \
    libopencv-dev \
    pkg-config

echo "===================================="
echo "Compilando proyecto..."
echo "===================================="

mkdir -p build
cd build

cmake ..
make -j$(nproc)

echo "===================================="
echo "Compilación completada"
echo "===================================="
echo "Para ejecutar:"
echo "./run.sh taller1"
echo "o"
echo "./run.sh taller2"
