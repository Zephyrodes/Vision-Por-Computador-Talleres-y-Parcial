#!/bin/bash

cd build

if [ "$1" == "taller1" ]; then

    if [ -f "./C1Taller1" ]; then
        ./C1Taller1
    else
        echo "Error: C1Taller1 no compilado"
    fi

elif [ "$1" == "taller2" ]; then

    if [ -f "./C1Taller2" ]; then
        ./C1Taller2
    else
        echo "Error: C1Taller2 no compilado"
    fi

else

    echo "Uso:"
    echo "./run.sh taller1"
    echo "./run.sh taller2"

fi
