#!/bin/bash
qemu-img create -f qcow2 -F raw -o backing_file=/home/students/inf/PUBLIC/SO/scenariusze/4/minix.img minix.img
qemu-system-x86_64 -nographic -drive file=minix.img -rtc base=localtime -net user,hostfwd=tcp::15881-:22 -net nic,model=virtio -m 1024M -enable-kvm &
sleep 2
scp -P 15881 ../*.patch root@localhost:../
ssh -p 15881 -o "ServerAliveInterval 2" root@localhost 'bash -s' < install_and_reboot.sh
sleep 3
scp -r -P 15881 tests root@localhost:
ssh -p 15881 root@localhost 'bash -s' < run_tests.sh
