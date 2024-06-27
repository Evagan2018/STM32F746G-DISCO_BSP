/******************************************************************************
 * @file     vio_STM32F746G-DISCO.c
 * @brief    Virtual I/O implementation for board STM32F746G-DISCO
 * @version  V2.0.1
 * @date     12. June 2024
 ******************************************************************************/
/*
 * Copyright (c) 2020-2024 Arm Limited (or its affiliates).
 * All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*! \page vio_STM32F746G-DISCO Physical I/O Mapping

The table below lists the physical I/O mapping of this CMSIS-Driver VIO implementation.

Virtual Resource  | Variable       | Physical Resource on STM32F746G-DISCO          |
:-----------------|:---------------|:-----------------------------------------------|
vioBUTTON0        | vioSignalIn.0  | GPIO I.11: Button USER                         |
vioLED0           | vioSignalOut.0 | GPIO I.1:  LD1 GREEN                           |
*/

#include "cmsis_vio.h"

#include "RTE_Components.h"                     // Component selection
#include CMSIS_device_header

// VIO input, output definitions
#define VIO_VALUE_NUM           3U              // Number of values

// VIO input, output variables
static uint32_t vioSignalIn             __USED; // Memory for incoming signal
static uint32_t vioSignalOut            __USED; // Memory for outgoing signal
static int32_t  vioValue[VIO_VALUE_NUM] __USED; // Memory for value used in vioGetValue/vioSetValue

// Initialize test input, output.
void vioInit (void) {
#if !defined(CMSIS_VOUT) || !defined(CMSIS_VIN)
  GPIO_InitTypeDef GPIO_InitStruct;
#endif

  vioSignalIn  = 0U;
  vioSignalOut = 0U;

  for (uint8_t i = 0U; i < VIO_VALUE_NUM; i++) {
    vioValue[i] = 0U;
  }

#if !defined(CMSIS_VOUT)
  // Enable LEDs port clocks
  __HAL_RCC_GPIOI_CLK_ENABLE();

  // Initialize LEDs pins
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

  // Initialize LED0 pin
  GPIO_InitStruct.Pin   = GPIO_PIN_1;
  HAL_GPIO_Init    (GPIOI, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOI, GPIO_PIN_1, GPIO_PIN_SET);
#endif

#if !defined(CMSIS_VIN)
  // Enable buttons port clocks
  __HAL_RCC_GPIOI_CLK_ENABLE();

  // Initialize buttons pins
  GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

  // Initialize USER button pin
  GPIO_InitStruct.Pin   = GPIO_PIN_11;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
#endif
}

// Set signal output.
void vioSetSignal (uint32_t mask, uint32_t signal) {

  vioSignalOut &= ~mask;
  vioSignalOut |=  mask & signal;

#if !defined(CMSIS_VOUT)
  // Output signals to LEDs
  if ((mask & vioLED0) != 0U) {
    if ((signal & vioLED0) != 0U) {
      HAL_GPIO_WritePin(GPIOI, GPIO_PIN_1, GPIO_PIN_SET);
    } else {
      HAL_GPIO_WritePin(GPIOI, GPIO_PIN_1, GPIO_PIN_RESET);
    }
  }
#endif
}

// Get signal input.
uint32_t vioGetSignal (uint32_t mask) {
  uint32_t signal;

#if !defined(CMSIS_VIN)
  // Get input signals from buttons
  if ((mask & vioBUTTON0) != 0U) {
    if (HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_11) == GPIO_PIN_SET) {
      vioSignalIn |=  vioBUTTON0;
    } else {
      vioSignalIn &= ~vioBUTTON0;
    }
  }
#endif

  signal = vioSignalIn & mask;

  return signal;
}

// Set value output.
void vioSetValue (uint32_t id, int32_t value) {
  uint32_t index = id;

  if (index >= VIO_VALUE_NUM) {
    return;                             /* return in case of out-of-range index */
  }

  vioValue[index] = value;
}

// Get value input.
int32_t vioGetValue (uint32_t id) {
  uint32_t index = id;
  int32_t  value = 0;

  if (index >= VIO_VALUE_NUM) {
    return value;                       /* return 0 in case of out-of-range index */
  }

  value = vioValue[index];

  return value;
}
