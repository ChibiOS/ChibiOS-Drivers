/*
    ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/
/*
   Concepts and parts of this file have been contributed by Fabio Utzig and
   Xo Wang.
*/
/*
   Rewritten by Emil Fresk (1/5 - 2014) for extended input capture
   functionallity. And fix for spourious callbacks in the interrupt handler.
*/



/* *
 *
 * Hardware Abstraction Layer for Extended Input Capture Unit
 *
 * */

#include "ch.h"
#include "hal.h"
#include "eicu_driver.h" /* Should be in hal.h but is not a part of ChibiOS */


#if DRIVER_USE_EICU || defined(__DOXYGEN__)


/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/
/**
 * @brief   EICUD1 driver identifier.
 * @note    The driver EICUD1 allocates the complex timer TIM1 when enabled.
 */
#if STM32_EICU_USE_TIM1 && !defined(__DOXYGEN__)
EICUDriver EICUD1;
#endif

/**
 * @brief   EICUD2 driver identifier.
 * @note    The driver EICUD2 allocates the timer TIM2 when enabled.
 */
#if STM32_EICU_USE_TIM2 && !defined(__DOXYGEN__)
EICUDriver EICUD2;
#endif

/**
 * @brief   EICUD3 driver identifier.
 * @note    The driver EICUD3 allocates the timer TIM3 when enabled.
 */
#if STM32_EICU_USE_TIM3 && !defined(__DOXYGEN__)
EICUDriver EICUD3;
#endif

/**
 * @brief   EICUD4 driver identifier.
 * @note    The driver EICUD4 allocates the timer TIM4 when enabled.
 */
#if STM32_EICU_USE_TIM4 && !defined(__DOXYGEN__)
EICUDriver EICUD4;
#endif

/**
 * @brief   EICUD5 driver identifier.
 * @note    The driver EICUD5 allocates the timer TIM5 when enabled.
 */
#if STM32_EICU_USE_TIM5 && !defined(__DOXYGEN__)
EICUDriver EICUD5;
#endif

/**
 * @brief   EICUD8 driver identifier.
 * @note    The driver EICUD8 allocates the timer TIM8 when enabled.
 */
#if STM32_EICU_USE_TIM8 && !defined(__DOXYGEN__)
EICUDriver EICUD8;
#endif

/**
 * @brief   EICUD9 driver identifier.
 * @note    The driver EICUD9 allocates the timer TIM9 when enabled.
 */
#if STM32_EICU_USE_TIM9 && !defined(__DOXYGEN__)
EICUDriver EICUD9;
#endif

/**
 * @brief   EICUD12 driver identifier.
 * @note    The driver EICUD12 allocates the timer TIM12 when enabled.
 */
#if STM32_EICU_USE_TIM12 && !defined(__DOXYGEN__)
EICUDriver EICUD12;
#endif

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/
/**
 * @brief   Shared IRQ handler.
 *
 * @param[in] eicup     Pointer to the @p EICUDriver object
 */
static void eicu_lld_serve_interrupt(EICUDriver *eicup)
{
  uint16_t sr;
  sr = eicup->tim->SR;

  /* Pick out the interrupts we are interested in by using
     the interrupt enable bits as mask */
  sr &= (eicup->tim->DIER & STM32_TIM_DIER_IRQ_MASK);

  /* Clear interrupts */
  eicup->tim->SR = ~sr;

  if (eicup->config->input_type == EICU_INPUT_PWM) {
    if (eicup->config->iccfgp[0] != NULL) {
      if ((sr & STM32_TIM_SR_CC1IF) != 0)
        _eicu_isr_invoke_pwm_period_cb(eicup, EICU_CHANNEL_1);
      if ((sr & STM32_TIM_SR_CC2IF) != 0)
        _eicu_isr_invoke_pwm_width_cb(eicup, EICU_CHANNEL_1);
    } else {
      if ((sr & STM32_TIM_SR_CC1IF) != 0)
        _eicu_isr_invoke_pwm_width_cb(eicup, EICU_CHANNEL_2);
      if ((sr & STM32_TIM_SR_CC2IF) != 0)
        _eicu_isr_invoke_pwm_period_cb(eicup, EICU_CHANNEL_2);
    }
  } else if (eicup->config->input_type == EICU_INPUT_PULSE) {
    if ((sr & STM32_TIM_SR_CC1IF) != 0)
      _eicu_isr_invoke_pulse_width_cb(eicup, EICU_CHANNEL_1);
    if ((sr & STM32_TIM_SR_CC2IF) != 0)
      _eicu_isr_invoke_pulse_width_cb(eicup, EICU_CHANNEL_2);
    if ((sr & STM32_TIM_SR_CC3IF) != 0)
      _eicu_isr_invoke_pulse_width_cb(eicup, EICU_CHANNEL_3);
    if ((sr & STM32_TIM_SR_CC4IF) != 0)
      _eicu_isr_invoke_pulse_width_cb(eicup, EICU_CHANNEL_4);
  } else {  /* EICU_INPUT_EDGE */
    if ((sr & STM32_TIM_SR_CC1IF) != 0)
      _eicu_isr_invoke_edge_detect_cb(eicup, EICU_CHANNEL_1);
    if ((sr & STM32_TIM_SR_CC2IF) != 0)
      _eicu_isr_invoke_edge_detect_cb(eicup, EICU_CHANNEL_2);
    if ((sr & STM32_TIM_SR_CC3IF) != 0)
      _eicu_isr_invoke_edge_detect_cb(eicup, EICU_CHANNEL_3);
    if ((sr & STM32_TIM_SR_CC4IF) != 0)
      _eicu_isr_invoke_edge_detect_cb(eicup, EICU_CHANNEL_4);
  }

  if ((sr & STM32_TIM_SR_UIF) != 0)
    _eicu_isr_invoke_overflow_cb(eicup);
}

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

#if STM32_EICU_USE_TIM1
#if !defined(STM32_TIM1_UP_HANDLER)
#error "STM32_TIM1_UP_HANDLER not defined"
#endif
/**
 * @brief   TIM1 compare interrupt handler.
 * @note    It is assumed that the various sources are only activated if the
 *          associated callback pointer is not equal to @p NULL in order to not
 *          perform an extra check in a potentially critical interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(STM32_TIM1_UP_HANDLER) {

  OSAL_IRQ_PROLOGUE();

  eicu_lld_serve_interrupt(&EICUD1);

  OSAL_IRQ_EPILOGUE();
}

#if !defined(STM32_TIM1_CC_HANDLER)
#error "STM32_TIM1_CC_HANDLER not defined"
#endif
/**
 * @brief   TIM1 compare interrupt handler.
 * @note    It is assumed that the various sources are only activated if the
 *          associated callback pointer is not equal to @p NULL in order to not
 *          perform an extra check in a potentially critical interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(STM32_TIM1_CC_HANDLER) {

  OSAL_IRQ_PROLOGUE();

  eicu_lld_serve_interrupt(&EICUD1);

  OSAL_IRQ_EPILOGUE();
}
#endif /* STM32_EICU_USE_TIM1 */

#if STM32_EICU_USE_TIM2

#if !defined(STM32_TIM2_HANDLER)
#error "STM32_TIM2_HANDLER not defined"
#endif
/**
 * @brief   TIM2 interrupt handler.
 * @note    It is assumed that the various sources are only activated if the
 *          associated callback pointer is not equal to @p NULL in order to not
 *          perform an extra check in a potentially critical interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(STM32_TIM2_HANDLER) {

  OSAL_IRQ_PROLOGUE();

  eicu_lld_serve_interrupt(&EICUD2);

  OSAL_IRQ_EPILOGUE();
}
#endif /* STM32_EICU_USE_TIM2 */

#if STM32_EICU_USE_TIM3
#if !defined(STM32_TIM3_HANDLER)
#error "STM32_TIM3_HANDLER not defined"
#endif
/**
 * @brief   TIM3 interrupt handler.
 * @note    It is assumed that the various sources are only activated if the
 *          associated callback pointer is not equal to @p NULL in order to not
 *          perform an extra check in a potentially critical interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(STM32_TIM3_HANDLER) {

  OSAL_IRQ_PROLOGUE();

  eicu_lld_serve_interrupt(&EICUD3);

  OSAL_IRQ_EPILOGUE();
}
#endif /* STM32_EICU_USE_TIM3 */

#if STM32_EICU_USE_TIM4
#if !defined(STM32_TIM4_HANDLER)
#error "STM32_TIM4_HANDLER not defined"
#endif
/**
 * @brief   TIM4 interrupt handler.
 * @note    It is assumed that the various sources are only activated if the
 *          associated callback pointer is not equal to @p NULL in order to not
 *          perform an extra check in a potentially critical interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(STM32_TIM4_HANDLER) {

  OSAL_IRQ_PROLOGUE();

  eicu_lld_serve_interrupt(&EICUD4);

  OSAL_IRQ_EPILOGUE();
}
#endif /* STM32_EICU_USE_TIM4 */

#if STM32_EICU_USE_TIM5
#if !defined(STM32_TIM5_HANDLER)
#error "STM32_TIM5_HANDLER not defined"
#endif
/**
 * @brief   TIM5 interrupt handler.
 * @note    It is assumed that the various sources are only activated if the
 *          associated callback pointer is not equal to @p NULL in order to not
 *          perform an extra check in a potentially critical interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(STM32_TIM5_HANDLER) {

  OSAL_IRQ_PROLOGUE();

  eicu_lld_serve_interrupt(&EICUD5);

  OSAL_IRQ_EPILOGUE();
}
#endif /* STM32_EICU_USE_TIM5 */

#if STM32_EICU_USE_TIM8
#if !defined(STM32_TIM8_UP_HANDLER)
#error "STM32_TIM8_UP_HANDLER not defined"
#endif
/**
 * @brief   TIM8 compare interrupt handler.
 * @note    It is assumed that the various sources are only activated if the
 *          associated callback pointer is not equal to @p NULL in order to not
 *          perform an extra check in a potentially critical interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(STM32_TIM8_UP_HANDLER) {

  OSAL_IRQ_PROLOGUE();

  eicu_lld_serve_interrupt(&EICUD8);

  OSAL_IRQ_EPILOGUE();
}

#if !defined(STM32_TIM8_CC_HANDLER)
#error "STM32_TIM8_CC_HANDLER not defined"
#endif
/**
 * @brief   TIM8 compare interrupt handler.
 * @note    It is assumed that the various sources are only activated if the
 *          associated callback pointer is not equal to @p NULL in order to not
 *          perform an extra check in a potentially critical interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(STM32_TIM8_CC_HANDLER) {

  OSAL_IRQ_PROLOGUE();

  eicu_lld_serve_interrupt(&EICUD8);

  OSAL_IRQ_EPILOGUE();
}
#endif /* STM32_EICU_USE_TIM8 */

#if STM32_EICU_USE_TIM9
#if !defined(STM32_TIM9_HANDLER)
#error "STM32_TIM9_HANDLER not defined"
#endif
/**
 * @brief   TIM9 interrupt handler.
 * @note    It is assumed that the various sources are only activated if the
 *          associated callback pointer is not equal to @p NULL in order to not
 *          perform an extra check in a potentially critical interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(STM32_TIM9_HANDLER) {

  OSAL_IRQ_PROLOGUE();

  eicu_lld_serve_interrupt(&EICUD9);

  OSAL_IRQ_EPILOGUE();
}
#endif /* STM32_EICU_USE_TIM9 */

#if STM32_EICU_USE_TIM12
#if !defined(STM32_TIM12_HANDLER)
#error "STM32_TIM12_HANDLER not defined"
#endif
/**
 * @brief   TIM12 interrupt handler.
 * @note    It is assumed that the various sources are only activated if the
 *          associated callback pointer is not equal to @p NULL in order to not
 *          perform an extra check in a potentially critical interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(STM32_TIM12_HANDLER) {

  OSAL_IRQ_PROLOGUE();

  eicu_lld_serve_interrupt(&EICUD12);

  OSAL_IRQ_EPILOGUE();
}
#endif /* STM32_EICU_USE_TIM12 */

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Low level EICU driver initialization.
 *
 * @notapi
 */
void eicu_lld_init(void) {
#if STM32_EICU_USE_TIM1
  /* Driver initialization.*/
  eicuObjectInit(&EICUD1);
  EICUD1.tim = STM32_TIM1;
#endif

#if STM32_EICU_USE_TIM2
  /* Driver initialization.*/
  eicuObjectInit(&EICUD2);
  EICUD2.tim = STM32_TIM2;
#endif

#if STM32_EICU_USE_TIM3
  /* Driver initialization.*/
  eicuObjectInit(&EICUD3);
  EICUD3.tim = STM32_TIM3;
#endif

#if STM32_EICU_USE_TIM4
  /* Driver initialization.*/
  eicuObjectInit(&EICUD4);
  EICUD4.tim = STM32_TIM4;
#endif

#if STM32_EICU_USE_TIM5
  /* Driver initialization.*/
  eicuObjectInit(&EICUD5);
  EICUD5.tim = STM32_TIM5;
#endif

#if STM32_EICU_USE_TIM8
  /* Driver initialization.*/
  eicuObjectInit(&EICUD8);
  EICUD8.tim = STM32_TIM8;
#endif

#if STM32_EICU_USE_TIM9
  /* Driver initialization.*/
  eicuObjectInit(&EICUD9);
  EICUD9.tim = STM32_TIM9;
#endif

#if STM32_EICU_USE_TIM12
  /* Driver initialization.*/
  eicuObjectInit(&EICUD12);
  EICUD12.tim = STM32_TIM12;
#endif
}

/**
 * @brief   Configures and activates the EICU peripheral.
 *
 * @param[in] eicup     Pointer to the @p EICUDriver object
 *
 * @notapi
 */
void eicu_lld_start(EICUDriver *eicup) {
  uint32_t psc;

  osalDbgAssert((eicup->config->iccfgp[0] != NULL) ||
                (eicup->config->iccfgp[1] != NULL) ||
                (eicup->config->iccfgp[2] != NULL) ||
                (eicup->config->iccfgp[3] != NULL),
                 "invalid input configuration");

  if (eicup->state == EICU_STOP) {
    /* Clock activation and timer reset.*/
#if STM32_EICU_USE_TIM1
    if (&EICUD1 == eicup) {
      rccEnableTIM1(FALSE);
      rccResetTIM1();
      nvicEnableVector(STM32_TIM1_UP_NUMBER, STM32_EICU_TIM1_IRQ_PRIORITY);
      nvicEnableVector(STM32_TIM1_CC_NUMBER, STM32_EICU_TIM1_IRQ_PRIORITY);
#if defined(STM32_TIM1CLK)
      eicup->clock = STM32_TIM1CLK;
#else
      eicup->clock = STM32_TIMCLK2;
#endif
    }
#endif
#if STM32_EICU_USE_TIM2
    if (&EICUD2 == eicup) {
      rccEnableTIM2(FALSE);
      rccResetTIM2();
      nvicEnableVector(STM32_TIM2_NUMBER, STM32_EICU_TIM2_IRQ_PRIORITY);
      eicup->clock = STM32_TIMCLK1;
    }
#endif
#if STM32_EICU_USE_TIM3
    if (&EICUD3 == eicup) {
      rccEnableTIM3(FALSE);
      rccResetTIM3();
      nvicEnableVector(STM32_TIM3_NUMBER, STM32_EICU_TIM3_IRQ_PRIORITY);
      eicup->clock = STM32_TIMCLK1;
    }
#endif
#if STM32_EICU_USE_TIM4
    if (&EICUD4 == eicup) {
      rccEnableTIM4(FALSE);
      rccResetTIM4();
      nvicEnableVector(STM32_TIM4_NUMBER, STM32_EICU_TIM4_IRQ_PRIORITY);
      eicup->clock = STM32_TIMCLK1;
    }
#endif
#if STM32_EICU_USE_TIM5
    if (&EICUD5 == eicup) {
      rccEnableTIM5(FALSE);
      rccResetTIM5();
      nvicEnableVector(STM32_TIM5_NUMBER, STM32_EICU_TIM5_IRQ_PRIORITY);
      eicup->clock = STM32_TIMCLK1;
    }
#endif
#if STM32_EICU_USE_TIM8
    if (&EICUD8 == eicup) {
      rccEnableTIM8(FALSE);
      rccResetTIM8();
      nvicEnableVector(STM32_TIM8_UP_NUMBER, STM32_EICU_TIM8_IRQ_PRIORITY);
      nvicEnableVector(STM32_TIM8_CC_NUMBER, STM32_EICU_TIM8_IRQ_PRIORITY);
#if defined(STM32_TIM8CLK)
      eicup->clock = STM32_TIM8CLK;
#else
      eicup->clock = STM32_TIMCLK2;
#endif
    }
#endif
#if STM32_EICU_USE_TIM9
    if (&EICUD9 == eicup) {
      rccEnableTIM9(FALSE);
      rccResetTIM9();
      nvicEnableVector(STM32_TIM9_NUMBER, STM32_EICU_TIM9_IRQ_PRIORITY);
      eicup->clock = STM32_TIMCLK2;
    }
#endif
#if STM32_EICU_USE_TIM12
    if (&EICUD12 == eicup) {
      rccEnableTIM12(FALSE);
      rccResetTIM12();
      nvicEnableVector(STM32_TIM12_NUMBER, STM32_EICU_TIM12_IRQ_PRIORITY);
      eicup->clock = STM32_TIMCLK1;
    }
#endif
  }
  else {
    /* Driver re-configuration scenario, it must be stopped first.*/
    eicup->tim->CR1    = 0;                  /* Timer disabled.              */
    eicup->tim->DIER   = eicup->config->dier &/* DMA-related DIER settings.   */
                        ~STM32_TIM_DIER_IRQ_MASK;
    eicup->tim->SR     = 0;                  /* Clear eventual pending IRQs. */
    eicup->tim->CCR[0] = 0;                  /* Comparator 1 disabled.       */
    eicup->tim->CCR[1] = 0;                  /* Comparator 2 disabled.       */
    eicup->tim->CNT    = 0;                  /* Counter reset to zero.       */
  }

  /* Timer configuration.*/
  psc = (eicup->clock / eicup->config->frequency) - 1;
  chDbgAssert((psc <= 0xFFFF) &&
             ((psc + 1) * eicup->config->frequency) == eicup->clock,
               "invalid frequency");
  eicup->tim->PSC   = (uint16_t)psc;
  eicup->tim->ARR   = 0xFFFF;

  /* Reset registers */
  eicup->tim->SMCR  = 0;
  eicup->tim->CCMR1 = 0;
  eicup->last_count[0] = 0;
  eicup->last_count[1] = 0;
  eicup->last_count[2] = 0;
  eicup->last_count[3] = 0;

#if STM32_EICU_USE_TIM9 && !STM32_EICU_USE_TIM12
  if (eicup != &EICUD9)
    eicup->tim->CCMR2 = 0;
#elif !STM32_EICU_USE_TIM9 && STM32_EICU_USE_TIM12
  if (eicup != &EICUD12)
    eicup->tim->CCMR2 = 0;
#elif STM32_EICU_USE_TIM9 && STM32_EICU_USE_TIM12
  if ((eicup != &EICUD9) && (eicup != &EICUD12))
    eicup->tim->CCMR2 = 0;
#else
  eicup->tim->CCMR2 = 0;
#endif

  if (eicup->config->input_type == EICU_INPUT_PWM)
  {
    if (eicup->config->iccfgp[0] != NULL) {
        /* Selected input 1.
           CCMR1_CC1S = 01 = CH1 Input on TI1.
           CCMR1_CC2S = 10 = CH2 Input on TI1.*/
        eicup->tim->CCMR1 = STM32_TIM_CCMR1_CC1S(1) | STM32_TIM_CCMR1_CC2S(2);
  
        /* SMCR_TS  = 101, input is TI1FP1.
           SMCR_SMS = 100, reset on rising edge.*/
        eicup->tim->SMCR  = STM32_TIM_SMCR_TS(5) | STM32_TIM_SMCR_SMS(4);
  
        /* The CCER settings depend on the selected trigger mode.
           EICU_INPUT_ACTIVE_HIGH: Active on rising edge, idle on falling edge.
           EICU_INPUT_ACTIVE_LOW:  Active on falling edge, idle on rising edge.
         */
        if (eicup->config->iccfgp[0]->mode == EICU_INPUT_ACTIVE_HIGH)
          eicup->tim->CCER = STM32_TIM_CCER_CC1E |
                             STM32_TIM_CCER_CC2E | STM32_TIM_CCER_CC2P;
        else
          eicup->tim->CCER = STM32_TIM_CCER_CC1E | STM32_TIM_CCER_CC1P |
                             STM32_TIM_CCER_CC2E;
  
        /* Direct pointers to the capture registers in order to make reading
           data faster from within callbacks.*/
        eicup->wccrp[0] = &eicup->tim->CCR[1];
        eicup->pccrp = &eicup->tim->CCR[0];
      } else {
        /* Selected input 2.
           CCMR1_CC1S = 10 = CH1 Input on TI2.
           CCMR1_CC2S = 01 = CH2 Input on TI2.*/
        eicup->tim->CCMR1 = STM32_TIM_CCMR1_CC1S(2) | STM32_TIM_CCMR1_CC2S(1);
  
        /* SMCR_TS  = 110, input is TI2FP2.
           SMCR_SMS = 100, reset on rising edge.*/
        eicup->tim->SMCR  = STM32_TIM_SMCR_TS(6) | STM32_TIM_SMCR_SMS(4);
  
        /* The CCER settings depend on the selected trigger mode.
           EICU_INPUT_ACTIVE_HIGH: Active on rising edge, idle on falling edge.
           EICU_INPUT_ACTIVE_LOW:  Active on falling edge, idle on rising edge.
         */
        if (eicup->config->iccfgp[1]->mode == EICU_INPUT_ACTIVE_HIGH)
          eicup->tim->CCER = STM32_TIM_CCER_CC1E | STM32_TIM_CCER_CC1P |
                             STM32_TIM_CCER_CC2E;
        else
          eicup->tim->CCER = STM32_TIM_CCER_CC1E |
                             STM32_TIM_CCER_CC2E | STM32_TIM_CCER_CC2P;
  
      /* Direct pointers to the capture registers in order to make reading
         data faster from within callbacks.*/
      eicup->wccrp[1] = &eicup->tim->CCR[0];
      eicup->pccrp = &eicup->tim->CCR[1];
    }
  } else { /* EICU_INPUT_EDGE & EICU_INPUT_PULSE */

    /* Set each input channel that is used as: a normal input capture channel,
       link the corresponding CCR register and set polarity. */

    /* Input capture channel 1 */
    if (eicup->config->iccfgp[0] != NULL) {
      /* Normal capture input input */
      eicup->tim->CCMR1 |= STM32_TIM_CCMR1_CC1S(1);

      /* Link CCR register */
      eicup->wccrp[0] = &eicup->tim->CCR[0];

      /* Set input polarity */
      if (eicup->config->iccfgp[0]->mode == EICU_INPUT_ACTIVE_HIGH)
        eicup->tim->CCER |= STM32_TIM_CCER_CC1E;
      else 
        eicup->tim->CCER |= STM32_TIM_CCER_CC1E | STM32_TIM_CCER_CC1P;
    }
    
    /* Input capture channel 2 */
    if (eicup->config->iccfgp[1] != NULL) {
      /* Normal capture input input */
      eicup->tim->CCMR1 |= STM32_TIM_CCMR1_CC2S(1);

      /* Link CCR register */
      eicup->wccrp[1] = &eicup->tim->CCR[1];

      /* Set input polarity */
      if (eicup->config->iccfgp[1]->mode == EICU_INPUT_ACTIVE_HIGH)
        eicup->tim->CCER |= STM32_TIM_CCER_CC2E;
      else 
        eicup->tim->CCER |= STM32_TIM_CCER_CC2E | STM32_TIM_CCER_CC2P;
    }

    /* Input capture channel 3 (not for TIM 9 and 12) */
    if (eicup->config->iccfgp[2] != NULL) {
      /* Normal capture input input */
      eicup->tim->CCMR2 |= STM32_TIM_CCMR2_CC3S(1);

      /* Link CCR register */
      eicup->wccrp[2] = &eicup->tim->CCR[2];

      /* Set input polarity */
      if (eicup->config->iccfgp[2]->mode == EICU_INPUT_ACTIVE_HIGH)
        eicup->tim->CCER |= STM32_TIM_CCER_CC3E;
      else 
        eicup->tim->CCER |= STM32_TIM_CCER_CC3E | STM32_TIM_CCER_CC3P;
    }

    /* Input capture channel 4 (not for TIM 9 and 12) */
    if (eicup->config->iccfgp[3] != NULL) {
      /* Normal capture input input */
      eicup->tim->CCMR2 |= STM32_TIM_CCMR2_CC4S(1);

      /* Link CCR register */
      eicup->wccrp[3] = &eicup->tim->CCR[3];

      /* Set input polarity */
      if (eicup->config->iccfgp[3]->mode == EICU_INPUT_ACTIVE_HIGH)
        eicup->tim->CCER |= STM32_TIM_CCER_CC4E;
      else 
        eicup->tim->CCER |= STM32_TIM_CCER_CC4E | STM32_TIM_CCER_CC4P;
    }
  }
}

/**
 * @brief   Deactivates the EICU peripheral.
 *
 * @param[in] eicup     Pointer to the @p EICUDriver object
 *
 * @notapi
 */
void eicu_lld_stop(EICUDriver *eicup) {
  if (eicup->state == EICU_READY) {
    /* Clock deactivation.*/
    eicup->tim->CR1  = 0;                     /* Timer disabled.              */
    eicup->tim->DIER = 0;                     /* All IRQs disabled.           */
    eicup->tim->SR   = 0;                     /* Clear eventual pending IRQs. */

#if STM32_EICU_USE_TIM1
    if (&EICUD1 == eicup) {
      nvicDisableVector(STM32_TIM1_UP_NUMBER);
      nvicDisableVector(STM32_TIM1_CC_NUMBER);
      rccDisableTIM1(FALSE);
    }
#endif
#if STM32_EICU_USE_TIM2
    if (&EICUD2 == eicup) {
      nvicDisableVector(STM32_TIM2_NUMBER);
      rccDisableTIM2(FALSE);
    }
#endif
#if STM32_EICU_USE_TIM3
    if (&EICUD3 == eicup) {
      nvicDisableVector(STM32_TIM3_NUMBER);
      rccDisableTIM3(FALSE);
    }
#endif
#if STM32_EICU_USE_TIM4
    if (&EICUD4 == eicup) {
      nvicDisableVector(STM32_TIM4_NUMBER);
      rccDisableTIM4(FALSE);
    }
#endif
#if STM32_EICU_USE_TIM5
    if (&EICUD5 == eicup) {
      nvicDisableVector(STM32_TIM5_NUMBER);
      rccDisableTIM5(FALSE);
    }
#endif
#if STM32_EICU_USE_TIM8
    if (&EICUD8 == eicup) {
      nvicDisableVector(STM32_TIM8_UP_NUMBER);
      nvicDisableVector(STM32_TIM8_CC_NUMBER);
      rccDisableTIM8(FALSE);
    }
#endif
#if STM32_EICU_USE_TIM9
    if (&EICUD9 == eicup) {
      nvicDisableVector(STM32_TIM9_NUMBER);
      rccDisableTIM9(FALSE);
    }
#endif
#if STM32_EICU_USE_TIM12
    if (&EICUD12 == eicup) {
      nvicDisableVector(STM32_TIM12_NUMBER);
      rccDisableTIM12(FALSE);
    }
#endif
  }
}

/**
 * @brief   Enables the EICU.
 *
 * @param[in] eicup     Pointer to the @p EICUDriver object
 *
 * @notapi
 */
void eicu_lld_enable(EICUDriver *eicup) {
  eicup->tim->EGR = STM32_TIM_EGR_UG;
  eicup->tim->SR = 0;                         /* Clear pending IRQs (if any). */

  if (eicup->config->input_type == EICU_INPUT_PWM) {
    if (eicup->config->iccfgp[0] != NULL) {
      if (eicup->config->period_cb != NULL)
        eicup->tim->DIER |= STM32_TIM_DIER_CC1IE;
      if ((eicup->config->iccfgp[EICU_CHANNEL_1] != NULL) &&
          (eicup->config->iccfgp[EICU_CHANNEL_1]->width_cb != NULL))
        eicup->tim->DIER |= STM32_TIM_DIER_CC2IE;
    } else {
      if ((eicup->config->iccfgp[EICU_CHANNEL_2] != NULL) &&
          (eicup->config->iccfgp[EICU_CHANNEL_2]->width_cb != NULL))
        eicup->tim->DIER |= STM32_TIM_DIER_CC1IE;
      if (eicup->config->period_cb != NULL)
        eicup->tim->DIER |= STM32_TIM_DIER_CC2IE;
    }
  } else { /* EICU_INPUT_PULSE & EICU_INPUT_EDGE */
    if ((eicup->config->iccfgp[EICU_CHANNEL_1] != NULL) &&
        (eicup->config->iccfgp[EICU_CHANNEL_1]->width_cb != NULL))
      eicup->tim->DIER |= STM32_TIM_DIER_CC1IE;
    if ((eicup->config->iccfgp[EICU_CHANNEL_2] != NULL) &&
        eicup->config->iccfgp[EICU_CHANNEL_2]->width_cb != NULL)
      eicup->tim->DIER |= STM32_TIM_DIER_CC2IE;
    if ((eicup->config->iccfgp[EICU_CHANNEL_3] != NULL) &&
        eicup->config->iccfgp[EICU_CHANNEL_3]->width_cb != NULL)
      eicup->tim->DIER |= STM32_TIM_DIER_CC3IE;
    if ((eicup->config->iccfgp[EICU_CHANNEL_4] != NULL) &&
        eicup->config->iccfgp[EICU_CHANNEL_4]->width_cb != NULL)
      eicup->tim->DIER |= STM32_TIM_DIER_CC4IE;
  }
  if (eicup->config->overflow_cb != NULL)
    eicup->tim->DIER |= STM32_TIM_DIER_UIE;

  eicup->tim->CR1 = STM32_TIM_CR1_URS | STM32_TIM_CR1_CEN;
}

/**
 * @brief   Disables the EICU.
 *
 * @param[in] eicup     Pointer to the @p EICUDriver object
 *
 * @notapi
 */
void eicu_lld_disable(EICUDriver *eicup) {
  eicup->tim->CR1   = 0;                      /* Initially stopped.           */
  eicup->tim->SR    = 0;                      /* Clear pending IRQs (if any). */

  /* All interrupts disabled.*/
  eicup->tim->DIER &= ~STM32_TIM_DIER_IRQ_MASK;
}

/**
 * @brief   Returns the width of the latest pulse.
 * @details The pulse width is defined as number of ticks between the start
 *          edge and the stop edge.
 *
 * @param[in] eicup     Pointer to the EICUDriver object.
 * @param[in] channel   The timer channel that fired the interrupt.
 * @return              The number of ticks.
 *
 * @notapi
 */
uint16_t eicu_lld_get_width(EICUDriver *eicup, uint16_t channel) {
  uint16_t capture, last_count;
  capture = eicu_lld_get_compare(eicup, channel);

  /* Add code to compensate for overflows when in pulse */
  if (eicup->config->input_type == EICU_INPUT_PULSE) {
    last_count = eicup->last_count[channel];

    if (capture > last_count)       /* No overflow */
      capture = capture - last_count;
    else if (capture < last_count)  /* Timer overflow */
      capture = ((0xFFFF - last_count) + capture); 
  }

  return capture;
}

#endif /* DRIVER_USE_EICU */
