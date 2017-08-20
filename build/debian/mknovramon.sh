#!/bin/sh

name=novramon

DPKG_COLORS="never"
export DPKG_COLORS

cd $name/build/debian
./mk.sh
