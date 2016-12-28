insmod hotplug_psoc4_spi_device.ko
insmod psoc4mod.ko

echo 0x1 > /sys/class/cplddrv/cpld/ext_serial_if_route_reg
echo 0x1 > /sys/class/cplddrv/cpld/spi_route_reg

rm -f /dev/psoc4CapSense /dev/psoc4Temp /dev/psoc4dummyRead /dev/psoc4write

major=$(awk '$2=="psoc4" {print $1}' /proc/devices)

mknod /dev/psoc4CapSense c $major 0
mknod /dev/psoc4Temp c $major 1
mknod /dev/psoc4dummyRead c $major 2
mknod /dev/psoc4write c $major 3



