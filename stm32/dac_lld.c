/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011,2012 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    STM32/DACv1/dac_lld.c
 * @brief   STM32 DAC subsystem low level driver source.
 *
 * @addtogroup DAC
 * @{
 */

#include "ch.h"
#include "hal.h"

#include "dac.h"
#include "dac_lld.h"

#if HAL_USE_DAC || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#if !defined(DAC1)
#define DAC1 DAC
#define rccEnableDAC1 rccEnableDAC
#define rccDisableDAC1 rccDisableDAC
#endif

#define DAC_CHN1_DMA_CHANNEL                                                 \
  STM32_DMA_GETCHANNEL(STM32_DAC_CHN1_DMA_STREAM,                            \
                       STM32_DAC_CHN1_DMA_CHN)

#define DAC_CHN2_DMA_CHANNEL                                                 \
  STM32_DMA_GETCHANNEL(STM32_DAC_CHN2_DMA_STREAM,                            \
                       STM32_DAC_CHN2_DMA_CHN)
           
#define DAC_CHN3_DMA_CHANNEL                                                 \
  STM32_DMA_GETCHANNEL(STM32_DAC_CHN3_DMA_STREAM,                            \
                       STM32_DAC_CHN3_DMA_CHN)

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/** @brief CHN1 driver identifier.*/
#if STM32_DAC_USE_CHN1 || defined(__DOXYGEN__)
DACDriver DACD1;
#endif

/** @brief CHN2 driver identifier.*/
#if STM32_DAC_USE_CHN2 || defined(__DOXYGEN__)
DACDriver DACD2;
#endif

/** @brief CHN3 driver identifier.*/
#if STM32_DAC_USE_CHN3 || defined(__DOXYGEN__)
DACDriver DACD3;
#endif

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/


/**
 * @brief   Shared end/half-of-tx service routine.
 *
 * @param[in] dacp      pointer to the @p DACDriver object
 * @param[in] flags     pre-shifted content of the ISR register
 */
static void dac_lld_serve_tx_interrupt(DACDriver *dacp, uint32_t flags) {
#if defined(STM32_DAC_DMA_ERROR_HOOK)
  (void)dacp;
  if ((flags & (STM32_DMA_ISR_TEIF | STM32_DMA_ISR_DMEIF)) != 0) {
    /* DMA errors handling.*/
    _dac_isr_error_code(dacp, flags);
  }
  else {
    if ((flags & STM32_DMA_ISR_HTIF) != 0) {
      /* Half transfer processing.*/
      _dac_isr_half_code(dacp);
    }
    if ((flags & STM32_DMA_ISR_TCIF) != 0) {
      /* Transfer complete processing.*/
      _dac_isr_full_code(dacp);
    }
  }
#else
  (void)dacp;
  (void)flags;
#endif
}

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Low level DAC driver initialization.
 *
 * @notapi
 */
void dac_lld_init(void) {

#if STM32_DAC_USE_CHN1
  dacObjectInit(&DACD1);
  DACD1.dac     = DAC1;
  DACD1.tim     = STM32_TIM6;
  DACD1.irqprio = STM32_DAC_CHN1_IRQ_PRIORITY;
  DACD1.dma     = STM32_DMA_STREAM(STM32_DAC_CHN1_DMA_STREAM);
  DACD1.dmamode = STM32_DMA_CR_CHSEL(DAC_CHN1_DMA_CHANNEL) |                  \
                  STM32_DMA_CR_PL(STM32_DAC_CHN1_DMA_PRIORITY) |              \
                  STM32_DMA_CR_DIR_M2P |                                      \
                  STM32_DMA_CR_DMEIE |                                        \
                  STM32_DMA_CR_TEIE |                                         \
                  STM32_DMA_CR_MINC | STM32_DMA_CR_TCIE;
#endif

#if STM32_DAC_USE_CHN2
  dacObjectInit(&DACD2);
  DACD2.dac     = DAC1;
  DACD2.tim     = STM32_TIM7;
  DACD2.irqprio = STM32_DAC_CHN2_IRQ_PRIORITY;
  DACD2.dma     = STM32_DMA_STREAM(STM32_DAC_CHN2_DMA_STREAM);
  DACD2.dmamode = STM32_DMA_CR_CHSEL(DAC_CHN2_DMA_CHANNEL) |                  \
                  STM32_DMA_CR_PL(STM32_DAC_CHN2_DMA_PRIORITY) |              \
                  STM32_DMA_CR_DIR_M2P |                                      \
                  STM32_DMA_CR_DMEIE |                                        \
                  STM32_DMA_CR_TEIE |                                         \
                  STM32_DMA_CR_MINC | STM32_DMA_CR_TCIE;
#endif

#if STM32_DAC_USE_CHN3
  dacObjectInit(&DACD3);
  DACD3.dac     = DAC2;
  DACD3.tim     = STM32_TIM18;
  DACD3.irqprio = STM32_DAC_CHN3_IRQ_PRIORITY;
  DACD3.dma     = STM32_DMA_STREAM(STM32_DAC_CHN3_DMA_STREAM);
  DACD3.dmamode = STM32_DMA_CR_CHSEL(DAC_CHN3_DMA_CHANNEL) |                  \
                  STM32_DMA_CR_PL(STM32_DAC_CHN2_DMA_PRIORITY) |              \
                  STM32_DMA_CR_DIR_M2P |                                      \
                  STM32_DMA_CR_DMEIE |                                        \
                  STM32_DMA_CR_TEIE |                                         \
                  STM32_DMA_CR_MINC | STM32_DMA_CR_TCIE;
#endif
}

/**
 * @brief   Configures and activates the DAC peripheral.
 *
 * @param[in] dacp      pointer to the @p DACDriver object
 *
 * @notapi
 */
void dac_lld_start(DACDriver *dacp) {

#if STM32_DAC_USE_CHN1
  if (&DACD1 == dacp) {
    rccEnableDAC1(FALSE);
  }
#endif
#if STM32_DAC_USE_CHN2
  if (&DACD2 == dacp) {
    rccEnableDAC1(FALSE);
  }
#endif
#if STM32_DAC_USE_CHN3
  if (&DACD3 == dacp) {
    rccEnableDAC2(FALSE);
  }
#endif
}

/**
 * @brief   Deactivates the DAC peripheral.
 *
 * @param[in] dacp      pointer to the @p DACDriver object
 *
 * @notapi
 */
void dac_lld_stop(DACDriver *dacp) {

  dmaStreamRelease(dacp->dma);
  dacp->tim->CR1 &= ~TIM_CR1_CEN; /* Disable associated timer */
        
#if STM32_DAC_USE_CHN1
  if (&DACD1 == dacp) {
    dacp->dac->CR &= ~STM32_DAC_CR_EN; /* DAC1 disable.*/
  }
#endif
#if STM32_DAC_USE_CHN2
  if (&DACD2 == dacp) {
    dacp->dac->CR &= ~STM32_DAC_CR_EN << 16; /* DAC1 disable.*/
  }
#endif
#if STM32_DAC_USE_CHN3
  if (&DACD3 == dacp) {
    dacp->dac->CR &= ~STM32_DAC_CR_EN; /* DAC2 disable.*/
    rccDisableDAC2(FALSE); /* DAC Clock disable.*/
  }
#endif

  if (!(DAC1->CR & (STM32_DAC_CR_EN | STM32_DAC_CR_EN << 16))) {
    /* DAC Clock disable only if all channels are off.*/
    rccDisableDAC1(FALSE);
  }
}

/**
 * @brief   Sends data over the DAC bus.
 * @details This asynchronous function starts a transmit operation.
 * @post    At the end of the operation the configured callback is invoked.
 *
 * @param[in] dacp      pointer to the @p DACDriver object
 * @param[in] n         number of words to send
 * @param[in] txbuf     the pointer to the transmit buffer
 *
 * @notapi
 */
void dac_lld_start_conversion(DACDriver *dacp) {
	
  chDbgAssert(dacp->samples, "dac_lld_start_conversion(), #1", 
    "dacp->samples is NULL pointer");
    
  uint32_t regshift, dataoffset, arr, trgo;
  bool_t b;

#if STM32_DAC_USE_CHN1
    if (&DACD1 == dacp) {
      /* DAC1 CR data is at bits 0:15 */
      regshift = 0;
      dataoffset = 0;
      /* Timer setup */
      trgo = STM32_DAC_CR_TSEL_TIM6;
      rccEnableTIM6(FALSE);
      rccResetTIM6();
    }
#endif
#if STM32_DAC_USE_CHN2
    if (&DACD2 == dacp) {
      /* DAC2 CR data is at bits 16:31 */
      regshift = 16;
      dataoffset = &dacp->dac->DHR12R2 - &dacp->dac->DHR12R1;
      /* Timer setup */
      trgo = STM32_DAC_CR_TSEL_TIM7;
      rccEnableTIM7(FALSE);
      rccResetTIM7();
    }
#endif
#if STM32_DAC_USE_CHN3
    if (&DACD3 == dacp) {
      /* DAC3 CR data is at bits 0:15 */
      regshift = 0;
      dataoffset = 0;
      /* Timer setup */
      trgo = STM32_DAC_CR_TSEL_TIM18;
      rccEnableTIM18(FALSE);
      rccResetTIM18();
    }
#endif
    
#if STM32_DAC_USE_CHN1 || STM32_DAC_USE_CHN2 || STM32_DAC_USE_CHN3

    dacp->clock = STM32_TIMCLK1;
    arr = (dacp->clock / dacp->grpp->frequency);
    chDbgAssert((arr <= 0xFFFF),
        "dac_lld_start(), #1", "invalid frequency");

    /* Timer configuration.*/
    dacp->tim->CR1  = 0;                        /* Initially stopped.   */
    dacp->tim->PSC  = 0;                        /* Prescaler value.     */
    dacp->tim->DIER = 0;
    dacp->tim->ARR  = arr;
    dacp->tim->EGR  = TIM_EGR_UG;               /* Update event.        */
    dacp->tim->CR2  &= (uint16_t)~TIM_CR2_MMS;
    dacp->tim->CR2  |= (uint16_t)TIM_CR2_MMS_1; /* Enable TRGO updates. */
    dacp->tim->CNT  = 0;                        /* Reset counter.       */
    dacp->tim->SR   = 0;                        /* Clear pending IRQs.  */
    /* Update Event IRQ enabled. */

    /* DAC configuration */
    dacp->dac->CR &=  ~(STM32_DAC_CR_MASK << regshift);
    dacp->dac->CR |= (STM32_DAC_CR_EN | STM32_DAC_CR_DMAEN | dacp->config->cr_flags) << regshift;
      
    /* DMA setup. */
    b = dmaStreamAllocate(dacp->dma,
          dacp->irqprio,
          (stm32_dmaisr_t)dac_lld_serve_tx_interrupt,
          (void *)dacp);
    chDbgAssert(!b, "dac_lld_start(), #2", "stream already allocated");
    switch (dacp->config->dhrm) {
      /* Sets the DAC data register */
      case DAC_DHRM_12BIT_RIGHT:
        dmaStreamSetPeripheral(dacp->dma, &dacp->dac->DHR12R1 + dataoffset);
        dacp->dmamode = (dacp->dmamode & ~STM32_DMA_CR_SIZE_MASK) |
              STM32_DMA_CR_PSIZE_HWORD | STM32_DMA_CR_MSIZE_HWORD;
        break;
      case DAC_DHRM_12BIT_LEFT:
        dmaStreamSetPeripheral(dacp->dma, &dacp->dac->DHR12L1 + dataoffset);
        dacp->dmamode = (dacp->dmamode & ~STM32_DMA_CR_SIZE_MASK) |
              STM32_DMA_CR_PSIZE_HWORD | STM32_DMA_CR_MSIZE_HWORD;
        break;
      case DAC_DHRM_8BIT_RIGHT:
        dmaStreamSetPeripheral(dacp->dma, &dacp->dac->DHR8R1 + dataoffset);
        dacp->dmamode = (dacp->dmamode & ~STM32_DMA_CR_SIZE_MASK) |
              STM32_DMA_CR_PSIZE_BYTE | STM32_DMA_CR_MSIZE_BYTE;
        break;
#if defined(STM32_HAS_DAC_CHN2) && STM32_HAS_DAC_CHN2
      case DAC_DHRM_12BIT_RIGHT_DUAL:
        dmaStreamSetPeripheral(dacp->dma, &dacp->dac->DHR12RD);
        dacp->dmamode = (dacp->dmamode & ~STM32_DMA_CR_SIZE_MASK) |
              STM32_DMA_CR_PSIZE_WORD | STM32_DMA_CR_MSIZE_WORD;
        break;
      case DAC_DHRM_12BIT_LEFT_DUAL:
        dmaStreamSetPeripheral(dacp->dma, &dacp->dac->DHR12LD);
        dacp->dmamode = (dacp->dmamode & ~STM32_DMA_CR_SIZE_MASK) |
              STM32_DMA_CR_PSIZE_WORD | STM32_DMA_CR_MSIZE_WORD;
        break;
      case DAC_DHRM_8BIT_RIGHT_DUAL:
        dmaStreamSetPeripheral(dacp->dma, &dacp->dac->DHR8RD);
        dacp->dmamode = (dacp->dmamode & ~STM32_DMA_CR_SIZE_MASK) |
             STM32_DMA_CR_PSIZE_HWORD | STM32_DMA_CR_MSIZE_HWORD;
        break;
#endif
  }
  
  dacp->dac->CR |= trgo << regshift; /* Enable timer trigger */
#endif
  
  if (dacp->grpp->circular == true)
    dacp->dmamode |= STM32_DMA_CR_CIRC;
    
  /* DMA Setup */  
  dmaStreamSetMemory0(dacp->dma, dacp->samples);
  dmaStreamSetTransactionSize(dacp->dma, dacp->depth);
  dmaStreamSetMode(dacp->dma, dacp->dmamode | STM32_DMA_CR_EN);

  /* Timer Start.*/
  dacp->tim->CNT  = 0;
  dacp->tim->CR1  = TIM_CR1_CEN;
  
}

void dac_lld_single_convert(DACDriver *dacp, dacsample_t value) {
	
  uint32_t dataoffset = 0;
  uint32_t regshift = 0;
  volatile dacsample_t* data = NULL;
	
#if STM32_DAC_USE_CHN2
  if (&DACD2 == dacp) {
    regshift = 16;
    dataoffset = &dacp->dac->DHR12R2 - &dacp->dac->DHR12R1;
  }
#endif

  switch (dacp->config->dhrm) {
    /* Sets the DAC data register */
    case DAC_DHRM_12BIT_RIGHT:
      data = (dacsample_t*)&dacp->dac->DHR12R1 + dataoffset;
      break;
    case DAC_DHRM_12BIT_LEFT:
      data = (dacsample_t*)&dacp->dac->DHR12L1 + dataoffset;
      break;
    case DAC_DHRM_8BIT_RIGHT:
      data = (dacsample_t*)&dacp->dac->DHR8R1 + dataoffset;
      break;
#if defined(STM32_HAS_DAC_CHN2) && STM32_HAS_DAC_CHN2
    case DAC_DHRM_12BIT_RIGHT_DUAL:
      data = (dacsample_t*)&dacp->dac->DHR12RD;
      break;
    case DAC_DHRM_12BIT_LEFT_DUAL:
      data = (dacsample_t*)&dacp->dac->DHR12LD;
      break;
    case DAC_DHRM_8BIT_RIGHT_DUAL:
      data = (dacsample_t*)&dacp->dac->DHR8RD;
      break;
#endif
  }
  
  dacp->dac->CR &=  ~(STM32_DAC_CR_MASK << regshift);
  dacp->dac->CR = ((STM32_DAC_CR_EN | STM32_DAC_CR_TSEL_SOFT | dacp->config->cr_flags) << regshift);
  
  if (data != NULL)
    *data = value;
  
#if STM32_DAC_USE_CHN1
  if (&DACD1 == dacp) {
    dacp->dac->SWTRIGR = STM32_DAC_SWTRIGR1;
  }
#endif
#if STM32_DAC_USE_CHN2
  if (&DACD2 == dacp) {
    dacp->dac->SWTRIGR = STM32_DAC_SWTRIGR2;
  }
#endif
#if STM32_DAC_USE_CHN3
  if (&DACD3 == dacp) {
    dacp->dac->SWTRIGR = STM32_DAC_SWTRIGR1;
  }
#endif

}

void dac_lld_stop_conversion(DACDriver *dacp) {
	
  /* Timer stop. */
  dacp->tim->CR1  &= ~TIM_CR1_CEN;

  /* DMA disable and release. */
  dmaStreamDisable(dacp->dma);
  dmaStreamRelease(dacp->dma);
  
#if STM32_DAC_USE_CHN1
  if (&DACD1 == dacp) {
    rccDisableTIM6(FALSE);
  }
#endif
#if STM32_DAC_USE_CHN2
  if (&DACD2 == dacp) {
    rccDisableTIM7(FALSE);
  }
#endif
#if STM32_DAC_USE_CHN3
  if (&DACD3 == dacp) {
    rccDisableTIM18(FALSE);
  }
#endif
  
  dacp->state = DAC_READY;
}
#endif /* HAL_USE_DAC */

/** @} */
