cd ..
patch -p1 < *.patch
cd /usr/src/minix/servers/pm
make
make install
cd ../../../lib/libc
make
make install
cd ../../releasetools
make do-hdboot
/sbin/reboot
