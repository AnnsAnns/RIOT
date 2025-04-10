// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once
#include <stdint.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file PHY init parameters and API
 */


/**
 * @brief Structure holding PHY init parameters
 */
typedef struct {
    uint8_t params[128];                    /*!< opaque PHY initialization parameters */
} esp_phy_init_data_t;

/**
 * @brief Opaque PHY calibration data
 */
typedef struct {
    uint8_t rf_cal_data[128];                   /*!< calibration data */
    uint32_t rx_gain_dc_table[125];
} esp_phy_calibration_data_t;

typedef enum {
    PHY_RF_CAL_PARTIAL = 0x00000000,        /*!< Do part of RF calibration. This should be used after power-on reset. */
    PHY_RF_CAL_NONE    = 0x00000001,        /*!< Don't do any RF calibration. This mode is only suggested to be used after deep sleep reset. */
    PHY_RF_CAL_FULL    = 0x00000002         /*!< Do full RF calibration. Produces best results, but also consumes a lot of time and current. Suggested to be used once. */
} esp_phy_calibration_mode_t;


/**
 * @brief Modules for modem sleep
 */
typedef enum {
    MODEM_WIFI_STATION_MODULE,     //!< Wi-Fi Station used
    MODEM_WIFI_SOFTAP_MODULE,      //!< Wi-Fi SoftAP used
    MODEM_WIFI_SNIFFER_MODULE,     //!< Wi-Fi Sniffer used
    MODEM_USER_MODULE,             //!< User used
    MODEM_MODULE_COUNT             //!< Number of items
} modem_sleep_module_t;

/**
 * @brief Module WIFI mask for medem sleep
 */
#define MODEM_WIFI_MASK ((1<<MODEM_WIFI_STATION_MODULE) |   \
                         (1<<MODEM_WIFI_SOFTAP_MODULE)  |   \
                         (1<<MODEM_WIFI_SNIFFER_MODULE))

/**
 * @brief Modules needing to call phy_rf_init
 */
typedef enum {
    PHY_WIFI_MODULE,        //!< Wi-Fi used
    PHY_MODEM_MODULE,       //!< Modem sleep used
    PHY_MODULE_COUNT        //!< Number of items
} phy_rf_module_t;

/**
 * @brief Get PHY init data
 *
 * If "Use a partition to store PHY init data" option is set in menuconfig,
 * This function will load PHY init data from a partition. Otherwise,
 * PHY init data will be compiled into the application itself, and this function
 * will return a pointer to PHY init data located in read-only memory (DROM).
 *
 * If "Use a partition to store PHY init data" option is enabled, this function
 * may return NULL if the data loaded from flash is not valid.
 *
 * @note Call esp_phy_release_init_data to release the pointer obtained using
 * this function after the call to esp_wifi_init.
 *
 * @return pointer to PHY init data structure
 */
const esp_phy_init_data_t *esp_phy_get_init_data(void);

/**
 * @brief Release PHY init data
 * @param data  pointer to PHY init data structure obtained from
 *              esp_phy_get_init_data function
 */
void esp_phy_release_init_data(const esp_phy_init_data_t *data);

/**
 * @brief Function called by esp_phy_init to load PHY calibration data
 *
 * This is a convenience function which can be used to load PHY calibration
 * data from NVS. Data can be stored to NVS using esp_phy_store_cal_data_to_nvs
 * function.
 *
 * If calibration data is not present in the NVS, or
 * data is not valid (was obtained for a chip with a different MAC address,
 * or obtained for a different version of software), this function will
 * return an error.
 *
 * @param out_cal_data pointer to calibration data structure to be filled with
 *                     loaded data.
 * @return ESP_OK on success
 */
esp_err_t esp_phy_load_cal_data_from_nvs(esp_phy_calibration_data_t *out_cal_data);

/**
 * @brief Function called by esp_phy_init to store PHY calibration data
 *
 * This is a convenience function which can be used to store PHY calibration
 * data to the NVS. Calibration data is returned by phy function.
 * Data saved using this function to the NVS can later be loaded using
 * esp_phy_store_cal_data_to_nvs function.
 *
 * @param cal_data pointer to calibration data which has to be saved.
 * @return ESP_OK on success
 */
esp_err_t esp_phy_store_cal_data_to_nvs(const esp_phy_calibration_data_t *cal_data);

/**
 * @brief Initialize PHY and RF module
 *
 * PHY and RF module should be initialized in order to use WiFi.
 * Now PHY and RF initializing job is done automatically when start WiFi. Users should not
 * call this API in their application.
 *
 * @param init_data  PHY parameters. Default set of parameters can
 *                   be obtained by calling esp_phy_get_default_init_data
 *                   function.
 * @param         mode              Calibration mode (Full, partial, or no calibration)
 * @param[in,out] calibration_data  Calibration data to use.
 * @return ESP_OK on success.
 * @return ESP_FAIL on fail.
 */
esp_err_t esp_phy_rf_init(const esp_phy_init_data_t *init_data, esp_phy_calibration_mode_t mode,
                          esp_phy_calibration_data_t *calibration_data, phy_rf_module_t module);

/**
 * @brief De-initialize PHY and RF module
 *
 * PHY module should be de-initialized in order to shutdown WiFi.
 * Now PHY and RF de-initializing job is done automatically when stop WiFi. Users should not
 * call this API in their application.
 *
 * @return ESP_OK on success.
 */
esp_err_t esp_phy_rf_deinit(phy_rf_module_t module);

/**
 * @brief Load calibration data from NVS and initialize PHY and RF module
 */
void esp_phy_load_cal_and_init(phy_rf_module_t module);

#ifdef __cplusplus
}
#endif

