#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xc4162456, "module_layout" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0xfd059f5d, "__register_chrdev" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0xd0da656b, "__stack_chk_fail" },
	{ 0xbcab6ee6, "sscanf" },
	{ 0xb9eb9fd8, "register_netdev" },
	{ 0xee002b19, "alloc_etherdev_mqs" },
	{ 0xe2d5255a, "strcmp" },
	{ 0xfe8c61f0, "_raw_read_lock" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0x5213d936, "free_netdev" },
	{ 0xe1052f9e, "unregister_netdev" },
	{ 0x221cc46f, "init_net" },
	{ 0xe68efe41, "_raw_write_lock" },
	{ 0xa5976e4f, "dev_base_lock" },
	{ 0x92997ed8, "_printk" },
	{ 0x87a21cb3, "__ubsan_handle_out_of_bounds" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "B4A93E45426155E569C4930");
