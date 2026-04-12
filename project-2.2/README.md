Question 1: The writer count doesn't need to be atomic because we change the writer_count value within the run_writer which is always called wth a write lock held.

Question 2: Writers can be starved in this situation if there are several readers holding the lock, and a write_lock starts spinning. The current reader finishes and another thread calls the read_lock. No write bit was ever written yet so these new reaers can enter the critical section. By this point, the writer will spin forever because there are more and more readers taking the lock.
