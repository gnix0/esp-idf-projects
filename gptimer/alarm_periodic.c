#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "driver/gptimer.h"

#include "esp_err.h"
#include "esp_log.h"

#define BLED_GPIO   GPIO_NUM_4

#define ENABLED     1
#define DISABLED    0

static esp_err_t s_init(void);
static esp_err_t s_release(void);
static bool IRAM_ATTR s_timer_cb(gptimer_handle_t timer,
                                const gptimer_alarm_event_data_t *edata,
                                void *user_data);

static gptimer_handle_t gptimer = NULL;

static const char TAG[] = "Periodic Alarm Demo";

void app_main(void)
{
    ESP_ERROR_CHECK(s_init());

    vTaskDelay(pdMS_TO_TICKS(30000));

    ESP_ERROR_CHECK(s_release());
}

esp_err_t s_init(void)
{
    esp_err_t ret = NULL;

    const gpio_config_t gpio_handle = {
        .pin_bit_mask   = (1ULL << BLED_GPIO),
        .mode           = GPIO_MODE_OUTPUT,
        .pull_up_en     = DISABLED,
        .pull_down_en   = DISABLED,
        .intr_type      = GPIO_INTR_NEGEDGE,
    };
    ESP_LOGI(TAG, "Configuring LED GPIOs");
    if ((ret = gpio_config(&gpio_config)))
        return ret;

    const gptimer_config_t timer_config = {
        .clk_src        = GPTIMER_CLK_SRC_DEFAULT,
        .direction      = GPTIMER_COUNT_UP,
        .resolution_hz  = 1000000,                  // 1 MHz, tick = 1us
    };
    ESP_LOGI(TAG, "Configuring a new timer");
    if ((ret = gptimer_new_timer(&timer_config, &gptimer)))
        return ret;

    ESP_LOGI(TAG, "Registering timer callback");
    const gptimer_event_callbacks_t timer_callbacks = {
        .on_alarm = s_timer_cb,
    };
    if ((ret = gptimer_register_event_callbacks(gptimer, &timer_callbacks, NULL)))
        return ret;
    
    ESP_LOGI(TAG, "Configuration the alarm");
    gptimer_alarm_config_t alarm_config = {
        .reload_count               = 0,
        .alarm_count                = 100000,   // 0.1s
        .flags.auto_reload_on_alarm = true,     // Will reset the count on alarm
    };
    if ((ret = gptimer_set_alarm_action(gptimer, &alarm_config)))
        return ret;
    
    ESP_LOGI(TAG, "Starting timer with period of %uus", (unsigned) alarm_config.alarm_count);
    ret = gptimer_start(gptimer);

    return ret;
}

esp_err_t s_release(void)
{
    esp_err_t ret = NULL;

    // Stop timer
    ESP_LOGI(TAG, "Stopping GTimer");
    ret = gptimer_stop(gptimer);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "gptimer_stop failed (%s)", esp_err_to_name(ret));
        return ret;
    }

    // Disable timer
    ESP_LOGI(TAG, "Disabling GPTimer");
    ret = gptimer_disable(gptimer);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "gptimer_disable failed (%s)", esp_err_to_name(ret));
        return ret;
    }

    // Delete timer
    ESP_LOGI(TAG, "Deleting GPTimer");
    ret = gptimer_del_timer(gptimer);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "gptimer_del_timer failed (%s)", esp_err_to_name(ret));
        return ret;
    }

    // Reset LED GPIO state
    ESP_LOGI(TAG, "Resetting LED GPIO state");
    ret = gpio_set_level(BLED_GPIO, DISABLED);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "gpio_set_level failed (%s)", esp_err_to_name(ret));
        return ret;
    }

    // Reset GPIO config to default
    ESP_LOGI(TAG, "Resetting GPIO to default");
    ret = gpio_reset_pin(BLED_GPIO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "gpio_reset_pin failed (%s)", esp_err_to_name(ret));
        return ret;
    }

    ret = ESP_OK;
    return ret;
}

bool s_timer_cb(gptimer_handle_t timer,
                const gptimer_alarm_event_data_t *edata,
                void *user_data)
{
    static DRAM_ATTR bool led_state = DISABLED;

    // Toggle LED state
    led_state ^= 1;
    gpio_set_level(BLED_GPIO, led_state);
    return false;
}