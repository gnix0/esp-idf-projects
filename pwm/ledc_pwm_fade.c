#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"

#include "driver/ledc.h"
#include "hal/ledc_types.h"
#include "soc/clk_tree_defs.h"
#include "soc/gpio_num.h"

#include <stdint.h>

void app_main(void)
{
    ledc_timer_config_t timer_config = {
        .speed_mode      = LEDC_LOW_SPEED_MODE,
        .timer_num       = LEDC_TIMER_0,
        .freq_hz         = 5000,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .clk_cfg         = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer_config);

    ledc_channel_config_t channel_config = {
        .channel    = LEDC_CHANNEL_0,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_sel  = LEDC_TIMER_0,
        .intr_type  = LEDC_INTR_DISABLE,
        .gpio_num   = GPIO_NUM_5,
        .duty       = 0
    };
    ledc_channel_config(&channel_config);

    ledc_fade_func_install(0);

    for (;;) {
        ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 1023, 2000, LEDC_FADE_WAIT_DONE);
        vTaskDelay(pdMS_TO_TICKS(20));
        ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0, 2000, LEDC_FADE_WAIT_DONE);
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
