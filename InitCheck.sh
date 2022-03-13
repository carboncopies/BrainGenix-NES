#!/bin/bash

for file in **/*.yaml
do
   echo $file
   totalips=$(grep -o '[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}' $file)
   echo "Found IPs:" $totalips
   size1=${#totalips}
   validips=$(grep -o -E '(([0-9]|[0-9]{2}|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[0-9]{2}|1[0-9]{2}|2[0-4][0-9]|25[0-5])' $file)
   echo "Valid IPs:" $validips
   size2=${#validips}

   if [ $size1 -ne $size2 ]
   then echo "One/More invalid IPs found in Config File"
   else
   echo "Config file is valid"
   fi
done
