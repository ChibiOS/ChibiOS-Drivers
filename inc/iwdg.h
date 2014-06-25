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

                                      ---

    A special exception to the GPL can be applied should you wish to distribute
    a combined work that includes ChibiOS/RT, without being obliged to provide
    the source code for any proprietary components. See the file exception.txt
    for full details of how and when the exception can be applied.
*/

/**
 * @file    iwdg.h
 * @brief   IWDG Driver macros and structures.
 *
 * @addtogroup IWDG
 * @{
 */

#ifndef _IWDG_H_
#define _IWDG_H_

#if HAL_USE_IWDG || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

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
  IWDG_UNINIT = 0,                   /**< Not initialized.                   */
  IWDG_STOP = 1,                     /**< Stopped.                           */
  IWDG_READY = 2,                    /**< Ready.                             */
} iwdgstate_t;

/**
 * @brief   Type of a structure representing a IWDG driver.
 */


#include "iwdg_lld.h"


/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
  void iwdgInit(void);
  void iwdgStart( IWDGDriver *iwdgp, const IWDGConfig * config );
  void iwdgReset( IWDGDriver *iwdgp );
#ifdef __cplusplus
}
#endif

#endif /* HAL_USE_IWDG */

#endif /* _IWDG_H_ */

/** @} */
