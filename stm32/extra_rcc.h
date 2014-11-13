#ifndef _EXTRA_RCC_
#define _EXTRA_RCC_

#include "stm32_rcc.h"

#ifndef rccEnableDAC
#define rccEnableDAC(lp) rccEnableAPB1(RCC_APB1ENR_DACEN, lp)
#define rccDisableDAC(lp) rccDisableAPB1(RCC_APB1ENR_DACEN, lp)
#define rccResetDAC() rccResetAPB1(RCC_APB1RSTR_DACRST)
#endif

#ifndef rccEnableDAC1
#define rccEnableDAC1(lp) rccEnableAPB1(RCC_APB1ENR_DAC1EN, lp)
#define rccDisableDAC1(lp) rccDisableAPB1(RCC_APB1ENR_DAC1EN, lp)
#define rccResetDAC1() rccResetAPB1(RCC_APB1RSTR_DAC1RST)
#endif

#ifndef rccEnableDAC2
#define rccEnableDAC2(lp) rccEnableAPB1(RCC_APB1ENR_DAC2EN, lp)
#define rccDisableDAC2(lp) rccDisableAPB1(RCC_APB1ENR_DAC2EN, lp)
#define rccResetDAC2() rccResetAPB1(RCC_APB1RSTR_DAC2RST)
#endif

#ifndef rccEnableTIM6
#define rccEnableTIM6(lp) rccEnableAPB1(RCC_APB1ENR_TIM6EN, lp)
#define rccDisableTIM6(lp) rccDisableAPB1(RCC_APB1ENR_TIM6EN, lp)
#define rccResetTIM6() rccResetAPB1(RCC_APB1RSTR_TIM6RST)
#endif

#ifndef rccEnableTIM7
#define rccEnableTIM7(lp) rccEnableAPB1(RCC_APB1ENR_TIM7EN, lp)
#define rccDisableTIM7(lp) rccDisableAPB1(RCC_APB1ENR_TIM7EN, lp)
#define rccResetTIM7() rccResetAPB1(RCC_APB1RSTR_TIM7RST)
#endif

#ifndef rccEnableTIM15
#define rccEnableTIM15(lp) rccEnableAPB2(RCC_APB2ENR_TIM15EN, lp)
#define rccDisableTIM15(lp) rccDisableAPB2(RCC_APB2ENR_TIM15EN, lp)
#define rccResetTIM15() rccResetAPB2(RCC_APB2RSTR_TIM15RST)
#endif

#ifndef rccEnableTIM16
#define rccEnableTIM16(lp) rccEnableAPB2(RCC_APB2ENR_TIM16EN, lp)
#define rccDisableTIM16(lp) rccDisableAPB2(RCC_APB2ENR_TIM16EN, lp)
#define rccResetTIM16() rccResetAPB2(RCC_APB2RSTR_TIM16RST)
#endif

#ifndef rccEnableTIM17
#define rccEnableTIM17(lp) rccEnableAPB2(RCC_APB2ENR_TIM17EN, lp)
#define rccDisableTIM17(lp) rccDisableAPB2(RCC_APB2ENR_TIM17EN, lp)
#define rccResetTIM17() rccResetAPB2(RCC_APB2RSTR_TIM17RST)
#endif

#ifndef rccEnableTIM18
#define rccEnableTIM18(lp) rccEnableAPB1(RCC_APB1ENR_TIM18EN, lp)
#define rccDisableTIM18(lp) rccDisableAPB1(RCC_APB1ENR_TIM18EN, lp)
#define rccResetTIM18() rccResetAPB1(RCC_APB1RSTR_TIM18RST)
#endif

#endif /* _EXTRA_RCC_ */