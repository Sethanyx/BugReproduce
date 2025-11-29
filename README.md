# BugReproduce

```bash
# Build Two Kernels

# With Bug
./setup_kernel 829c1651e9c4a6f78398d3e67651cef9bb6b42cc 
./setup_image

# Without Bug
./setup_kernel cab3ecaed5cdcc9c36a96874b4c45056a46ece45
./setup_image

```

## Bug7ea98dfa
```bash
./qemu.sh
# Open another terminal and Copy repro.c setup.sh run_loop into qemu virtual machine
gcc -o repro repro.c -lpthread
./setup.sh
./run_loop.sh
```
```bash
# Expected Output For Kernel Version With Bug
root@schedtest:~/bug7ea98dfa# ./run_loop.sh
Starting stress test...
Run #1: !!! HANG DETECTED !!! Bug Reproduced!
CPU 2 is stuck in idle. Process 301 is waiting for a lock that will never be released.

# Expected Output For Kernel Version Without Bug
root@schedtest:~/bug7ea98dfa# ./run_loop.sh
Starting stress test...
Run #1: Finished.
Run #2: Finished.
Run #3: Finished.
Run #4: Finished.
Run #5: Finished.
...

```
