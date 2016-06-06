Final Result: 14.5/15

The 0.5 lost was because I didn't flush to buffer after writing to each line of the log.
I have since done so by adding the line `fflush(log_f);` in logging.c, which fixed it. 