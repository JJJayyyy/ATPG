#!/bin/bash

g++ -std=c++11 node.hpp circuit.hpp circuit.cpp cmd_parser.hpp mode.hpp main.cpp -o simulator

# if [g++ -std=c++11 node.hpp circuit.hpp circuit.cpp cmd_parser.hpp mode.hpp main.cpp -o simulator] 
# then
#     echo "---Program successfully compile---"
#     echo "---Use ./simulator to run---"
# fi