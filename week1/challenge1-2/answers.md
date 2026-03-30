1. Response time is 200 ms, and the turn around is 400.
2. With 100, 200, and 300, the response time is 133.33 and the turnaround is 333.33
3. With the RR scheduler and a time step of 1, our response time is only 1 ms, but the turnaround is 465.67 ms.
4. In both workloads, both with the same job length and with different lengths, FIFO and RR were the same.
5. RR and SJF work the same if the the job lengths are the same and the time slice is the same length as the job length.
6. As the job length increases, so does the response time.
7. As the quantum length increases so does the response time, but the average turn around time decreases. The worst case response time given is (Sum of job lengths / N jobs) 