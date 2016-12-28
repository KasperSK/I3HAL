#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xaa8a822b, "module_layout" },
	{ 0xdd9fa878, "cdev_del" },
	{ 0xa9da9c49, "class_destroy" },
	{ 0xdee5b14, "device_destroy" },
	{ 0x38bb6baa, "del_timer" },
	{ 0x9dfdf722, "gpio_free_array" },
	{ 0x6a44bb49, "device_create" },
	{ 0x8c19e5ba, "__class_create" },
	{ 0xbf258353, "init_timer_key" },
	{ 0x7485e15e, "unregister_chrdev_region" },
	{ 0x20495cbb, "cdev_add" },
	{ 0x9330885b, "cdev_init" },
	{ 0x29537c9e, "alloc_chrdev_region" },
	{ 0x8574ca6c, "gpio_request_array" },
	{ 0x67c2fa54, "__copy_to_user" },
	{ 0x28118cb6, "__get_user_1" },
	{ 0x432fd7f6, "__gpio_set_value" },
	{ 0x6c8d5ae8, "__gpio_get_value" },
	{ 0x91715312, "sprintf" },
	{ 0x193ef101, "add_timer" },
	{ 0x7d11c268, "jiffies" },
	{ 0x996bdb64, "_kstrtoul" },
	{ 0xc00f6a9a, "dev_get_drvdata" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
	{ 0x27e1a049, "printk" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "61F32E187D9E9D2CF057DDA");
