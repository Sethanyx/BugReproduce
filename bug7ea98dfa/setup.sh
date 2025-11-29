#!/bin/bash

# 1. 确保 Debugfs 挂载
mountpoint -q /sys/kernel/debug || mount -t debugfs none /sys/kernel/debug

# 2. 挂载 cpuset (如果没挂载)
mkdir -p /sys/fs/cgroup/cpuset
mountpoint -q /sys/fs/cgroup/cpuset || mount -t cgroup -o cpuset cpuset /sys/fs/cgroup/cpuset

# 3. 关闭实时限流
sysctl -w kernel.sched_rt_runtime_us=-1

# 4. 清理旧环境
rmdir /sys/fs/cgroup/cpuset/dl_cpu2 2>/dev/null
rmdir /sys/fs/cgroup/cpuset/dl_cpu3 2>/dev/null

# 5. 创建 CPU 2 组 (独占)
mkdir -p /sys/fs/cgroup/cpuset/dl_cpu2
echo 2 > /sys/fs/cgroup/cpuset/dl_cpu2/cpuset.cpus
echo 0 > /sys/fs/cgroup/cpuset/dl_cpu2/cpuset.mems
echo 1 > /sys/fs/cgroup/cpuset/dl_cpu2/cpuset.cpu_exclusive
echo 1 > /sys/fs/cgroup/cpuset/dl_cpu2/cpuset.sched_load_balance

# 6. 创建 CPU 3 组 (独占)
mkdir -p /sys/fs/cgroup/cpuset/dl_cpu3
echo 3 > /sys/fs/cgroup/cpuset/dl_cpu3/cpuset.cpus
echo 0 > /sys/fs/cgroup/cpuset/dl_cpu3/cpuset.mems
echo 1 > /sys/fs/cgroup/cpuset/dl_cpu3/cpuset.cpu_exclusive
echo 1 > /sys/fs/cgroup/cpuset/dl_cpu3/cpuset.sched_load_balance

# 7. 重建调度域
echo 0 > /sys/fs/cgroup/cpuset/cpuset.sched_load_balance
echo 1 > /sys/fs/cgroup/cpuset/cpuset.sched_load_balance

echo "Setup Success! Directories created."
