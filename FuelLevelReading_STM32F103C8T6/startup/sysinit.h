/**
  ******************************************************************************
  * @file    sysinit.h
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    10/15/2010
  * @brief   CMSIS Cortex-M3 Device Peripheral Access Layer System Header File.
  ******************************************************************************  
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  ******************************************************************************
  */


#ifndef __SYSTEM_STM32F10X_H
#define __SYSTEM_STM32F10X_H

#ifdef __cplusplus
 extern "C" {
#endif 


extern uint32_t  SystemCoreClock;          /*!< System Clock Frequency (Core Clock) */  
extern void 	 SystemInit(void);
extern void 	 SystemCoreClockUpdate(void);


#ifdef __cplusplus
}
#endif

#endif /*__SYSTEM_STM32F10X_H */

 
/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
