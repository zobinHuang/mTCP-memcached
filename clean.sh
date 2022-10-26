export mmc_root=$(pwd)

cd $mmc_root/third_party

# clean mtcp
cd $mmc_root/third_party/mtcp
make distclean

# clean libevent
cd $mmc_root/third_party/libevent
cd build && make clean
cd .. && rm -rf build

# clean dpdk-iface-kmod
cd $mmc_root/third_party/dpdk-iface-kmod
export RTE_SDK=/usr/src/dpdk-21.11
make clean

# clean memcached
cd $mmc_root
make clean