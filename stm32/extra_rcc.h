#ifndef _EXTRA_RCC_
#define _EXTRA_RCC_

#include "stm32_rcc.h"

#ifndef rccEnableDAC

#define rccEnableDAC(lp) rccEnableAPB1(RCC_APB1ENR_DACEN, lp)
#define rccDisableDAC(lp) rccDisableAPB1(RCC_APB1ENR_DACEN, lp)
#define rccResetDAC() rccResetAPB1(RCC_APB1RSTR_DACRST)

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

#ifndef rccEnableTIM18

#define rccEnableTIM18(lp) rccEnableAPB1(RCC_APB1ENR_TIM18EN, lp)
#define rccDisableTIM18(lp) rccDisableAPB1(RCC_APB1ENR_TIM18EN, lp)
#define rccResetTIM18() rccResetAPB1(RCC_APB1RSTR_TIM18RST)

#endif

#endif /* _EXTRA_RCC_ */