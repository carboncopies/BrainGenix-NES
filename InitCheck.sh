#!/bin/bash

files= find . -type f -name '*.yaml'

for file in files
do
   echo file
   source file
done
