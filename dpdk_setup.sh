export dpdk_root=/usr/src/dpdk-21.11/
export interface_name=ens192

# Setup non-iommu mode
echo 1 > /sys/module/vfio/parameters/enable_unsafe_noiommu_mode

# Setup hugepage
echo 1024 > /sys/devices/system/node/node0/hugepages/hugepages-2048kB/nr_hugepages
echo 1024 > /sys/devices/system/node/node1/hugepages/hugepages-2048kB/nr_hugepages
echo 1024 > /sys/devices/system/node/node0/hugepages/hugepages-1048576kB/nr_hugepages
echo 1024 > /sys/devices/system/node/node1/hugepages/hugepages-1048576kB/nr_hugepages
mkdir /mnt/huge
mount -t hugetlbfs pagesize=2MB /mnt/huge
mount -t hugetlbfs pagesize=1GB /mnt/huge

# Bind interface to vfio driver
cd $dpdk_root/usertools
./dpdk-devbind.py -b vfio-pci $interface_name --force