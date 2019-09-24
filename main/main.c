
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"

#include "driver/i2c.h"

#define EXAMPLE_I2C_PORT_NUMBER I2C_NUM_1 //I2C port number
#define EXAMPLE_I2C_FREQ_HZ 100000        //I2C master clock frequency
#define EXAMPLE_I2C_SCL_GPIO 22           //GPIO pin
#define EXAMPLE_I2C_SDA_GPIO 21           //GPIO pin
#define EXAMPLE_I2C_ADDR 0x09
#define EXAMPLE_I2C_WRITE_BIT I2C_MASTER_WRITE
#define EXAMPLE_I2C_READ_BIT I2C_MASTER_READ
#define EXAMPLE_I2C_ACK_CHECK_EN 0x1
#define EXAMPLE_I2C_ACK_CHECK_DIS 0x0
#define EXAMPLE_I2C_ACK_VAL 0x0
#define EXAMPLE_I2C_NACK_VAL 0x1

#define SSID_TO_SCAN CONFIG_SSID_TO_SCAN // Run $idf.py menuconfig to set this value. Default: "myssid"
#define MAX_APs 5                        // Investigate: For some reasos using 20 as max_aps crash everytime in the ap list print loop

static const char *TAG = "SIGSTREN";

// Prototypes
static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
void configWIFI(void);
void loop_task(void *pvParameter);
void configRGBLed(void);
void blinkRGBLed(void);
void app_main(void);

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

        if (ap_num > 0)
        {
            ESP_LOGI(TAG, "Found %s AP! Signal Strength: %d", (char *)ap_records[0].ssid, ap_records[0].rssi);
        }
    }
}

void configWIFI(void)
{
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
}

void loop_task(void *pvParameter)
{
    while (1)
    {
        ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, true));
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

void configRGBLed(void)
{
    int i2c_master_port = EXAMPLE_I2C_PORT_NUMBER;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = EXAMPLE_I2C_SDA_GPIO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = EXAMPLE_I2C_SCL_GPIO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = EXAMPLE_I2C_FREQ_HZ; //I2C frequency is the clock speed for a complete high low clock sequence
    i2c_param_config(i2c_master_port, &conf);
    i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0);
}

void blinkRGBLed(void)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (EXAMPLE_I2C_ADDR << 1) | EXAMPLE_I2C_WRITE_BIT, EXAMPLE_I2C_ACK_CHECK_EN);
    i2c_master_write_byte(cmd, 'c', EXAMPLE_I2C_ACK_CHECK_EN);  // Command fade to color
    i2c_master_write_byte(cmd, 0xff, EXAMPLE_I2C_ACK_CHECK_EN); // RED
    i2c_master_write_byte(cmd, 0xff, EXAMPLE_I2C_ACK_CHECK_EN); // GREEN
    i2c_master_write_byte(cmd, 0x00, EXAMPLE_I2C_ACK_CHECK_EN); // BLUE

    i2c_master_cmd_begin(EXAMPLE_I2C_PORT_NUMBER, cmd, 1000 / portTICK_RATE_MS);
    i2c_master_stop(cmd);
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

    configWIFI();

    // // configure and run the scan process in blocking mode
    // ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, true));

    configRGBLed();

    // infinite loop
    xTaskCreate(&loop_task, "loop_task", 2048, NULL, 5, NULL);
}
