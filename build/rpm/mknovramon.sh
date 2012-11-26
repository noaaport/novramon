#!/bin/sh

branchname=novramon

cd ${branchname}/build/rpm
make clean
make package
