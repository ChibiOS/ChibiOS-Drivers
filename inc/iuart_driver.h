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
 * @file    iuart.h
 * @brief   IUART Driver macros and structures.
 *
 * @addtogroup IUART
 * @{
 */

#ifndef _IUART_DRIVER_H_
#define _IUART_DRIVER_H_

#if DRIVER_USE_IUART || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/**
 * @name    IUART status flags
 * @{
 */
#define IUART_NO_ERROR           0   /**< @brief No pending conditions.      */
#define IUART_PARITY_ERROR       4   /**< @brief Parity error happened.      */
#define IUART_FRAMING_ERROR      8   /**< @brief Framing error happened.     */
#define IUART_OVERRUN_ERROR      16  /**< @brief Overflow happened.          */
#define IUART_NOISE_ERROR        32  /**< @brief Noise on the line.          */
#define IUART_BREAK_DETECTED     64  /**< @brief Break detected.             */
/** @} */

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/**
 * @brief   Driver state machine possible states.
 */
typedef enum {
  IUART_UNINIT = 0,                  /**< Not initialized.                   */
  IUART_STOP = 1,                    /**< Stopped.                           */
  IUART_READY = 2                    /**< Ready.                             */
} iuartstate_t;

/**
 * @brief   Transmitter state machine states.
 */
typedef enum {
  IUART_TX_IDLE = 0,                 /**< Not transmitting.                  */
  IUART_TX_ACTIVE = 1,               /**< Transmitting.                      */
  IUART_TX_COMPLETE = 2              /**< Buffer complete.                   */
} iuarttxstate_t;

/**
 * @brief   Receiver state machine states.
 */
typedef enum {
  IUART_RX_IDLE = 0,                 /**< Not receiving.                     */
  IUART_RX_ACTIVE = 1,               /**< Receiving.                         */
  IUART_RX_COMPLETE = 2              /**< Buffer complete.                   */
} iuartrxstate_t;

#include "iuart_driver_lld.h"

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
  void iuartInit(void);
  void iuartObjectInit(IUARTDriver *iuartp);
  void iuartStart(IUARTDriver *iuartp, const IUARTConfig *config);
  void iuartStop(IUARTDriver *iuartp);
  void iuartStartSend(IUARTDriver *iuartp, size_t n, const void *txbuf);
  void iuartStartSendI(IUARTDriver *iuartp, size_t n, const void *txbuf);
  size_t iuartStopSend(IUARTDriver *iuartp);
  size_t iuartStopSendI(IUARTDriver *iuartp);
  void iuartStartReceive(IUARTDriver *iuartp, size_t n, void *rxbuf);
  void iuartStartReceiveI(IUARTDriver *iuartp, size_t n, void *rxbuf);
  size_t iuartStopReceive(IUARTDriver *iuartp);
  size_t iuartStopReceiveI(IUARTDriver *iuartp);
#ifdef __cplusplus
}
#endif

#endif /* DRIVER_USE_IUART */

#endif /* _IUART_DRIVER_H_ */

/** @} */
