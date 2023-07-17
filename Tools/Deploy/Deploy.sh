#!/bin/bash


# Build Program, Build Package
echo "-- Building Project/Package"
cd Tools
bash Build.sh
bash Package.sh
cd ..

# Find Package, Deploy
echo "-- Setting Noninteractive Mode For APT"
DEBIAN_FRONTEND=noninteractive

echo "-- Finding Current Package Version"
cd $Artifacts && PackageVar=$(find -type f -name 'BrainGenix-NES*.*.deb');
echo "Found API Package At $PackageVar"

echo "-- Installing New Package"
sudo --preserve-env apt install $PackageVar -y --reinstall
