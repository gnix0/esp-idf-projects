#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "driver/gptimer.h"

#include "esp_err.h"
#include "esp_log.h"

#define BLED_GPIO GPIO_NUM_4

#define TIMER_RES   1000000
#define ALARM_MAX   100000
#define ALARM_MIN   1000
#define ALARM_STEP  1000

#define ELABLED     1
#define DISABLED    0

static gptimer_handle_t gptimer = NULL;

static esp_err_t s_init(void *user_data);
static esp_err_t s_release(void);
static bool IRAM_ATTR s_timer_cb(gptimer_handle_t timer,
                                const gptimer_alarm_event_data_t *edata,
                                void *user_data);

static const char TAG[] = "Dynamic Alarm Demo";

typedef struct {
    uint64_t delta_t;
    TaskHandle_t user_task;
} UserData;

void app_main(void)
{
	UserData user_data = {
		.delta_t = ALARM_MAX,
		.user_task = xTaskGetCurrentTaskHandle()
	};

	bool decrementing = true;

	//Initilize timer and LED gpios
	ESP_ERROR_CHECK(s_init(&user_data));

	//Change alarm period on received notification
	while (1) {
		if (ulTaskNotifyTake(pdFALSE, portMAX_DELAY)) {
			if (decrementing) {
				user_data.delta_t -= ALARM_STEP;
				if (user_data.delta_t <= ALARM_MIN)
					decrementing = false;
			}
			else {
				user_data.delta_t += ALARM_STEP;
				if (user_data.delta_t >= ALARM_MAX)
					decrementing = true;
			}
		}
	}

	//Release timer and clear LED gpios
	ESP_ERROR_CHECK(s_release());
}

esp_err_t s_init(void *user_data)
{
    esp_err_t ret = NULL;

    const gpio_config_t gpio_handle = {
        .pin_bit_mask   = (1ULL << BLED_GPIO),
        .mode           = GPIO_MODE_OUTPUT,
        .pull_up_en     = DISABLED,
        .pull_down_en   = DISABLED,
        .intr_type      = GPIO_INTR_DISABLE,
    };
    ESP_LOGI(TAG, "Configuring LED GPIO");
    if ((ret = gpio_config(&gpio_handle)))
        return ret;

    // Configure GPTimer
    const gptimer_config_t timer_cfg = {
        .clk_src        = GPTIMER_CLK_SRC_DEFAULT,
        .direction      = GPTIMER_COUNT_UP,
        .resolution_hz  = TIMER_RES,
    };
    ESP_LOGI(TAG, "Configuring GPTimer");
    if ((ret = gptimer_new_timer(&timer_cfg, &gptimer)))
        return ret;
    
    // Register callback
    ESP_LOGI(TAG, "Registering timer callback");
    const gptimer_event_callbacks_t timer_callbacks = {
        .on_alarm = s_timer_cb,
    };
    if ((ret = gptimer_register_event_callbacks(gptimer, &timer_callbacks, user_data)))
        return ret;

    // Enable timer
    ESP_LOGI(TAG, "Enabling timer");
    if ((ret = gptimer_enable(gptimer)))
        return ret;                     // Timer has not yet started
    
    // Configure alarm
    ESP_LOGI(TAG, "Configuring alarm");
    gptimer_alarm_config_t alarm_cfg = {
        .reload_count               = 0,
        .alarm_count                = ((UserData *) user_data)->delta_t,
        .flags.auto_reload_on_alarm = DISABLED, // Dynamic timer, no need for reload
    };
    if ((ret = gptimer_set_alarm_action(gptimer, &alarm_cfg)))
        return ret;

    ESP_LOGI(TAG, "Starting timer with a period of %uus", (unsigned) alarm_cfg.alarm_count);
    ret = gptimer_start(gptimer);

    return ret;
}

esp_err_t s_release(void)
{
    esp_err_t ret = NULL;

    // Stop GPTimer
    ESP_LOGI(TAG, "Stopping timer");
    ret = gptimer_stop(gptimer);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "gptimer_stop failed (%s)", esp_err_to_name(ret));
        return ret;
    }

    // Disable GPTimer
    ESP_LOGI(TAG, "Disabling timer");
    ret = gptimer_disable(gptimer);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "gptimer_disable failed (%s)", esp_err_to_name(ret));
        return ret;
    }

    // Delete GPTimer
    ESP_LOGI(TAG, "Deleting timer");
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

    // Reset GPIO to default state
    ESP_LOGI(TAG, "Resetting GPIO to default");
    ret = gpio_reset_pin(BLED_GPIO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "gpio_reset_pin failed (%s)", esp_err_to_name(ret));
        return ret;
    }

    return ESP_OK;
}

bool s_timer_cb(gptimer_handle_t timer,
			    const gptimer_alarm_event_data_t *edata,
			    void *user_data)
{
	BaseType_t task_woken = pdFALSE;
	static DRAM_ATTR bool led_state = DISABLED;

	//Toggle LED state
	led_state ^= 1;
	gpio_set_level(BLED_GPIO, led_state);

	//Configure next alarm
	gptimer_alarm_config_t alarm_config = {
		// Set the next alarm count to the current alarm value plus delta_t
		// This will effectively change the alarm period dynamically
		// based on the user_data provided
		.alarm_count = edata->alarm_value + ((UserData *) user_data)->delta_t
	};
	gptimer_set_alarm_action(timer, &alarm_config);
	
	//Notify the user task to change the alarm period
	vTaskNotifyGiveFromISR(((UserData *) user_data)->user_task, &task_woken);
	return task_woken;
}