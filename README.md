ChibiOS-Drivers
===============

Collection of drivers for ChibiOS' HAL.

#### Using
* Clone this repo in the same directory as ChibiOS-RT. (not inside)
* Include the drivers.mk file into your project's Makefile ( include $(CHIBIOS)/../ChibiOS-Drivers/drivers.mk )
* Add the required files lists to the CSRC and CINC lists ( Usually $(STM32DRIVERSRC) and $(STM32DRIVERINC) )
* Include the drivers.h file in main.c, and anywhere else needed
* Add a driversInit() call just below halInit()
* You might have to tweak extra_registry.h depending on your hardware
  
Everything else works like using a standard driver.  
So far only drivers for STM32.

#### Drivers list
* Digital Analog Converter (dac), with optional DMA transfers
* Timer input capture (timcap), works on all channels simultaneously
* Independent watchdog timer (iwdg)
* Interrupt based UART driver (iuart), in case you run out of DMA channels  
