#!/bin/bash
mv minix.img trashbin/minix.img
qemu-img create -f qcow2 -o backing_file=backup_minix.img minix.img
qemu-system-x86_64 -curses -drive file=minix.img -enable-kvm -rtc base=localtime -net user,hostfwd=tcp::10022-:22 -net nic,model=virtio -m 1024M
