# bff93 - Befunge93 interpreter

A simple but strict befunge93 interpreter written in C, now with added support for threaded bf93 code.

![Mondrian-esquized befunge93 fibonacci](/out/07-fibonacci.bf93.jpg)

Some notes:
- The interpreter implements an extension of standard bf93: support for the `{` thread branching instruction is toggled with the `-t` flag. More on this below.
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

### `bf93` "threads" extension

The `-t` flag enables the execution of code written in a superset of bf93, where more than one instruction pointer (IP) can exist at the same time.

What does this mean? Let's look at this easy example (to execute it, remember to specify the `-t` flag, otherwise it won't work!):

    v   > 1 . @
    > 1 {
        > 2 . @

Initially, you have just one thread running (let's identify it with 0). When the IP encounters a `{`, the execution of the program is splitted and a new thread, 1, is created: his father's stack contents will be copied into the stack of the new thread. In the next turn, thread 0 will execute the instruction exactly above the `{`, while thread 1 the one below; the "upper" thread will always inherit the father's thread number (0, in this case), while the "lower" will be given the smallest integer not yet used to identify a thread.

The upper thread will always have precedence over the lower one. That means that 0 will always execute _before_ 1.

When more than one branch happens, the order of execution of the various threads will follow a tree structure. Let's see an example:

    v       > 0 @  
        > 0 {
    > 0 {   > 2 @
        v   > 1 @
        > 1 {
            > 3 @

Supposing that the initial thread ID is 0, the order of execution through the program is as follows:

    0
    |\
    0 1
    |\ \
    0 2 1
    | | |\
    0 2 1 3

When a thread reaches `@` it is terminated. When every thread has reached a `@` the program is halted.

Note that executing normal bf93 source code with `bf93 -t` should not be a problem unless, of course, there are `{` floating around (in bf93 they would be interpreted as simple comments and ignored). It is not true the other way round: executing parallelized bf93 code without the `-t` flag will probably cause your program to loop.

### Mondrianesquizization

To mondrian-esquize your sources:

    cd utils
    python piet.py
