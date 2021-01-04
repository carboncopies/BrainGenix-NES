##############################################################################################################
# This file will attempt to automatically get and install the needed components to run a BrainGenix instance #
##############################################################################################################


# First, Make Sure Python Is Installed #

sudo apt install Python3 -y0


# Next, Check That Pip3 Is Installed #

sudo apt install Python3-pip -y


# Install Python Packages #

pip3 install -r requirements.txt


# Install MySQL #

sudo apt install MySQL -y


# Install Java For ZK #

sudo apt install default-jdk -y


# Install ZooKeeper #

sudo apt install zookeeper -y




# Create Users For ZK #

#sudo useradd zookeeper -m
#sudo usermod --shell /bin/bash zookeeper
#sudo passwd zookeeper
#sudo usermod -aG sudo zookeeper

#mkdir /zookeeper
#chown -R zookeeper:zookeeper /zookeeper

