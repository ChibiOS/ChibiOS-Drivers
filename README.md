ChibiOS-Drivers
===============

Collection of drivers for ChibiOS' HAL.

#### Using
* Include the drivers.mk file into your project's Makefile ( include ChibiOS-Drivers/drivers.mk )
* Add the required lists to the CSRC and CINC lists ( Usually $(STM32DRIVERSRC) and $(STM32DRIVERINC) )
* Include the drivers.h file in main.c, and anywhere else needed
* Add a driversInit() call just below halInit()
  
Everything else works like using a standard driver.  
So far only drivers for STM32.

#### Drivers list
* Digital Analog Converter (dac), with optional DMA transfers
* Timer input capture (timcap), works on all channels simultaneously
* Independent watchdog timer (iwdg)
* Interrupt based UART driver (iuart), in case you run out of DMA channels  
