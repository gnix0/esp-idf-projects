#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/uart.h"

#include "esp_err.h"
#include "esp_log.h"

#define PATTERN_CHR_NUM (3)
#define BUF_SIZE        (1024)
#define RD_BUF_SIZE     (BUF_SIZE)
#define BAUD_RATE       115200

static const char *TAG = "UART Events Demo";

QueueHandle_t uart_queue;

void uart_event_task(void *pvParameters);

void app_main(void)
{
    uart_config_t uart_cfg = {
        .baud_rate  = BAUD_RATE,
        .data_bits  = UART_DATA_8_BITS,
        .parity     = UART_PARITY_DISABLE,
        .stop_bits  = UART_STOP_BITS_1,
        .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    uart_driver_install(UART_NUM_0, BUF_SIZE * 2, BUF_SIZE * 2, 5, &uart_queue, 0);
    uart_param_config(UART_NUM_0, &uart_cfg);

    uart_enable_pattern_det_baud_intr(UART_NUM_0, '*', PATTERN_CHR_NUM, 9, 0, 0);
    uart_pattern_queue_reset(UART_NUM_0, 20);

    xTaskCreate(
        uart_event_task,
        "uart_event_task",
        2048,
        NULL,
        12,
        NULL
    );

    vTaskDelete(app_main);
}

void uart_event_task(void *pvParameters)
{
    uart_event_t event;     // UART events
    size_t buffered_size;   // UART buffer size

    uint8_t dtmp = malloc(RD_BUF_SIZE); // UART data
    if (dtmp == NULL) {
        ESP_LOGE(TAG, "malloc failed");
        abort();
    }

    for (;;) {
        if (xQueueReceive(uart_queue, (void *)&event, (TickType_t)portMAX_DELAY)) { // Wait for UART event
            bzero(dtmp, RD_BUF_SIZE);
            ESP_LOGI(TAG, "uart[%d] event:", UART_NUM_0);

            switch(event.type) {
                case UART_DATA:
                    ESP_LOGI(TAG, "[UART DATA]: %d", event.size);
                    uart_read_bytes(UART_NUM_0, dtmp, event.size, portMAX_DELAY);
                    ESP_LOGI(TAG, "[DATA EVT]: %s", dtmp);
                    uart_write_bytes(UART_NUM_0, (const char *)dtmp, event.size);
                    break;
                
                case UART_FIFO_OVF:
                    ESP_LOGI(TAG, "hw fifo overflow");
                    uart_flush_input(UART_NUM_0);
                    xQueueReset(uart_queue);
                    break;

                case UART_BUFFER_FULL:
                    ESP_LOGI(TAG, "ring buffer full");
                    uart_flush_input(UART_NUM_0);
                    xQueueReset(uart_queue);
                    break;

                case UART_BREAK:
                    ESP_LOGI(TAG, "uart rx break");
                    break;

                case UART_PARITY_ERR:
                    ESP_LOGI(TAG, "uart frame parity error");
                    break;

                case UART_FRAME_ERR:
                    ESP_LOGI(TAG, "uart frame error");
                    break;

                case UART_PATTERN_DET:
                    uart_get_buffered_data_len(UART_NUM_0, &buffered_size);
                    int pos = uart_pattern_pop_pos(UART_NUM_0);
                    ESP_LOGI(TAG, "[UART PATTERN DETECTED] pos: %d, buffered size: %d", pos, buffered_size);
                    if (pos == -1)
                        uart_flush_input(UART_NUM_0);
                    else {
                        uart_read_bytes(UART_NUM_0, dtmp, pos, 100 / portTICK_PERIOD_MS);
                        uint8_t pat[PATTERN_CHR_NUM + 1];
                        memset(pat, 0, sizeof pat);
                        uart_read_bytes(UART_NUM_0, pat, PATTERN_CHR_NUM, 100 / portTICK_PERIOD_MS);
                        ESP_LOGI(TAG, "read data: %s", dtmp);
                        ESP_LOGI(TAG, "read pat: %s", pat);
                    }
                    break;

                default:
                    ESP_LOGI(TAG, "uart event type: %d", event.type);
                    break;
            }
        }
    }

    free(dtmp);
    dtmp = NULL;
    vTaskDelete(uart_event_task);
}
