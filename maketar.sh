#! /bin/bash
rm -f distanceVector.tar.gz
tar -cvzf distanceVector.tar.gz *
scp distanceVector.tar.gz sushivan@lh008linux-01.soic.indiana.edu:/u/sushivan/dv

