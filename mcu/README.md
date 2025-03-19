# MCU 

This example was derived from [liamhays/rk3566-mcu](https://github.com/liamhays/rk3566-mcu) repo.
There is a lot of very useful information and examples there.

## How it works
- The mcu binary is linked to run at RK3566 SYSRAM (0xfdcc0000 - 0xfdccffff)
- The mcu binary is built into the main program
- The main program resets the mcu and copies the mcu binary to SYSRAM
- The main program sets the entry point for the MCU
- The main program takes the mcu out of reset
- The mcu runs a program toggling GPIO0-C5 (Pin 31 on the Radxa CM3 IO)

## Note about U-Boot and BL31
U-Boot must be built with Rockchip's [BL31 binary](https://github.com/rockchip-linux/rkbin/blob/master/bin/rk35/rk3568_bl31_v1.44.elf)
for this example to run. 
When using ATF's BL31 the RK3566 SYSRAM throws an exception when read or written.
