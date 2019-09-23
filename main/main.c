/* Scan Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

/*
    This example shows how to use the All Channel Scan or Fast Scan to connect
    to a Wi-Fi network.

    In the Fast Scan mode, the scan will stop as soon as the first network matching
    the SSID is found. In this mode, an application can set threshold for the
    authentication mode and the Signal strength. Networks that do not meet the
    threshold requirements will be ignored.

    In the All Channel Scan mode, the scan will end only after all the channels
    are scanned, and connection will start with the best network. The networks
    can be sorted based on Authentication Mode or Signal Strength. The priority
    for the Authentication mode is:  WPA2 > WPA > WEP > Open
*/
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"

#define SSID_TO_SCAN CONFIG_SSID_TO_SCAN
#define MAX_APs 5 // For some reasos using 20 as max_aps crash everytime in the ap list print loop

static const char *TAG = "SIGSTREN";

wifi_scan_config_t scan_config = {
    .ssid = (uint8_t *)SSID_TO_SCAN,
    .bssid = 0,
    .channel = 0,
    .show_hidden = true};

static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_SCAN_DONE)
    {
        // get the list of APs found in the last scan
        uint16_t ap_num = MAX_APs;
        wifi_ap_record_t ap_records[MAX_APs];
        ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&ap_num, ap_records));

        // print the list
        printf("Found %d access points:\n", ap_num);
        printf("\n");
        printf("               SSID              | Channel | RSSI \n");
        printf("----------------------------------------------------------------\n");
        for (int i = 0; i < ap_num; i++)
            printf("%32s | %7d | %4d \n", (char *)ap_records[i].ssid, ap_records[i].primary, ap_records[i].rssi);
        printf("----------------------------------------------------------------\n");
    }
}

// Empty infinite task
void loop_task(void *pvParameter)
{
    while (1)
    {
        ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, true));
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

void app_main(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // initialize the tcp stack
    tcpip_adapter_init();

    // initialize the wifi event handler
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // configure, initialize and start the wifi driver
    wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_config));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    // configure and run the scan process in blocking mode
    ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, true));

    ESP_LOGI(TAG, "");

    // infinite loop
    xTaskCreate(&loop_task, "loop_task", 8192, NULL, 5, NULL);
}
