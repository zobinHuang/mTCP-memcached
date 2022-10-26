# MTCP-accelerated Memcached

## Build

Firtsly we need to compile mTCP

```bash
cd memcached/third_party/mtcp
./configure --with-dpdk-lib=$RTE_SDK/$RTE_TARGET
make
```

Then we need to build and install mTCP-accelerated libevent

```bash
cd ./third_party/libevent
mkdir build && cd build
cmake ..
make
make install
```

Then we need to build and run `dpdk-iface` which mTCP provides to manage DPDK-controlled NIC

```bash
cd ./third_party/dpdk-iface-kmod
export RTE_SDK=/usr/src/dpdk-21.11
make
insmod dpdk_iface.ko
./dpdk_iface_main
```

To build memcached in your machine from local repo you will have to install
autotools, automake and libevent. In a debian based system that will look
like this

```bash
sudo apt-get install autotools-dev
sudo apt-get install automake
sudo apt-get install libevent-dev
```

After that you can build memcached binary using automake

```bash
cd [Root of Memcached]
./autogen.sh
./configure
make
make test
```

It should create the binary in the same folder, which you can run

```bash
./memcached
```

You can telnet into that memcached to ensure it is up and running

```bash
telnet 127.0.0.1 11211
stats
```

IF BUILDING PROXY, AN EXTRA STEP IS NECESSARY:

```bash
cd memcached
cd vendor
./fetch.sh
cd ..
./autogen.sh
./configure --enable-proxy
make
make test
```