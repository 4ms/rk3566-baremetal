# I2S example

Status: Runs in stereo TX only mode. MCLK is generated at 256 * LRCLK, and SCLK is 64 * LRCLK.
LRCLK is about 48kHz, though not exact. 
Frames are 32-bits. 

Tried TDM mode, and it seemed to be working but I didn't try it long enough to get the clock ratios correct.

Have not tried RX yet.

Have not tried DMA yet.


One weirdness is the I2S ISR must be enabled after the register_and_start_isr() call or else it never fires.
