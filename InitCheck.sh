#!/bin/bash

enable -n local

for file in **/*.yaml;
do
   echo $file
   sed -n '/^.*\..*\..*\..*$/p' $file
done
