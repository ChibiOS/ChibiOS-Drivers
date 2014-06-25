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
	
	
	IUART Driver modified to use interrupts throughout, as an option; just define:
		STM32_IUART_USE_INTERRUPTS	TRUE
	or	STM32_IUART_USE_DMA			TRUE
*/

/**
 * @file    STM32/USARTv2/iuart_lld.h
 * @brief   STM32 low level IUART driver header.
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

#ifndef _IUART_LLD_H_
#define _IUART_LLD_H_

#if HAL_USE_IUART || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @name    Configuration options
 * @{
 */
/**
 * @brief   IUART driver on USART1 enable switch.
 * @details If set to @p TRUE the support for USART1 is included.
 * @note    The default is @p FALSE.
 */
#if !defined(STM32_IUART_USE_USART1) || defined(__DOXYGEN__)
#define STM32_IUART_USE_USART1               FALSE
#endif

/**
 * @brief   IUART driver on USART2 enable switch.
 * @details If set to @p TRUE the support for USART2 is included.
 * @note    The default is @p FALSE.
 */
#if !defined(STM32_IUART_USE_USART2) || defined(__DOXYGEN__)
#define STM32_IUART_USE_USART2               FALSE
#endif

/**
 * @brief   IUART driver on USART3 enable switch.
 * @details If set to @p TRUE the support for USART3 is included.
 * @note    The default is @p FALSE.
 */
#if !defined(STM32_IUART_USE_USART3) || defined(__DOXYGEN__)
#define STM32_IUART_USE_USART3               FALSE
#endif

/**
 * @brief   USART1 interrupt priority level setting.
 */
#if !defined(STM32_IUART_USART1_IRQ_PRIORITY) || defined(__DOXYGEN__)
#define STM32_IUART_USART1_IRQ_PRIORITY      12
#endif

/**
 * @brief   USART2 interrupt priority level setting.
 */
#if !defined(STM32_IUART_USART2_IRQ_PRIORITY) || defined(__DOXYGEN__)
#define STM32_IUART_USART2_IRQ_PRIORITY      12
#endif

/**
 * @brief   USART3 interrupt priority level setting.
 */
#if !defined(STM32_IUART_USART3_IRQ_PRIORITY) || defined(__DOXYGEN__)
#define STM32_IUART_USART3_IRQ_PRIORITY      12
#endif

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

#if STM32_IUART_USE_USART1 && !STM32_HAS_USART1
#error "USART1 not present in the selected device"
#endif

#if STM32_IUART_USE_USART2 && !STM32_HAS_USART2
#error "USART2 not present in the selected device"
#endif

#if STM32_IUART_USE_USART3 && !STM32_HAS_USART3
#error "USART3 not present in the selected device"
#endif

#if !STM32_IUART_USE_USART1 && !STM32_IUART_USE_USART2 &&                     \
    !STM32_IUART_USE_USART3
#error "IUART driver activated but no USART/IUART peripheral assigned"
#endif

#if STM32_IUART_USE_USART1 &&                                                \
    !CORTEX_IS_VALID_KERNEL_PRIORITY(STM32_IUART_USART1_IRQ_PRIORITY)
#error "Invalid IRQ priority assigned to USART1"
#endif

#if STM32_IUART_USE_USART2 &&                                                \
    !CORTEX_IS_VALID_KERNEL_PRIORITY(STM32_IUART_USART2_IRQ_PRIORITY)
#error "Invalid IRQ priority assigned to USART2"
#endif

#if STM32_IUART_USE_USART3 &&                                                \
    !CORTEX_IS_VALID_KERNEL_PRIORITY(STM32_IUART_USART3_IRQ_PRIORITY)
#error "Invalid IRQ priority assigned to USART3"
#endif

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/**
 * @brief   IUART driver condition flags type.
 */
typedef uint32_t iuartflags_t;

/**
 * @brief   Structure representing an IUART driver.
 */
typedef struct IUARTDriver IUARTDriver;

/**
 * @brief   Generic IUART notification callback type.
 *
 * @param[in] iuartp     pointer to the @p IUARTDriver object
 */
typedef void (*iuartcb_t)(IUARTDriver *iuartp);

/**
 * @brief   Character received IUART notification callback type.
 *
 * @param[in] iuartp     pointer to the @p IUARTDriver object
 * @param[in] c         received character
 */
typedef void (*iuartccb_t)(IUARTDriver *iuartp, uint16_t c);

/**
 * @brief   Receive error IUART notification callback type.
 *
 * @param[in] iuartp     pointer to the @p IUARTDriver object
 * @param[in] e         receive error mask
 */
typedef void (*iuartecb_t)(IUARTDriver *iuartp, iuartflags_t e);

/**
 * @brief   Driver configuration structure.
 * @note    It could be empty on some architectures.
 */
typedef struct {
  /**
   * @brief End of transmission buffer callback.
   */
  iuartcb_t                  txend1_cb;
  /**
   * @brief Physical end of transmission callback.
   */
  iuartcb_t                  txend2_cb;
  /**
   * @brief Receive buffer filled callback.
   */
  iuartcb_t                  rxend_cb;
  /**
   * @brief Character received while out if the @p IUART_RECEIVE state.
   */
  iuartccb_t                 rxchar_cb;
  /**
   * @brief Receive error callback.
   */
  iuartecb_t                 rxerr_cb;
  /* End of the mandatory fields.*/
  /**
   * @brief Bit rate.
   */
  uint32_t                  speed;
  /**
   * @brief Initialization value for the CR1 register.
   */
  uint32_t                  cr1;
  /**
   * @brief Initialization value for the CR2 register.
   */
  uint32_t                  cr2;
  /**
   * @brief Initialization value for the CR3 register.
   */
  uint32_t                  cr3;
} IUARTConfig;

/**
 * @brief   Structure representing an IUART driver.
 */
struct IUARTDriver {
  /**
   * @brief Driver state.
   */
  iuartstate_t               state;
  /**
   * @brief Transmitter state.
   */
  iuarttxstate_t             txstate;
  /**
   * @brief Receiver state.
   */
  iuartrxstate_t             rxstate;
  /**
   * @brief Current configuration data.
   */
  const IUARTConfig          *config;
#if defined(IUART_DRIVER_EXT_FIELDS)
  IUART_DRIVER_EXT_FIELDS
#endif
  /* End of the mandatory fields.*/
  /**
   * @brief Pointer to the USART registers block.
   */
  USART_TypeDef             *usart;

  /**
   * @brief Default receive buffer while into @p IUART_RX_IDLE state.
   */
  volatile uint16_t         rxbuf;

  volatile uint16_t txCount;          // Number of bytes in transmit buffer
  volatile uint8_t  *txBuf;          // Pointer to current transmit buffer
  volatile uint16_t rxCount;          // Number of bytes for 'active' receive
  volatile uint8_t  *rxBuffer;        // Pointer to receive buffer if block receive active
};

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#if STM32_IUART_USE_USART1 && !defined(__DOXYGEN__)
extern IUARTDriver IUARTD1;
#endif

#if STM32_IUART_USE_USART2 && !defined(__DOXYGEN__)
extern IUARTDriver IUARTD2;
#endif

#if STM32_IUART_USE_USART3 && !defined(__DOXYGEN__)
extern IUARTDriver IUARTD3;
#endif

#ifdef __cplusplus
extern "C" {
#endif
  void iuart_lld_init(void);
  void iuart_lld_start(IUARTDriver *iuartp);
  void iuart_lld_stop(IUARTDriver *iuartp);
  void iuart_lld_start_send(IUARTDriver *iuartp, size_t n, const void *txbuf);
  size_t iuart_lld_stop_send(IUARTDriver *iuartp);
  void iuart_lld_start_receive(IUARTDriver *iuartp, size_t n, void *rxbuf);
  size_t iuart_lld_stop_receive(IUARTDriver *iuartp);
#ifdef __cplusplus
}
#endif

#endif /* HAL_USE_IUART */

#endif /* _IUART_LLD_H_ */

/** @} */
