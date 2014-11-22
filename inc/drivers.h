#ifndef DRIVERS_H
#define DRIVERS_H

#include "hal.h"
#include "drivers_conf.h"
#include "dac_driver.h"
#include "iwdg_driver.h"
#include "timcap_driver.h"
#include "iuart_driver.h"
#include "eicu_driver.h"

#ifdef __cplusplus
extern "C" {
#endif
  void driversInit(void);
#ifdef __cplusplus
}
#endif

#endif
