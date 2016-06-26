import multiprocessing
import subprocess
from sys import argv

# Very quick and dirty script. No checking for argv but python error messages
# are very descriptive. Gets the job done. Runs multiple client connections at
# the same time. It has been verified in the logs that they are not
# running serially.

# Running this on the same machine as the server seems to crash the server.
# Something about resource stealing or forking or whatnot.

address = "mm.danielporteous.xyz"
port = argv[2]
inp = "test_input.txt"

def test_server(num):
	subprocess.check_output('./client %s %s < %s' % (address, port, inp), shell=True)
	print("Running number " + str(num))

def test_server_multiple(numIterations):
	output = []
	for i in range(0, numIterations):
		output.append(i)

	pool = multiprocessing.Pool(numIterations)
	pool.map(test_server, output)

test_server_multiple(int(argv[1]))
