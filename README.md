Things I need:

- Individual instance of a game in a thread
- A way of keeping track of all threads (cyclical array of structs? 
but global incrementing thread number?)
- a way of instantiating threads 
- a way of binding threads to sockets

threads may not always match to the socket number.
for example, you could have multiple clients from the same IP.
This probably means you need one socket for the multiple threads.