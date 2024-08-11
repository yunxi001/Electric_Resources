/* SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

void mqtt_app_start(void);
esp_err_t mqtt_air_purifer_on(void);
esp_err_t mqtt_air_purifer_off(void);
esp_err_t mqtt_lamp_on(void);
esp_err_t mqtt_lampr_off(void);
esp_err_t mqtt_lamp_brighter(void);
esp_err_t mqtt_lampr_dimmer(void);
esp_err_t mqtt_led_on(void);
esp_err_t mqtt_led_off(void);

#ifdef __cplusplus
}
#endif