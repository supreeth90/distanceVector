#! /bin/bash
rm -Rf dvnode.log include/ readme Release/ src/
tar -xvzf distanceVector.tar.gz
cd Release/
make clean;make all
