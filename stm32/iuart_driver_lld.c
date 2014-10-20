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

/**
 * @file    STM32/USARTv2/iuart_lld.c
 * @brief   STM32 low level IUART driver code.
 *
 * @addtogroup IUART
 * @{
 */

/**
 * Allows selection between DMA (the default) and interrupts for data transfer
 *
 * To enable interrupt handling, add to mcuconf.h:
 * #define STM32_IUART_USE_INTERRUPTS TRUE
 *
 * This enables interrupt handling for all defined IUART channels, releasing DMA
 * channels for other purposes.
 */
#include "hal.h"

#include "iuart_driver.h"

#if DRIVER_USE_IUART || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/** @brief USART1 IUART driver identifier.*/
#if STM32_IUART_USE_USART1 || defined(__DOXYGEN__)
IUARTDriver IUARTD1;
#endif

/** @brief USART2 IUART driver identifier.*/
#if STM32_IUART_USE_USART2 || defined(__DOXYGEN__)
IUARTDriver IUARTD2;
#endif

/** @brief USART3 IUART driver identifier.*/
#if STM32_IUART_USE_USART3 || defined(__DOXYGEN__)
IUARTDriver IUARTD3;
#endif

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/**
 * @brief   Status bits translation.
 *
 * @param[in] sr        USART SR register value
 *
 * @return  The error flags.
 */
static iuartflags_t translate_errors(uint32_t isr) 
{
  iuartflags_t sts = 0;

  if (isr & USART_ISR_ORE)
    sts |= IUART_OVERRUN_ERROR;
  if (isr & USART_ISR_PE)
    sts |= IUART_PARITY_ERROR;
  if (isr & USART_ISR_FE)
    sts |= IUART_FRAMING_ERROR;
  if (isr & USART_ISR_NE)
    sts |= IUART_NOISE_ERROR;
  if (isr & USART_ISR_LBDF)
    sts |= IUART_BREAK_DETECTED;
  return sts;
}

/**
 * @brief   USART de-initialization.
 * @details This function must be invoked with interrupts disabled.
 *
 * @param[in] iuartp     pointer to the @p IUARTDriver object
 */
static void usart_stop(IUARTDriver *iuartp) 
{
  /* Stops USART operations.*/
  iuartp->usart->CR1 = 0;
  iuartp->usart->CR2 = 0;
  iuartp->usart->CR3 = 0;
}


/**
 * @brief   USART initialization.
 * @details This function must be invoked with interrupts disabled.
 *
 * @param[in] iuartp     pointer to the @p IUARTDriver object
 */
static void usart_start(IUARTDriver *iuartp) 
{
  USART_TypeDef *u = iuartp->usart;

  /* Defensive programming, starting from a clean state.*/
  usart_stop(iuartp);

  /* Baud rate setting.*/
#if defined(STM32F0XX)
  if (iuartp->usart == USART1)
    u->BRR = STM32_USART1CLK / iuartp->config->speed;
  else
    u->BRR = STM32_PCLK / iuartp->config->speed;
#else /* !defined(STM32F0XX) */
  if (iuartp->usart == USART1)
    u->BRR = STM32_PCLK2 / iuartp->config->speed;
  else
    u->BRR = STM32_PCLK1 / iuartp->config->speed;
#endif /* !defined(STM32F0XX) */

  /* Resetting eventual pending status flags.*/
  u->ICR = 0xFFFFFFFF;

  /* Note that some bits are enforced because required for correct driver
     operations.*/
  u->CR2 = iuartp->config->cr2 | USART_CR2_LBDIE;

  u->CR3 = iuartp->config->cr3 | USART_CR3_EIE;
  u->CR1 = iuartp->config->cr1 | USART_CR1_UE | USART_CR1_PEIE |
                            USART_CR1_RXNEIE | USART_CR1_TE |
                            USART_CR1_RE;       // Enable receive interrupts straight away
}


/**
 * @brief   USART common service routine.
 *
 * @param[in] iuartp     pointer to the @p IUARTDriver object
 */
static void serve_usart_irq(IUARTDriver *iuartp) {

  uint32_t isr;
  USART_TypeDef *u = iuartp->usart;

  /* Reading and clearing status.*/
  isr = u->ISR;
  u->ICR = isr;

  /* Error condition detection.*/
  if (isr & (USART_ISR_LBDF | USART_ISR_ORE | USART_ISR_NE |
             USART_ISR_FE  | USART_ISR_PE)) {
    if (iuartp->config->rxerr_cb != NULL)
      iuartp->config->rxerr_cb(iuartp, translate_errors(isr));        // Receive error callback
  }

  uint32_t cr1 = u->CR1;

  /* Data available (receive). */
  if (isr & USART_ISR_RXNE) 
  {
	iuartp->rxbuf = (uint16_t)u->RDR;            // Get the character
	if (iuartp->rxCount > 0)
	{   // Must be a block receive
	  *iuartp->rxBuffer++ = iuartp->rxbuf;        // Problem with >8-bit data here - @TODO: needs handling
	  (iuartp->rxCount)--;
	  if (iuartp->rxCount == 0)
	  {
	    /* Receiver in active state, a callback is generated, if enabled, after
	       a completed transfer.*/
	    iuartp->rxstate = IUART_RX_COMPLETE;
	    if (iuartp->config->rxend_cb != NULL)
	      iuartp->config->rxend_cb(iuartp);

	    /* If the callback didn't explicitly change state then the receiver
	       automatically returns to the idle state.*/
	    if (iuartp->rxstate == IUART_RX_COMPLETE) {
	      iuartp->rxstate = IUART_RX_IDLE;
	    }
	  }
	}
	else
	{   // Receive character while in IUART_RX_IDLE mode
      if (iuartp->config->rxchar_cb != NULL)
        iuartp->config->rxchar_cb(iuartp, iuartp->rxbuf);				// Receive character callback
	}
  }

  /* Transmission buffer empty.*/
  if ((cr1 & USART_CR1_TXEIE) && (isr & USART_ISR_TXE)) 
  {
    u->TDR = *iuartp->txBuf++;         // Next character to transmit output buffer
    if (--(iuartp->txCount) == 0)
    {
      iuartp->txBuf = NULL;
      /* A callback is generated, if enabled, after a completed transfer.*/
      iuartp->txstate = IUART_TX_COMPLETE;
	  if (iuartp->config->txend1_cb != NULL)
		iuartp->config->txend1_cb(iuartp);            // Signal that Tx buffer finished with

	  /* If the callback didn't explicitly change state then the transmitter
	     automatically returns to the idle state.*/
	  if (iuartp->txstate == IUART_TX_COMPLETE)
	    iuartp->txstate = IUART_TX_IDLE;
      u->CR1 = (cr1 & ~USART_CR1_TXEIE) | USART_CR1_TCIE;       // Disable transmit data interrupt, enable TxBuffer empty
    }
  }
  /* Physical transmission end.*/
  if (isr & USART_ISR_TC)
  {
    if (iuartp->config->txend2_cb != NULL)
      iuartp->config->txend2_cb(iuartp);      // Signal that whole transmit message gone
    u->CR1 = cr1 & ~USART_CR1_TCIE;         // Disable transmit buffer empty interrupt
  }
}



/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

#if STM32_IUART_USE_USART1 || defined(__DOXYGEN__)
#if !defined(STM32_USART1_HANDLER)
#error "STM32_USART1_HANDLER not defined"
#endif
/**
 * @brief   USART1 IRQ handler.
 *
 * @isr
 */
CH_IRQ_HANDLER(STM32_USART1_HANDLER) {

  CH_IRQ_PROLOGUE();

  serve_usart_irq(&IUARTD1);

  CH_IRQ_EPILOGUE();
}
#endif /* STM32_IUART_USE_USART1 */

#if STM32_IUART_USE_USART2 || defined(__DOXYGEN__)
#if !defined(STM32_USART2_HANDLER)
#error "STM32_USART2_HANDLER not defined"
#endif
/**
 * @brief   USART2 IRQ handler.
 *
 * @isr
 */
CH_IRQ_HANDLER(STM32_USART2_HANDLER) {

  CH_IRQ_PROLOGUE();

  serve_usart_irq(&IUARTD2);

  CH_IRQ_EPILOGUE();
}
#endif /* STM32_IUART_USE_USART2 */

#if STM32_IUART_USE_USART3 || defined(__DOXYGEN__)
#if !defined(STM32_USART3_HANDLER)
#error "STM32_USART3_HANDLER not defined"
#endif
/**
 * @brief   USART3 IRQ handler.
 *
 * @isr
 */
CH_IRQ_HANDLER(STM32_USART3_HANDLER) {

  CH_IRQ_PROLOGUE();

  serve_usart_irq(&IUARTD3);

  CH_IRQ_EPILOGUE();
}
#endif /* STM32_IUART_USE_USART3 */

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Low level IUART driver initialization.
 *
 * @notapi
 */
void iuart_lld_init(void) {

#if STM32_IUART_USE_USART1
  iuartObjectInit(&IUARTD1);
  IUARTD1.usart   = USART1;
#endif

#if STM32_IUART_USE_USART2
  iuartObjectInit(&IUARTD2);
  IUARTD2.usart   = USART2;
#endif

#if STM32_IUART_USE_USART3
  iuartObjectInit(&IUARTD3);
  IUARTD3.usart   = USART3;
#endif
}

/**
 * @brief   Configures and activates the IUART peripheral.
 *
 * @param[in] iuartp     pointer to the @p IUARTDriver object
 *
 * @notapi
 */
void iuart_lld_start(IUARTDriver *iuartp) {

  if (iuartp->state == IUART_STOP) {
#if STM32_IUART_USE_USART1
    if (&IUARTD1 == iuartp) 
	{
      rccEnableUSART1(FALSE);
      nvicEnableVector(STM32_USART1_NUMBER, STM32_IUART_USART1_IRQ_PRIORITY);
    }
#endif

#if STM32_IUART_USE_USART2
    if (&IUARTD2 == iuartp) {

      rccEnableUSART2(FALSE);
      nvicEnableVector(STM32_USART2_NUMBER, STM32_IUART_USART2_IRQ_PRIORITY);
    }
#endif

#if STM32_IUART_USE_USART3
    if (&IUARTD3 == iuartp) {
      bool b;
      rccEnableUSART3(FALSE);
      nvicEnableVector(STM32_USART3_NUMBER, STM32_IUART_USART3_IRQ_PRIORITY);
    }
#endif

    iuartp->rxbuf = 0;
    iuartp->rxCount = 0;
    iuartp->rxBuffer = NULL;
  }

  iuartp->rxstate = IUART_RX_IDLE;
  iuartp->txstate = IUART_TX_IDLE;
  usart_start(iuartp);
}

/**
 * @brief   Deactivates the IUART peripheral.
 *
 * @param[in] iuartp     pointer to the @p IUARTDriver object
 *
 * @notapi
 */
void iuart_lld_stop(IUARTDriver *iuartp) {

  if (iuartp->state == IUART_READY) {
    usart_stop(iuartp);

#if STM32_IUART_USE_USART1
    if (&IUARTD1 == iuartp) {
      nvicDisableVector(STM32_USART1_NUMBER);
      rccDisableUSART1(FALSE);
      return;
    }
#endif

#if STM32_IUART_USE_USART2
    if (&IUARTD2 == iuartp) {
      nvicDisableVector(STM32_USART2_NUMBER);
      rccDisableUSART2(FALSE);
      return;
    }
#endif

#if STM32_IUART_USE_USART3
    if (&IUARTD3 == iuartp) {
      nvicDisableVector(STM32_USART3_NUMBER);
      rccDisableUSART3(FALSE);
      return;
    }
#endif
  }
}

/**
 * @brief   Starts a transmission on the IUART peripheral.
 * @note    The buffers are organized as uint8_t arrays for data sizes below
 *          or equal to 8 bits else it is organized as uint16_t arrays.
 *
 * @param[in] iuartp     pointer to the @p IUARTDriver object
 * @param[in] n         number of data frames to send
 * @param[in] txbuf     the pointer to the transmit buffer
 *
 * @notapi
 */
void iuart_lld_start_send(IUARTDriver *iuartp, size_t n, const void *txbuf) {

  if (iuartp->txCount)
  {
    return;             // Already transmission in progress - not much we can do
  }
  iuartp->txBuf = (uint8_t *)txbuf;
  if (0)
  {
    iuartp->txCount = 2*n;     // 16-bit data  - @TODO: How do we determine this?
  }
  else
  {
    iuartp->txCount = n;     // 8-bit data
  }

  // Now enable transmit interrupts - just the data interrupt
  iuartp->usart->CR1 |= USART_CR1_TXEIE;
}

/**
 * @brief   Stops any ongoing transmission.
 * @note    Stopping a transmission also suppresses the transmission callbacks.
 *
 * @param[in] iuartp     pointer to the @p IUARTDriver object
 *
 * @return              The number of data frames not transmitted by the
 *                      stopped transmit operation.
 *
 * @notapi
 */
size_t iuart_lld_stop_send(IUARTDriver *iuartp) {

  size_t rem = iuartp->txCount;
  iuartp->usart->CR1 &= ~(USART_CR1_TXEIE | USART_CR1_TCIE);
  iuartp->txCount = 0;        // Clear character count and buffer reference
  iuartp->txBuf = NULL;
  return rem;
}

/**
 * @brief   Starts a receive operation on the IUART peripheral.
 * @note    The buffers are organized as uint8_t arrays for data sizes below
 *          or equal to 8 bits else it is organized as uint16_t arrays.
 *
 * @param[in] iuartp     pointer to the @p IUARTDriver object
 * @param[in] n         number of data frames to receive
 * @param[out] rxbuf    the pointer to the receive buffer
 *
 * @notapi
 */
void iuart_lld_start_receive(IUARTDriver *iuartp, size_t n, void *rxbuf) {


  /* Just copy across new info - abandon any previous receive in progress */
  osalSysLock();
  iuartp->rxBuffer = rxbuf;
  iuartp->rxCount = n;
  iuartp->rxstate = IUART_RX_ACTIVE;
  osalSysUnlock();
}

/**
 * @brief   Stops any ongoing receive operation.
 * @note    Stopping a receive operation also suppresses the receive callbacks.
 *
 * @param[in] iuartp     pointer to the @p IUARTDriver object
 *
 * @return              The number of data frames not received by the
 *                      stopped receive operation.
 *
 * @notapi
 */
size_t iuart_lld_stop_receive(IUARTDriver *iuartp) {
  size_t n;

  osalSysLock();
  iuartp->rxBuffer = NULL;
  n = iuartp->rxCount;
  iuartp->rxCount = 0;
  iuartp->rxstate = IUART_RX_IDLE;
  osalSysUnlock();

  return n;
}

#endif /* DRIVER_USE_IUART */

/** @} */
