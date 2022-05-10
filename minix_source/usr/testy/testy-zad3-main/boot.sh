#!/bin/bash

cd
cd /usr/src/minix/servers/pm/
make && make install
cd
cd /usr/src/lib/libc/
make && make install
cd
cd /usr/src/releasetools/
make do-hdboot
reboot
