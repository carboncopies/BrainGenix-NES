
# Set Variables #
Zookeeper_Password='123456'


# Init #
sudo apt update


# Install PreReqs For Zookeeper #
sudo apt install default-jre -y

# Install Zookeeper #
sudo useradd zookeeper -m
sudo usermod --shell /bin/bash zookeeper
sudo usermod -p $Zookeeper_Password zookeeper
sudo usermod -aG sudo zookeeper

sudo mkdir -p /data/zookeeper
chown -R zookeeper:zookeeper /data/zookeeper
cd /opt

sudo wget https://apache.claz.org/zookeeper/zookeeper-3.6.3/apache-zookeeper-3.6.3-bin.tar.gz
sudo tar -xvf apache-zookeeper-3.6.3-bin.tar.gz

mv apache-zookeeper-3.6.3-bin zookeeper
chown -R zookeeper:zookeeper /opt/zookeeper


# Configure Zookeeper #
sudo cat > /opt/zookeeper/conf/zoo.cfg <<_EOF_
tickTime = 2000
dataDir = /data/zookeeper
clientPort = 2181
initLimit = 5
syncLimit = 2
_EOF_


# Create Zookeeper Service #
sudo cat > /etc/systemd/system/zookeeper.service <<_EOF_
[Unit]
Description=Zookeeper Daemon
Documentation=http://zookeeper.apache.org
Requires=network.target
After=network.target

[Service]
Type=forking
WorkingDirectory=/opt/zookeeper
User=zookeeper
Group=zookeeper
ExecStart=/opt/zookeeper/bin/zkServer.sh start /opt/zookeeper/conf/zoo.cfg
ExecStop=/opt/zookeeper/bin/zkServer.sh stop /opt/zookeeper/conf/zoo.cfg
ExecReload=/opt/zookeeper/bin/zkServer.sh restart /opt/zookeeper/conf/zoo.cfg
TimeoutSec=30
Restart=on-failure

[Install]
WantedBy=default.target
_EOF_


# Install System Service File #
sudo systemctl daemon-reload

systemctl start zookeeper
systemctl enable zookeeper


# Install Python3 PIP #
sudo apt install python3-pip -y

# Install Python3 Packages #
pip3 install -r requirements.txt


# Add Reminder To Configure Replicated Mode #
echo "Remember to configure /opt/zookeeper/conf/zoo.cfg in replicated mode!"
