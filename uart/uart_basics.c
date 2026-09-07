#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "driver/uart.h"

#include "esp_err.h"
#include "esp_log.h"

#include <string.h>

#define UART_TX     GPIO_NUM_16
#define UART_RX     GPIO_NUM_17
#define UART_PORT   UART_NUM_0
#define TX_BUF_SIZE 1024
#define RX_BUF_SIZE 1024
#define BAUD_RATE   9600

static const char TAG[] = "UART Basic Demo";

void app_main(void)
{
    esp_err_t ret;

    // Configure UART
    ESP_LOGI(TAG, "Configuring UART");
    uart_config_t uart_cfg = {
        .baud_rate  = BAUD_RATE,
        .data_bits  = UART_DATA_8_BITS,
        .parity     = UART_PARITY_DISABLE,
        .stop_bits  = UART_STOP_BITS_1,
        .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    ret = uart_param_config(UART_PORT, &uart_cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "uart_param_config failed (%s)", esp_err_to_name(ret));
        abort();
    }

    // Install UART
    ESP_LOGI(TAG, "Installing UART driver");
    ret = uart_driver_install(UART_PORT, RX_BUF_SIZE, 0, 0, NULL, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "uart_driver_install failed (%s)", esp_err_to_name(ret));
        abort();
    }

    char message[] = "Hello from ESP32-C6\n";

    for (uint8_t i = 0; i < 5; i++) {
        ESP_LOGI(TAG, "Sending: %s", message);
        uart_write_bytes(UART_PORT, message, sizeof message);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    uint8_t *data = malloc(RX_BUF_SIZE + 1);
    if (data == NULL) {
        ESP_LOGE(TAG, "malloc failed");
        abort();
    }

    for (;;) {
        int data_len = uart_read_bytes(UART_PORT, data, RX_BUF_SIZE, pdMS_TO_TICKS(20));
        if (data_len == -1) {
            ESP_LOGE(TAG, "uart_read_bytes failed with [ERROR -1]");
            abort();
        }

        if (data_len > 0) {
            data[data_len] = '\0';
            ESP_LOGI(TAG, "Received: %s", data);
        }
    }

    // Here, free(data); is unreachable. Hence, unnecessary
    // Same goes for vTaskDelete(NULL);
}