Instructions for deploying the Ampersand Hub on AWS. These instructions
are based on a manual deployment. 

# Things You Need To Deploy

These instructions assume you are starting from nothing except:
* The URL of the binary packages (get that from Bruce)
* The ASL node number and password for the hub node 

# Steps To Install (AWS Pre-Install)

Network setup:
* Create a Security Groups that allows IAX/SSH inbound 

IAM Setup:
* Create an IAM role for EC2 that grants `AdministratorAccess`. This may be useful
later when you want to run AWS CLI commands.

Create an EC2 instance:
* Debian 13, x86-64 using t3.micro instance type.
* If you don't already have a keypair, create one called "amp-hub-key"
* If necessary, download the private half of the keypair to ~/.ssh/amp-hub-key.pem so that you can log in using SSH.
* Accept the default EBS size of 8G.
* Associate the IAM role that grants administrator access.
* Wait for the instance to come up.

If not created previously, get the public IPv4 address from the EC2 console. Use SSH to log into the new instance as admin:

    ssh -i ~/.ssh/amp-hub-key.pem admin@xxx.xxx.xxx.xxx

Or update your ~/.ssh/config file to register the new server and credentials:

    Host amp-hub
        Hostname amp-hub
        User admin
        IdentityFile C:\\Users\\bruce\\.ssh\\amp-hub-key.pem
        IdentitiesOnly yes

(The rest of the steps are executed on the new EC2 instance,
all from the admin home directory.)

# Steps To Install

Add the required Linux packages:

    sudo apt update
    sudo apt -y upgrade
    sudo apt -y install net-tools wget 

Install the binary package:

    export ARCH=x86_64
    wget https://mackinnon.info/ampersand/releases/amp-hub_1.0-1_${ARCH}.tar.gz
    gunzip amp-hub_1.0-1_${ARCH}.tar.gz
    mkdir -p /usr/bin
    mkdir -p /usr/lib
    mkdir -p /usr/etc
    cd amp-hub-1.0-1_${ARCH}
    cp -r bin /usr
    cp -r lib /usr
    cp -r etc /usr

**Before starting the service** make a few adjustments /usr/etc/amp-hub.env file. These
lines will probably need to change:

    # Add the secrets here:
    AMP_NODE0_NUMBER=nnnnn
    AMP_NODE0_PASSWORD=xxxx

    # Make sure the IAX (UDP) port is correct:
    AMP_IAX_PORT=4569

Create the Linux service:
    
    # The service runs as amp-hub
    sudo useradd -r -s /sbin/nologin amp-hub
    # systemd setup
    sudo cp /usr/etc/amp-hub.service /lib/systemd/system
    sudo systemctl enable amp-hub

Start the Linux service:

    sudo systemctl start amp-hub
    # Check the log file
    journalctl -u amp-hub -f

# Network Configuration

![Security Group](sg1.jpg)

