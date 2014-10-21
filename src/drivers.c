#include "drivers.h"

void driversInit(void)
{
#if defined(DRIVER_USE_DAC) && DRIVER_USE_DAC
  dacInit();
#endif
#if defined(DRIVER_USE_IWDG) && DRIVER_USE_IWDG
  iwdgInit();
#endif
#if defined(DRIVER_USE_TIMCAP) && DRIVER_USE_TIMCAP
  timcapInit();
#endif
#if defined(DRIVER_USE_IUART) && DRIVER_USE_IUART
  iuartInit();
#endif
#if defined(DRIVER_USE_EICU) && DRIVER_USE_EICU
  eicuInit();
#endif
}
