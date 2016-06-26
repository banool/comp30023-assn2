# COMP30023 - Assignment 2
Assignment 2 for COMP30023 - Computer Systems.

## Overview
This is assignment 2 for the unimelb subject COMP30023 - Computer Systems, a multithreaded TCP/IP server and client for the game Mastermind.

The basic implementation of the server involves creating and binding a socket and then waiting for clients to connect. Upon connection, a new socket is created and passed to a thread which is spawned to handle that game instance. The game logic itself is fairly straightforward and is handled in one main function with a couple of helpers.

The task also included running the server on both the university machines as well as the NeCTAR research cloud and testing various runtime characteristics relating to memory usage, disk usage, scheduling, etc.

There were some notable problems which had to be overcome:

- Upon killing the server, we needed to write to log a collection of stats about the server's execution lifetime. These stats look into interesting characteristics using information from get_rusage() and /proc/, as well as basic information about the clients and their games.
- In order to do this, the server had to be able to catch a SIGINT or SIGTERM and write to log first before servicing it. Using a signal handler did not initially work because the accept() function, which sits there waiting for a client to connect, would block it if the handler was activated. There were two ways to overcome this:
    1. Put the accept() part of the code inside its own thread. Upon receiving the signal inside the parent thread, the child could just be stopped and then we would hopefully stop *its* child threads gracefully also.
    2. Wrap the accept() in a poll() command. This function sits there and checks the specified socket(s) for activity in a non-blocking manner. When it does detect incoming traffic, then the execution is passed forward to accept(). This prevents any blocking from occurring.
    
    Solution number 2 was what I went with since it allowed me greater power of the control flow inside the server process without subthreading, and I also feel like it is standard practice when working with sockets.

- When calling the pthread_create() function for a new process, you can only pass one argument through to it. As such, the program had to be designed with this in mind, making sure that the new thread had everything it needed to start. To solve this problem a struct called StateInfo was made which itself holds a list of structs (called Instance) for each of the currently running games. The Instance struct for each game would be initialised beforehand and then put into StateInfo. Once the thread was made, the appropriate Instance would be relocated using the thread id, which ended up working quite well.

The client itself was fairly straightforward to write. After receiving a welcome message, it comprises of a simple loop which waits for user input, sends it to the server, waits for the response and then repeats until it gets a message indicating that it has won, lost or run out of turns, at which point the game is over and execution ends.


## Directory structure

The root directory contains all the code and header files, including makefile, this readme, the spec. It also contains the final report.

- **testing** contains files used for testing the server under heavy load. This includes a text file with a single sample input set, as well as a python script which spawns a user defined number of clients simultaneously and bombards the server with game traffic. Note that the tester contained within should not be run on the same machine as the server or you will get weird results.

## Results

Once I get my results back for this assignment, I will post the result so as to provide an indication of its quality.

### Update

Final Result: 14.5/15

Client - Server: 3/3
Concurrency: 3/3
Log file: 2.5/3
Nectar cloud: 2/2
Code Quality: 2/2
Report: 2/2

The 0.5 lost was because I didn't flush to buffer after writing to each line of the log. I have since done so by adding the line `fflush(log_f);` in logging.c, which fixed the problem.
