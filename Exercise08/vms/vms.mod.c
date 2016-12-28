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
	{ 0x42b8bcce, "platform_device_unregister" },
	{ 0x3adc990, "sysfs_remove_group" },
	{ 0x88e0a20c, "input_unregister_device" },
	{ 0xc92b6fa5, "input_register_device" },
	{ 0x72320cc, "input_set_capability" },
	{ 0x1473f39e, "input_allocate_device" },
	{ 0xd6a69c01, "sysfs_create_group" },
	{ 0x8f5a8873, "platform_device_register_full" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
	{ 0x27e1a049, "printk" },
	{ 0x22031488, "input_event" },
	{ 0x42224298, "sscanf" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "645EE8327D97C9A8BDDED44");
