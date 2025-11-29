qemu-system-x86_64 \
  -name "sched-dl-bug-repro" \
  -m 4G \
  -smp 4 \
  -kernel ./linux/arch/x86/boot/bzImage \
  -drive file=./image/bullseye.img,format=raw,index=0,media=disk \
  -nographic \
  -cpu max \
  -net user,hostfwd=tcp::10022-:22 \
  -net nic,model=virtio \
  -append "console=ttyS0 root=/dev/sda earlyprintk=serial \
           isolcpus=2,3 \
           nohz_full=2,3 \
           rcu_nocbs=2,3 \
           nmi_watchdog=0 \
           nowatchdog \
           idle=poll \
           selinux=0 enforcing=0 \
           systemd.unified_cgroup_hierarchy=0"
