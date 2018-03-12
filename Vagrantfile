$script = <<SCRIPT
echo "Installing required packages"
# Update ruby
sudo apt-add-repository ppa:brightbox/ruby-ng
sudo apt-get update
sudo apt-get install -y ruby2.4
# Installing other dependencies
sudo apt-get install -y git libtool autoconf automake pkg-config srecord valgrind
# Install nodejs
curl -sL https://deb.nodesource.com/setup_6.x | sudo -E bash -
sudo apt-get install -y nodejs
# Install pip
sudo apt-get install -y python-setuptools python-dev build-essential
sudo easy_install pip
sudo pip install gcovr
# Update gem
sudo gem install rubygems-update
sudo update_rubygems
# Avoiding using sudo to install gems
echo 'export GEM_HOME=~/.gems' >> ~/.bashrc
echo 'PATH=$PATH:~/.gems/bin' >> ~/.bashrc
gem install ceedling
SCRIPT

Vagrant.configure("2") do |config|
    config.vm.box = "ubuntu/xenial64"
    config.vm.provision "shell", inline: $script
end


