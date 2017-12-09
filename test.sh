#!/bin/bash

if [ $# -eq 1 ] # if args
then
    if [ $1 -ge 1 ] && [ $1 -le 3 ]
    then
        # Remake project
        make
        cd build/bin
        # Launch project with config file parameters
        echo 
        echo "Test config"
        echo CONFIG: config$1.ini
        ./main ../../data/config/config$1.ini
    else
        echo no such config file [config$1.ini]. Aborted.
        echo choose \'1-3\' configs, or use default parameters    
    fi
else # no args
    # Make project
    make
    cd build/bin
    # Launch project with default parameters
    echo 
    echo "Test default"
    ./main 
fi