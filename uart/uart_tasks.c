#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "driver/uart.h"

#include "esp_err.h"
#include "esp_log.h"

#define BAUD_RATE       115200
#define SEND_DATA_ERR   -1

static const char *TAG = "UART Tasks Demo";
static const char *TX_TASK_TAG = "TX_TASK";
static const char *RX_TASK_TAG = "RX_TASK";

static const int RX_BUF_SIZE = 512;

int send_data(const char *log_name, const char *data);

void tx_task(void *pvParameters);
void rx_task(void *pvParameters);

void app_main(void)
{
    esp_err_t ret;

    const uart_config_t uart_cfg = {
        .baud_rate  = BAUD_RATE,
        .data_bits  = UART_DATA_8_BITS,
        .parity     = UART_PARITY_DISABLE,
        .stop_bits  = UART_STOP_BITS_1,
        .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    ret = uart_driver_install(UART_NUM_0, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "uart_driver_install failed (%s)", esp_err_to_name(ret));
        abort();
    }

    ret = uart_param_config(UART_NUM_0, &uart_cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "uart_param_config failed (%s)", esp_err_to_name(ret));
        abort();
    }

    xTaskCreate(
        tx_task,
        "uart_tx_task",
        configMINIMAL_STACK_SIZE + 1024,
        NULL,
        1,
        NULL
    );

    xTaskCreate(
        rx_task,
        "uart_rx_task",
        configMINIMAL_STACK_SIZE + 1024 * 2,
        NULL,
        2,
        NULL
    );
}

int send_data(const char *log_name, const char *data)
{
    const unsigned int len  = (int) strlen(data);
    const int tx_bytes      = uart_write_bytes(UART_NUM_0, data, len);
    if (tx_bytes == -1) {
        ESP_LOGE(TAG, "uart_write_bytes failed");
        return SEND_DATA_ERR;
    }

    ESP_LOGI(log_name, "Wrote %d bytes, Message: %s", tx_bytes, data);
    return tx_bytes;
}

void tx_task(void *pvParameters)
{
    char data[128];
    uint32_t counter    = 0;
    int ret             = 0;

    for (;;) {
        sprintf(data, "Counting: %d\n", counter);
        ret = send_data(TX_TASK_TAG, data);
        if (ret == -1) {
            ESP_LOGE(TX_TASK_TAG, "send_data failed");
            abort();
        }

        counter++;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void rx_task(void *pvParameters)
{
    uint8_t *data = malloc(RX_BUF_SIZE + 1);
    if (data == NULL) {
        ESP_LOGE(RX_TASK_TAG, "malloc failed");
        abort();
    }

    for (;;) {
        const int rx_bytes = uart_read_bytes(UART_NUM_0, data, RX_BUF_SIZE, pdMS_TO_TICKS(1000));
        if (rx_bytes == -1) {
            ESP_LOGE(RX_TASK_TAG, "uart_read_bytes failed");
            abort();
        }

        if (rx_bytes > 0) {
            data[rx_bytes] = '\0';
            ESP_LOGI(RX_TASK_TAG, "Read %d bytes: %s", rx_bytes, data);
        }
    }
}