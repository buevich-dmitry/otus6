To download and install the latest version, run:
```
wget -O otus6-Linux.deb https://github.com/buevich-dmitry/otus6/releases/latest/download/otus6-Linux.deb
sudo dpkg -i otus6-Linux.deb
```

Also you can build the project locally:
```
./build.sh
sudo dpkg -i bin/otus6-*-Linux.deb
```

Then run otus6 app:
```
otus6 <block_size>
```
