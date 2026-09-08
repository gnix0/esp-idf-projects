#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/i2c.h>
#include <esp_err.h>
#include <esp_log.h>

#define I2C_MASTER_SCL_IO           GPIO_NUM_0  // GPIO number used for I2C master clock
#define I2C_MASTER_SDA_IO           GPIO_NUM_1  // GPIO number used for I2C master data
#define I2C_MASTER_NUM              0           // I2C master i2c port number
#define I2C_MASTER_FREQ_HZ          400000      // I2C master clock frequency
#define I2C_MASTER_TX_BUF_DISABLE   0           // I2C master doesn't need buffer
#define I2C_MASTER_RX_BUF_DISABLE   0           // I2C master doesn't need buffer
#define ACK_CHECK_EN                0x1         // I2C master will check ack from slave
#define ACK_CHECK_DIS               0x0         // I2C master will not check ack from slave
#define ACK_VAL                     0x0         // I2C ack value
#define NACK_VAL                    0x1         // I2C nack value

static const char *TAG = "I2C Scanner Demo";

void app_main(void)
{
    ESP_LOGI(TAG, "Starting I2C scanning demo...");

    i2c_config_t i2c_cfg = {
        .mode               = I2C_MODE_MASTER,
        .sda_io_num         = I2C_MASTER_SDA_IO,
        .sda_pullup_en      = GPIO_PULLUP_ENABLE,
        .scl_io_num         = I2C_MASTER_SCL_IO,
        .scl_pullup_en      = GPIO_PULLUP_ENABLE,
        .master.clk_speed   = I2C_MASTER_FREQ_HZ,
    };
    i2c_param_config(I2C_MASTER_NUM, &i2c_cfg);
    i2c_driver_install(I2C_MASTER_NUM, i2c_cfg.mode, I2C_MASTER_RX_BUF_DISABLE,
                        I2C_MASTER_TX_BUF_DISABLE, 0);
    
    ESP_LOGI(TAG, "I2C started");

    ESP_LOGI(TAG, "Scanning I2C addresses...");
    for (uint8_t i = 0; i < 128; i++) {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();                                   // Create a command link
        i2c_master_start(cmd);                                                          // Start the command link
        i2c_master_write_byte(cmd, (i << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);          // Write the address to the slave
        i2c_master_stop(cmd);                                                           // Stop the command link

        esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(10));   // Send the command link
        if (ret == ESP_OK)
            ESP_LOGI(TAG, "Found=0x%x", i);
        
        i2c_cmd_link_delete(cmd);
    }

    ESP_LOGI(TAG, "Scanner finished");
}
