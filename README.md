# bff93 - Befunge93 interpreter

A simple but strict befunge93 interpreter written in C, now with added support for threaded bf93 code.

![Mondrian-esquized befunge93 fibonacci](/out/07-fibonacci.bf93.jpg)

Some notes:
- The interpreter implements an extension of standard bf93: support for the `{` thread branching instruction is toggled with the `-t` flag. More on this below.
- The interpreter may have bugs.
- Every cell has a default dimension of `sizeof(char)`, usually 1 byte; in case of overflow, the value of a cell will wrap back to 0. It's possible to change the dimension of the cells by using the flag `-b <bits num>`, with a maximum of 32 bits. Cells values are signed.
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

#### Threads

The `-t` flag enables the execution of code written in a superset of bf93, where more than one instruction pointer (IP) can exist at the same time.

What does this mean? Let's look at this easy example (to execute it, remember to specify the `-t` flag, otherwise it won't work!):

    v   > 1 . @
    > 1 {
        > 2 . @

Initially, you have just one thread running (let's identify it with 0). When the IP encounters a `{`, the execution of the program is splitted and a new thread, 1, is created: his father's stack contents will be copied into the stack of the new thread. In the next turn, thread 0 will move -90 degrees from the old direction, while thread 1 will move +90 degrees instead. That is:

        ^ (0) (Root)       ^ (1)
    >>> {                  { <<<
        v (1) (Child)      v (0)
        
        
        v               <  {  >
        v              (0) ^ (1)
    (1) v (0)              ^
     <  {  >               ^
      
The 0-thread, or "root thread", will always inherit the father's thread number (0, in this case), while the 1-thread, or "child thread", will be given the smallest integer not yet used to identify a thread.

The root thread will always have precedence over the lower one. That means that 0 will always execute _before_ 1.

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
    ---------> TIME
    
When a thread reaches `@` it is terminated. When every thread has reached a `@` the program is halted.

#### Messages

One way to comunicate between threads is through the use of the `g` and `p` operators: for the memory is shared among all the threads, it is possible to reserve one or more cells of memory for sharing data. With this method, however, it's difficult to correctly synchronize/temporize threads and the results, albeit deteministic (see the preceding paragraph), are not trivial to predict. 

Hence two new operators are added in order to make synchronization a little bit easier: `s` (send) and `w` (wait). 

+ `w` pops a value from the stack and put the thread in the WAITING state. The thread will then be suspended until a message with the same content of the value popped before is received.
+ `s` pops a value from the stack and sends it to every thread (included itself).

As an example:

      > 1w 0.@
    > {
      >              1s@

The upper thread will put itself to bed with `1w`, waiting for the message `1`. The second thread, after some time, reaches the `1s` and hence sends the message `1` to every thread. At the next turn, the upper thread will check its mail and find the message `1`, which is what it was waiting for; it then procedes to unfreeze and to execute the remaining instructions ( `0.` will simply print the number `0`). 

#### Notes on threaded code

Note that executing normal bf93 source code with `bf93 -t` should not be a problem unless there are `{`, `w` or `s` floating around (in bf93 they would be interpreted as simple comments and ignored). It is not true the other way round: executing parallelized bf93 code without the `-t` flag will probably cause your program to loop.

### Mondrianesquizization

To mondrian-esquize your sources:

    cd utils
    python piet.py
