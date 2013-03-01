project0
========

My simple code for a launchpad VFD clock. Read the code for details, but the basic idea is:

On first power-up, the time is read via i2c from a DS1307 RTC chip.
One input is connected to the square wave 1Hz output of the DS1307, and increments the second counter on an interrupt.
One output is PWM'ed to drive a switch mode power supply that boosts 5v-50V for the tubes.
Three outputs send data to the MAX6921 driver chip constantly to keep the persistence of vision effect working.
