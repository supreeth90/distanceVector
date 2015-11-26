#! /bin/bash
rm -f distanceVector.tar.gz
tar -cvzf distanceVector.tar.gz *
scp distanceVector.tar.gz singhgag@lh008linux-01.soic.indiana.edu:/u/singhgag/dv
scp distanceVector.tar.gz singhgag@lh008linux-01.soic.indiana.edu:/u/singhgag/dv2
scp distanceVector.tar.gz singhgag@lh008linux-01.soic.indiana.edu:/u/singhgag/dv3
