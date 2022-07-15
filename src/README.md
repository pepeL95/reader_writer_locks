Fast Compilation:
    For fast build: use the "make" command.
    Once built, to run it use the following format:
    ./memsim <pathToTraceFile> <numberOfFrames> <fifo|lru|sfifo> <quiet|debug>

Fast Delete: 
    When done use the "make clean" command

Note:
Please, be aware that the code is intended to work with a maximum of 12 threads per scenarios, and a total of 4 scenarios are provided. If you want to change the number of threads per scenario, or increase/decrease the number of scenarions make sure you change the THREADS_PER_SCENARIO and  NUMBER_OF_SCENARIOS global definitions within the code accordingly. Otherwise, just run it with the provided scenarios.txt file provided.


