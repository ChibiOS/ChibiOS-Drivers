ChibiOS-Drivers
===============

Collection of drivers for ChibiOS' HAL.

#### Using
* Clone this repo in the same directory as ChibiOS-RT. (not inside)
* Include the drivers.mk file into your project's Makefile ( include $(CHIBIOS)/../ChibiOS-Drivers/drivers.mk )
* Add the required files lists to the CSRC and CINC lists ( Usually $(STM32DRIVERSRC) and $(STM32DRIVERINC) )
* Include the drivers.h file in main.c, and anywhere else needed
* Add a driversInit() call just below halInit()
* include drivers_conf.h at the top of mcuconf.h
* You might have to tweak extra_registry.h and drivers_conf.h depending on your hardware
  
Everything else works like using a standard driver.  
So far only drivers for STM32.

#### Drivers list
* Digital Analog Converter (dac), with optional DMA transfers
* Timer input capture (timcap), works on all channels simultaneously
* Independent watchdog timer (iwdg)
* Interrupt based UART (iuart), in case you run out of DMA channels  
* Enhanced input capture unit (eicu).
