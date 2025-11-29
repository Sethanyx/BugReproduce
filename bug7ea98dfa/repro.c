#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sched.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <linux/types.h>
#include <errno.h>

#ifdef __x86_64__
#define __NR_sched_setattr 314
#endif

#define SCHED_DEADLINE 6

struct sched_attr {
    __u32 size;
    __u32 sched_policy;
    __u64 sched_flags;
    __s32 sched_nice;
    __u32 sched_priority;
    __u64 sched_runtime;
    __u64 sched_deadline;
    __u64 sched_period;
};

int sched_setattr(pid_t pid, const struct sched_attr *attr, unsigned int flags) {
    return syscall(__NR_sched_setattr, pid, attr, flags);
}

void join_cgroup(const char *cgroup_name) {
    char path[128];
    snprintf(path, sizeof(path), "/sys/fs/cgroup/cpuset/%s/tasks", cgroup_name);
    int fd = open(path, O_WRONLY);
    if (fd < 0) {
        system("mkdir -p /sys/fs/cgroup/cpuset");
        system("mount -t cgroup -o cpuset cpuset /sys/fs/cgroup/cpuset");
        fd = open(path, O_WRONLY);
        if (fd < 0) exit(1);
    }
    pid_t tid = syscall(SYS_gettid);
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", tid);
    write(fd, buf, strlen(buf));
    close(fd);
}

void burn_cpu(unsigned long us) {
    struct timespec start, now;
    clock_gettime(CLOCK_MONOTONIC, &start);
    while (1) {
        clock_gettime(CLOCK_MONOTONIC, &now);
        unsigned long elapsed = (now.tv_sec - start.tv_sec) * 1000000 + 
                                (now.tv_nsec - start.tv_nsec) / 1000;
        if (elapsed >= us) break;
    }
}

pthread_mutex_t lock;

void *task_a(void *arg) {
    join_cgroup("dl_cpu2");

    // Runtime: 10ms
    // Period:  2000ms
    // 结果: A 跑完 10ms 后，会有接近 2 秒的时间处于 Throttled 状态。
    struct sched_attr attr = {
        .size = sizeof(attr),
        .sched_policy = SCHED_DEADLINE,
        .sched_runtime  = 10 * 1000 * 1000,   // 10ms
        .sched_deadline = 2000 * 1000 * 1000, // 2s
        .sched_period   = 2000 * 1000 * 1000  // 2s
    };
    
    if (sched_setattr(0, &attr, 0) < 0) {
        perror("Task A sched_setattr");
        return NULL;
    }

    pthread_mutex_lock(&lock);
    
    burn_cpu(20 * 1000); 

    pthread_mutex_unlock(&lock);
    return NULL;
}

void *task_b(void *arg) {
    join_cgroup("dl_cpu3");
    
    usleep(500000); 

    struct sched_attr attr = {
        .size = sizeof(attr),
        .sched_policy = SCHED_DEADLINE,
        .sched_runtime  = 10 * 1000 * 1000,
        .sched_deadline = 50 * 1000 * 1000,
        .sched_period   = 50 * 1000 * 1000
    };
    if (sched_setattr(0, &attr, 0) < 0) {
        perror("Task B sched_setattr");
        return NULL;
    }

    pthread_mutex_lock(&lock);
    pthread_mutex_unlock(&lock);
    return NULL;
}

int main() {
    pthread_mutexattr_t mattr;
    pthread_mutexattr_init(&mattr);
    pthread_mutexattr_setprotocol(&mattr, PTHREAD_PRIO_INHERIT);
    pthread_mutex_init(&lock, &mattr);

    pthread_t t1, t2;
    pthread_create(&t1, NULL, task_a, NULL);
    pthread_create(&t2, NULL, task_b, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}
