#! /bin/bash
#user="sushivan"
user="singhgag"
rm -f distanceVector.tar.gz
tar -cvzf distanceVector.tar.gz *
scp distanceVector.tar.gz $user@lh008linux-01.soic.indiana.edu:/u/$user/dv
scp distanceVector.tar.gz $user@lh008linux-01.soic.indiana.edu:/u/$user/dv2
scp distanceVector.tar.gz $user@lh008linux-01.soic.indiana.edu:/u/$user/dv3
