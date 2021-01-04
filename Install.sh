##############################################################################################################
# This file will attempt to automatically get and install the needed components to run a BrainGenix instance #
##############################################################################################################


# First, Make Sure Python Is Installed #

sudo apt install python3 -y0


# Next, Check That Pip3 Is Installed #

sudo apt install python3-pip -y


# Install Python Packages #

pip3 install -r requirements.txt


# Install MySQL #

sudo apt install mysql-server -y


# Install Java For ZK #

sudo apt install default-jdk -y


# Install ZooKeeper #

sudo useradd zookeeper -m
sudo usermod --shell /bin/bash zookeeper
sudo usermod -p 123456 zookeeper ##<---- Change This Password! ##
sudo usermod -aG sudo zookeeper

sudo mkdir /zookeeper
sudo chown -R zookeeper:zookeeper /zookeeper

cd /opt

sudo wget https://mirrors.estointernet.in/apache/zookeeper/zookeeper-3.6.2/apache-zookeeper-3.6.2-bin.tar.gz
sudo tar -xvzf apache-zookeeper-3.6.2-bin.tar.gz
sudo mv apache-zookeeper-3.6.2-bin zookeeper

sudo chown -R zookeeper:zookeeper /opt/zookeeper


cat > /opt/zookeeper/conf/zoo.cfg <<EOF
tickTime=2500
dataDir=/zookeeper
clientPort=2181
maxClientCnxns=80
EOF


cd /opt/zookeeper
bin/zkServer.sh start
