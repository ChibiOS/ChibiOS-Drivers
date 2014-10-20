
DRIVERPATH = $(CHIBIOS)/../ChibiOS-Drivers

DRIVERSRC = $(DRIVERPATH)/src/drivers.c
DRIVERSRC += $(DRIVERPATH)/src/timcap_driver.c
DRIVERSRC += $(DRIVERPATH)/src/dac_driver.c
DRIVERSRC += $(DRIVERPATH)/src/iwdg_driver.c
DRIVERSRC += $(DRIVERPATH)/src/iuart_driver.c

DRIVERINC += $(DRIVERPATH)/inc

STM32DRIVERSRC = $(DRIVERSRC)
STM32DRIVERSRC += $(DRIVERPATH)/stm32/iwdg_driver_lld.c
STM32DRIVERSRC += $(DRIVERPATH)/stm32/timcap_driver_lld.c
STM32DRIVERSRC += $(DRIVERPATH)/stm32/dac_driver_lld.c
STM32DRIVERSRC += $(DRIVERPATH)/stm32/iuart_driver_lld.c

STM32DRIVERINC = $(DRIVERINC)
STM32DRIVERINC += $(DRIVERPATH)/stm32
