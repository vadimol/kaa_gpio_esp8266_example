/*
 *  Copyright 2014-2016 CyberVision, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include "target_gpio_led.h"

#include <stdint.h>

#include "gpio.h"
#include "target.h"
#include "gpio_register.h"

#define BIT(nr)       (1UL << (nr))

// GPIO4 and GPIO5 is used for demo
static unsigned int demo_gpio_id[NUM_GPIO_LEDS] = {4, 5};

void target_gpio_led_init(void)
{
	GPIO_ConfigTypeDef pGPIOConfig;

	pGPIOConfig.GPIO_IntrType = GPIO_PIN_INTR_DISABLE;
	pGPIOConfig.GPIO_Mode = GPIO_Mode_Output;
	pGPIOConfig.GPIO_Pullup = GPIO_PullUp_EN;

	for (unsigned int i = 0; i < NUM_GPIO_LEDS; i++) {
		pGPIOConfig.GPIO_Pin = (BIT(demo_gpio_id[i]));
		gpio_config(&pGPIOConfig);

		GPIO_OUTPUT_SET(demo_gpio_id[i], 0);
	}
}

void target_gpio_led_toggle(uint32_t id, bool status)
{
	if (id >= NUM_GPIO_LEDS) {
		return;
	}

    if (status) {
        GPIO_OUTPUT_SET(demo_gpio_id[id], 1);
    } else {
        GPIO_OUTPUT_SET(demo_gpio_id[id], 0);
    }
}
