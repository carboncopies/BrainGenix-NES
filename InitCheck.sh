#!/bin/bash

sudo apt-get install yamllint
pip install shyaml

files= find . -type f -name '*.yaml'

for file in files
do
   echo file
   source file
   
   # Check validity of config file
   yamllint file
   
   # Return data values and their datatypes after parsing config file
   cat test.yaml | shyaml values-0 | \
   while read -r -d $'\0' value; do
       echo "RECEIVED: '$value'"
       echo "DATATYPE: '$type(value)'"
   done
   
done
