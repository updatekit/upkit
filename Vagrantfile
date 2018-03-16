$script = <<SCRIPT
echo "Installing required packages"
sudo apt-get update
# Installing build toolchain
sudo apt-get install -y g++ git libtool autoconf automake pkg-config srecord valgrind
SCRIPT

Vagrant.configure("2") do |config|
    config.vm.box = "ubuntu/trusty64"
    config.vm.provision "shell", inline: $script
end


