#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "driver/gpio.h"
#include "driver/gptimer.h"

#include "esp_err.h"
#include "esp_log.h"

#define BLED_GPIO   GPIO_NUM_4
#define BUTTON_GPIO GPIO_NUM_6

#define TIMER_RES           1000000
#define ALARM_RELOAD_CNT    2000000

#define ENABLED     1
#define DISABLED    0

static const char TAG[] = "One-shot Alarm Demo";

static gptimer_handle_t gptimer     = NULL;
static SemaphoreHandle_t timer_sem  = NULL;
static bool timer_active            = DISABLED;

static esp_err_t s_init(void);
static bool IRAM_ATTR s_timer_cb(gptimer_handle_t timer,
                                const gptimer_alarm_event_data_t *edata,
                                void *user_data);
static void led_control_task(void *arg);
                               
void app_main(void)
{
    timer_sem = xSemaphoreCreateBinary();
    if (timer_sem == NULL) {
        ESP_LOGE(TAG, "Semaphore creation failed");
        return;
    }

    // Start task for LED control; waits for timer event
    xTaskCreate(led_control_task, "led_control_task", 2048, NULL, 5, NULL);

    // Initialize GPIO and timer
    ESP_ERROR_CHECK(s_init());

    // Poll the button
    for (;;) {
        // Detect press, wait for release
        if (!gpio_get_level(BUTTON_GPIO) && !timer_active) {
            vTaskDelay(pdMS_TO_TICKS(20)); // Debounce

            if (!gpio_get_level(BUTTON_GPIO)) {
                ESP_LOGI(TAG, "Button pressed. Turning LED on and starting one-shot timer");
                gpio_set_level(BLED_GPIO, ENABLED);
                timer_active = ENABLED;

                gptimer_stop(gptimer);
                gptimer_set_raw_count(gptimer, 0);
                gptimer_start(gptimer);

                // Wait for release
                while (!gpio_get_level(BUTTON_GPIO))
                    vTaskDelay(pdMS_TO_TICKS(10));
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

static esp_err_t s_init(void)
{
    esp_err_t ret;

    gpio_config_t led_conf = {
        .pin_bit_mask   = (1ULL << BLED_GPIO),
        .mode           = GPIO_MODE_OUTPUT,
        .pull_up_en     = DISABLED,
        .pull_down_en   = DISABLED,
        .intr_type      = GPIO_INTR_DISABLE,
    };
    if ((ret = gpio_config(&led_conf)) != ESP_OK) return ret;

    gpio_config_t btn_conf = {
        .pin_bit_mask   = (1ULL << BUTTON_GPIO),
        .mode           = GPIO_MODE_INPUT,
        .pull_up_en     = ENABLED,
        .pull_down_en   = DISABLED,
        .intr_type      = GPIO_INTR_NEGEDGE,
    };
    if ((ret = gpio_config(&btn_conf)) != ESP_OK) return ret;

    const gptimer_config_t timer_conf = {
        .clk_src        = GPTIMER_CLK_SRC_DEFAULT,
        .direction      = GPTIMER_COUNT_UP,
        .resolution_hz  = TIMER_RES,
    };
    ESP_LOGI(TAG, "Configuring a new timer");
    if ((ret = gptimer_new_timer(&timer_conf, &gptimer)))
        return ret;

    ESP_LOGI(TAG, "Registering timer callback");
    const gptimer_event_callbacks_t timer_callbacks = {
        .on_alarm = s_timer_cb,
    };
    if ((ret = gptimer_register_event_callbacks(gptimer, &timer_callbacks, NULL)))
        return ret;

    ESP_LOGI(TAG, "Enabling timer");
    if ((ret = gptimer_enable(gptimer)))
        return ret;
    
    ESP_LOGI(TAG, "Configuring the alarm");
    gptimer_alarm_config_t alarm_cfg = {
        .reload_count               = 0,
        .alarm_count                = ALARM_RELOAD_CNT,
        .flags.auto_reload_on_alarm = DISABLED,
    };
    if ((ret = gptimer_set_alarm_action(gptimer, &alarm_cfg)))
        return ret;
    
    return ret;
}

bool IRAM_ATTR s_timer_cb(gptimer_handle_t timer,
                            const gptimer_alarm_event_data_t *edata,
                            void *user_data)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(timer_sem, &xHigherPriorityTaskWoken);

    return xHigherPriorityTaskWoken == pdTRUE;
}

static void led_control_task(void *arg)
{
    for (;;) {
        if (xSemaphoreTake(timer_sem, portMAX_DELAY) == pdTRUE) {
            gpio_set_level(BLED_GPIO, DISABLED);
            timer_active = DISABLED;
            ESP_LOGI(TAG, "Timer expired. Turning LED off");
        }
    }
}