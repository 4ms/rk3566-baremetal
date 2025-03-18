## Commands:

Calculate one second worth of audio: (on MP1, this takes 60-70ms)
GPIO0 C5 will go high at the start, and low when it's done.
The argument is ignored.
```
play 0
```

Toggle the GPIO as fast as possible (argument sets number of pulses)
```
pin 20
```


To use this project, try various clock settings and run play or pin to see
how they are effected.


## Startup values:
```
rr 0xFDD20000
0x00002044

APLL_CON0   0x2044   bits 13, 6, 2
                     bypass[15] = no
                     postdiv1[14:12] = 2        First post divide value (1-7)
                     fbdiv[11:0] = 0x44 = 68

CRU_MODE_CON00: 0x0055  bits 1, 3, 5, 7
rr 0xFDD200C0           apll_mux = clk_apll
                        dpll_mux = clk_dpll
                        cpll_mux = clk_cpll
                        gpll_mux = clk_gpll


> rr 0xFDD200C4
0x00000000
> rr 0xFDD200C8
0x00000000
> rr 0xFDD200CC
0x00000000
> rr 0xFDD200D0
0x00640064

```

Run djembe, it takes ~17ms = 1.7% (??!!)

```
Unlock PLLA: Set bit 10 to 0 in CON1
wr 0xFDD20004 0x04000000
0x00001441

Bypass PLLA: set bit 15 to 1 in CON0
wr 0xFDD20000 0x80008000
0x0000a044

Lock PLLA: Set bit 10 to 1 in CON1
wr 0xFDD20004 0x04000400
0x00001441
``` 

Bypassed, djembe takes 576ms to generate 1s of audio = 57.6%


```
Unbypass PLLA:
wr 0xFDD20000 0x80000000
```

takes 17ms again.


```
Set Post divide value to 3 (bits 14:12 = 011)
wr 0xFDD20000 0x70003000
```

Djembe takes 25ms now

```
Set Post divide back to 2 (bits 14:12 = 010)
wr 0xFDD20000 0x70002000
```

Set fastest clock (I think?)

```
Set Post divide value to 1 (bits 14:12 = 001)
wr 0xFDD20000 0x70001000
```



## Test out clock

```
Read Enable clock reg?? Seems not necessary?
rr 0xFDD20388  
wr 0xFDD20388 0x80008000    Set bit 15 = disable testout
wr 0xFDD20388 0x80000000    Clr bit 15 = enable testout

Select clock source

rr 0xFDD20228
0x0000001f      xin_osc0 / 32. 24MHZ / 32 = 750kHz

                  # 0x00001Fdd
                  # dd: Bits 7:0 are divider-1
				  # 00-1F: Bits 12:8 select clock
				  00: xin_osc0_func_mux
				  02: clk_core_ndft
				  03: sclk_core_pre_ndft
				  06: ddrphy1x
				  08: clk_isp_core_ndft
				  09: aclk_vop_pre_ndft
				  0A: dclk0:vop_ndft
				  10: clk_core_pvtpll_out
				  13: aclk_bus_ndft
				  18: mclk_i2s0_8ch_rx_ndft
				  1E: clk_usbphy0_ref
				  1F: usbphy480m_i

clk_core out, divided by 256 = should see ~6.25MHz (core is 1.6GHz)
wr 0xFDD20228 0x1FFF02FF   == 3.188MHz (*256 = 816MHz)


PinMux GRF_GPIO2A_IOMUX_L
   Bits 10:8 = 0b010 sets GPIO2_A2 to TEST_CLKOUT
   Note: this disables SDMMC0_CLK in case that's in use


Default pinmux value:
rr 0xFDC600020
0x00001111

PinMux GPIO2A2 as TEST_CLK_OUT:
wr 0xFDC60020 0x03000200 

```

Check "core" clock
```
Set Post divide value to 1 (bits 14:12 = 001)
wr 0xFDD20000 0x70001000

Route clk_core to TEST OUT, with /256
wr 0xFDD20228 0x1FFF02FF
```

Clock out is now 6.38MHz. *256 = 1.633GHz


Read other clocks

```
USB Phy 480M / 256   reads 1.876MHz (*256 = 480MHz) 
wr 0xFDD20228 0x1FFF1FFF

USB Ref: 24MHz
wr 0xFDD20228 0x1FFF1E00

MCLK I2S0: 50MHz
wr 0xFDD20228 0x1FFF1800

XIN OSC0 (xtal?): 24MHz
wr 0xFDD20228 0x1FFF0000

ACLK: 150MHz (50MHz * 3)
wr 0xFDD20228 0x1FFF1302

SCLK: /256 = 3.185MHz, so actual = 815MHz
wr 0xFDD20228 0x1FFF03FF

```


