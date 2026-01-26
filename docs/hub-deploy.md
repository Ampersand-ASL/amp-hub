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
* Debian 13, arm-64 using t4g-micro instance type.
* If you don't already have a keypair, create one called "ampersand-1"
* If necessary, download the private half of the keypair to ~/.ssh/parrot-1.pem so that you can log in using SSH.
* Accept the default EBS size of 8G.
* Associate the two network interfaces setup above.
* Associate the IAM role that grants administrator access.
* Wait for the instance to come up.

If not created previously, get the public IPv4 address from the EC2 console. Use SSH to log into the new instance as admin:

    ssh -i ~/.ssh/ampersand-1.pem admin@xxx.xxx.xxx.xxx

(The rest of the steps are executed on the new EC2 instance,
all from the admin home directory.)

# Steps To Install

Add the required Linux packages:

    sudo apt update
    sudo apt -y upgrade
    sudo apt -y install net-tools wget 

Install the binary package:

    wget https://mackinnon.info/ampersand/releases/amp-hub_1.0-1_arm64.tar.gz
    gunzip amp-hub_1.0-1_arm64.tar.gz
    mkdir -p /usr/bin
    mkdir -p /usr/lib
    mkdir -p /usr/etc
    cd amp-hub-1.0-1_arm64
    cp -r bin /usr
    cp -r lib /usr
    cp -r etc /usr

Create the Linux service:
    
    # The service runs as amp-hub
    useradd -r -s /sbin/nologin amp-hub
    # systemd setup
    sudo cp /usr/etc/amp-hub.service /lib/systemd/system
    sudo systemctl enable amp-hub

Start the Linux service:

    sudo systemctl start amp-hub
    # Check the log file
    journalctl -u amp-hub











    sudo apt install ./asl-parrot_1.4-1_arm64.deb

NOTE: There may be a notice displayed that contains "permission denied." If this is 
just a notice it can be ignored.

**Before starting the service** make adjustments /usr/etc/asl-parrot.env file.

Add the secrets here:

    AMP_NODE0_NUMBER=nnnnn
    AMP_NODE0_PASSWORD=xxxx

Change the bind address used for network diagnostic testing to the INTERNAL
IP address associated with the network interface that was setup for network
diagnostics.

    # Network interface (IPv4) that is used to initiate network tests.
    # Needs to be different from the interface accepting IAX connections
    # for the test to be fully effective
    AMP_NET_TEST_BIND_ADDR4=172.31.23.91

Adjust the HTTP listen port (internal VPC only) as needed:

    AMP_HTTP_PORT=8080

And then enable and start the service:

    sudo systemctl enable asl-parrot
    sudo systemctl start asl-parrot

Check the log:

    journalctl -u asl-parrot.service -f

Check the network tests API using curl:

    curl http://localhost:8080/network-test?node=2002

# Network Configuration

![Security Group](sg1.jpg)

# Network Test API 

Example request:
    
    curl http://asl-parrot:8080/network-test?node=2002

Example response (good case):

    {"ipv4":{"addr":"18.226.187.225","pingms":49,"port":4569,"rc":0,"status":"ok"}}

Example response (bad case):

    {"ipv4":{"rc":-9,"status":"unreachable"}}
