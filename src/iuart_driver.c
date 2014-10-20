/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011,2012,2013 Giovanni Di Sirio.

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

                                      ---

    A special exception to the GPL can be applied should you wish to distribute
    a combined work that includes ChibiOS/RT, without being obliged to provide
    the source code for any proprietary components. See the file exception.txt
    for full details of how and when the exception can be applied.
*/

/**
 * @file    iuart.c
 * @brief   IUART Driver code.
 *
 * @addtogroup IUART
 * @{
 */

#include "ch.h"
#include "hal.h"

#include "iuart_driver.h"

#if DRIVER_USE_IUART || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   IUART Driver initialization.
 * @note    This function is implicitly invoked by @p halInit(), there is
 *          no need to explicitly initialize the driver.
 *
 * @init
 */
void iuartInit(void) {

  iuart_lld_init();
}

/**
 * @brief   Initializes the standard part of a @p IUARTDriver structure.
 *
 * @param[out] iuartp    pointer to the @p IUARTDriver object
 *
 * @init
 */
void iuartObjectInit(IUARTDriver *iuartp) {

  iuartp->state   = IUART_STOP;
  iuartp->txstate = IUART_TX_IDLE;
  iuartp->rxstate = IUART_RX_IDLE;
  iuartp->config  = NULL;
  /* Optional, user-defined initializer.*/
#if defined(IUART_DRIVER_EXT_INIT_HOOK)
  IUART_DRIVER_EXT_INIT_HOOK(iuartp);
#endif
}

/**
 * @brief   Configures and activates the IUART peripheral.
 *
 * @param[in] iuartp     pointer to the @p IUARTDriver object
 * @param[in] config    pointer to the @p IUARTConfig object
 *
 * @api
 */
void iuartStart(IUARTDriver *iuartp, const IUARTConfig *config) {

  osalDbgCheck((iuartp != NULL) && (config != NULL));

  osalSysLock();
  osalDbgAssert((iuartp->state == IUART_STOP) || (iuartp->state == IUART_READY),
              "invalid state");

  iuartp->config = config;
  iuart_lld_start(iuartp);
  iuartp->state = IUART_READY;
  osalSysUnlock();
}

/**
 * @brief   Deactivates the IUART peripheral.
 *
 * @param[in] iuartp     pointer to the @p IUARTDriver object
 *
 * @api
 */
void iuartStop(IUARTDriver *iuartp) {

  osalDbgCheck(iuartp != NULL);

  osalSysLock();
  osalDbgAssert((iuartp->state == IUART_STOP) || (iuartp->state == IUART_READY),
             "invalid state");

  iuart_lld_stop(iuartp);
  iuartp->state = IUART_STOP;
  iuartp->txstate = IUART_TX_IDLE;
  iuartp->rxstate = IUART_RX_IDLE;
  osalSysUnlock();
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
 * @api
 */
void iuartStartSend(IUARTDriver *iuartp, size_t n, const void *txbuf) {

  osalDbgCheck((iuartp != NULL) && (n > 0) && (txbuf != NULL));
             
  osalSysLock();
  osalDbgAssert(iuartp->state == IUART_READY,
              "is active");
  osalDbgAssert(iuartp->txstate != IUART_TX_ACTIVE,
              "tx active");

  iuart_lld_start_send(iuartp, n, txbuf);
  iuartp->txstate = IUART_TX_ACTIVE;
  osalSysUnlock();
}

/**
 * @brief   Starts a transmission on the IUART peripheral.
 * @note    The buffers are organized as uint8_t arrays for data sizes below
 *          or equal to 8 bits else it is organized as uint16_t arrays.
 * @note    This function has to be invoked from a lock zone.
 *
 * @param[in] iuartp     pointer to the @p IUARTDriver object
 * @param[in] n         number of data frames to send
 * @param[in] txbuf     the pointer to the transmit buffer
 *
 * @iclass
 */
void iuartStartSendI(IUARTDriver *iuartp, size_t n, const void *txbuf) {

  osalDbgCheckClassI();
  osalDbgCheck((iuartp != NULL) && (n > 0) && (txbuf != NULL));
  osalDbgAssert(iuartp->state == IUART_READY,
              "is active");
  osalDbgAssert(iuartp->txstate != IUART_TX_ACTIVE,
              "tx active");

  iuart_lld_start_send(iuartp, n, txbuf);
  iuartp->txstate = IUART_TX_ACTIVE;
}

/**
 * @brief   Stops any ongoing transmission.
 * @note    Stopping a transmission also suppresses the transmission callbacks.
 *
 * @param[in] iuartp     pointer to the @p IUARTDriver object
 *
 * @return              The number of data frames not transmitted by the
 *                      stopped transmit operation.
 * @retval 0            There was no transmit operation in progress.
 *
 * @api
 */
size_t iuartStopSend(IUARTDriver *iuartp) {
  size_t n;

  osalDbgCheck(iuartp != NULL);

  osalSysLock();
  osalDbgAssert(iuartp->state == IUART_READY, "not active");

  if (iuartp->txstate == IUART_TX_ACTIVE) {
    n = iuart_lld_stop_send(iuartp);
    iuartp->txstate = IUART_TX_IDLE;
  }
  else
    n = 0;
  osalSysUnlock();
  return n;
}

/**
 * @brief   Stops any ongoing transmission.
 * @note    Stopping a transmission also suppresses the transmission callbacks.
 * @note    This function has to be invoked from a lock zone.
 *
 * @param[in] iuartp     pointer to the @p IUARTDriver object
 *
 * @return              The number of data frames not transmitted by the
 *                      stopped transmit operation.
 * @retval 0            There was no transmit operation in progress.
 *
 * @iclass
 */
size_t iuartStopSendI(IUARTDriver *iuartp) {

  osalDbgCheckClassI();
  osalDbgCheck(iuartp != NULL);
  osalDbgAssert(iuartp->state == IUART_READY, "not active");

  if (iuartp->txstate == IUART_TX_ACTIVE) {
    size_t n = iuart_lld_stop_send(iuartp);
    iuartp->txstate = IUART_TX_IDLE;
    return n;
  }
  return 0;
}

/**
 * @brief   Starts a receive operation on the IUART peripheral.
 * @note    The buffers are organized as uint8_t arrays for data sizes below
 *          or equal to 8 bits else it is organized as uint16_t arrays.
 *
 * @param[in] iuartp     pointer to the @p IUARTDriver object
 * @param[in] n         number of data frames to send
 * @param[in] rxbuf     the pointer to the receive buffer
 *
 * @api
 */
void iuartStartReceive(IUARTDriver *iuartp, size_t n, void *rxbuf) {

  osalDbgCheck((iuartp != NULL) && (n > 0) && (rxbuf != NULL));

  osalSysLock();
  osalDbgAssert(iuartp->state == IUART_READY,
              "is active");
  osalDbgAssert(iuartp->rxstate != IUART_RX_ACTIVE,
               "rx active");

  iuart_lld_start_receive(iuartp, n, rxbuf);
  iuartp->rxstate = IUART_RX_ACTIVE;
  osalSysUnlock();
}

/**
 * @brief   Starts a receive operation on the IUART peripheral.
 * @note    The buffers are organized as uint8_t arrays for data sizes below
 *          or equal to 8 bits else it is organized as uint16_t arrays.
 * @note    This function has to be invoked from a lock zone.
 *
 * @param[in] iuartp     pointer to the @p IUARTDriver object
 * @param[in] n         number of data frames to send
 * @param[out] rxbuf    the pointer to the receive buffer
 *
 * @iclass
 */
void iuartStartReceiveI(IUARTDriver *iuartp, size_t n, void *rxbuf) {

  osalDbgCheckClassI();
  osalDbgCheck((iuartp != NULL) && (n > 0) && (rxbuf != NULL));
  osalDbgAssert(iuartp->state == IUART_READY,
              "is active");
  osalDbgAssert(iuartp->rxstate != IUART_RX_ACTIVE,
              "rx active");

  iuart_lld_start_receive(iuartp, n, rxbuf);
  iuartp->rxstate = IUART_RX_ACTIVE;
}

/**
 * @brief   Stops any ongoing receive operation.
 * @note    Stopping a receive operation also suppresses the receive callbacks.
 *
 * @param[in] iuartp     pointer to the @p IUARTDriver object
 *
 * @return              The number of data frames not received by the
 *                      stopped receive operation.
 * @retval 0            There was no receive operation in progress.
 *
 * @api
 */
size_t iuartStopReceive(IUARTDriver *iuartp) {
  size_t n;

  osalDbgCheck(iuartp != NULL);

  osalSysLock();
  osalDbgAssert(iuartp->state == IUART_READY,
               "not active");

  if (iuartp->rxstate == IUART_RX_ACTIVE) {
    n = iuart_lld_stop_receive(iuartp);
    iuartp->rxstate = IUART_RX_IDLE;
  }
  else
    n = 0;
  osalSysUnlock();
  return n;
}

/**
 * @brief   Stops any ongoing receive operation.
 * @note    Stopping a receive operation also suppresses the receive callbacks.
 * @note    This function has to be invoked from a lock zone.
 *
 * @param[in] iuartp      pointer to the @p IUARTDriver object
 *
 * @return              The number of data frames not received by the
 *                      stopped receive operation.
 * @retval 0            There was no receive operation in progress.
 *
 * @iclass
 */
size_t iuartStopReceiveI(IUARTDriver *iuartp) {

  osalDbgCheckClassI();
  osalDbgCheck(iuartp != NULL);
  osalDbgAssert(iuartp->state == IUART_READY,
              "not active");

  if (iuartp->rxstate == IUART_RX_ACTIVE) {
    size_t n = iuart_lld_stop_receive(iuartp);
    iuartp->rxstate = IUART_RX_IDLE;
    return n;
  }
  return 0;
}

#endif /* DRIVER_USE_IUART */

/** @} */
