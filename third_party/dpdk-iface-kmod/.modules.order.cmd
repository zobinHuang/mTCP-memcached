cmd_/root/projects/memcached/third_party/dpdk-iface-kmod/modules.order := {   echo /root/projects/memcached/third_party/dpdk-iface-kmod/dpdk_iface.ko; :; } | awk '!x[$$0]++' - > /root/projects/memcached/third_party/dpdk-iface-kmod/modules.order
