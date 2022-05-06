#!/bin/bash

rm -r usr_src/
mkdir usr_src
sshfs root@localhost:/usr/src/ usr_src/ -p 10022
