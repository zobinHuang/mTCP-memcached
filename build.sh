export mmc_root=$(pwd)

cd $mmc_root/third_party

# build mtcp
echo "Build MTCP..."
cd $mmc_root/third_party/mtcp
./configure --with-dpdk-lib=$RTE_SDK/$RTE_TARGET
make

# install mtcp
cp $mmc_root/third_party/mtcp/mtcp/include/mtcp*.h /usr/local/include/
cp $mmc_root/third_party/mtcp/mtcp/lib/libmtcp.a /usr/local/lib/libmtcp.a

# build libevent
echo "Build libevent..."
cd $mmc_root/third_party/libevent
if [ -d "$mmc_root/third_party/libevent/build" ]
then
    rm -rf $mmc_root/third_party/libevent/build
fi
mkdir build && cd build
cmake ..
make
make install

# build dpdk-iface
echo "Register network interface dpdk0"
cd $mmc_root/third_party/dpdk-iface-kmod
export RTE_SDK=/usr/src/dpdk-21.11
make
insmod dpdk_iface.ko
./dpdk_iface_main
ifconfig dpdk0 10.0.109.2 netmask 255.255.255.0 up

# build memcached
echo "Build Memcached..."
cd $mmc_root
./autogen.sh
./configure
make