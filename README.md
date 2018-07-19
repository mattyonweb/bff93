# bff93 - Befunge93 interpreter

A simple but strict befunge93 interpreter written in C. 

Some notes:
- The interpreter may have bugs.
- Every cell has dimension of `sizeof(unsigned char)`, usually 1 byte; every value stored has to be considered modulo maxValue, usually 255.
- It will probably not work with sources written in befunge98.
- In the directory `utils` you'll find a funny python script which converts the files in the `tests` directory into mondrian-esque pictures. The script is very hacky but should work; requires `PIL` and `python3`.   

### Installation
    git clone https://github.com/mattyonweb/bff93.git
    cd bff93
    make

To run the tests present in the folder `tests`, type:

    cd tests
    python test.py

To mondrian-esquize your sources:

    cd utils
    python piet.py
