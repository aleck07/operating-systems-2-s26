1. With two jobs, the first one 12 ms and the other 16. I think the total time will obviously take 28 ms. I think each job will be in the queues for 10 ms each.

2. For example 1: `mlfq.py -n 3 -j 1 -q 10`

- For example 2: `mlfq.py -n 3 -q 10 -l 0,200,0:100,20,0`
- For example 3: `mlfq.py -n 3 -q 10 -l 0,200,0:0,50,1 -i 5 -S -c`

3. To make the scheduler work like round robin, we only have one priority level. `mlfq.py -n 1 -q 10 -c`

4. To game the scheduler we can do `python3 mlfq.py -n 3 -q 50 -l 0,1000,49:0,1000,0 -i 1 -S -c`

5. I believe to get the process to have at least 5% CPU time we set the boost to 20.

6. When using the -I flag, it was faster in the previous example using the boost.