#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "bmp280.h"
#include "ads1015.h"

bmp280_type_t bmp280_sensor;
bmp280_data_t bmp280_data;
ads1015_type_t ads1015_converter;
float a0, a1, a2, a3;

void app_main(void)
{
    fccu_i2c_master_init();
    bmp280_init(&bmp280_sensor);
    ads1015_init(&ads1015_converter);

    while (1)
    {
        bmp280_read_pressure_temperature(&bmp280_sensor, &bmp280_data);
        printf("T = %.2f*C, P = %.2f hPa\r\n", bmp280_data.temperature, bmp280_data.pressure);
        vTaskDelay(pdMS_TO_TICKS(1000));
        a0 = ads1015_read_channel_single_shot(&ads1015_converter, 0);
        vTaskDelay(pdMS_TO_TICKS(20));
        a1 = ads1015_read_channel_single_shot(&ads1015_converter, 1);
        vTaskDelay(pdMS_TO_TICKS(20));
        a2 = ads1015_read_channel_single_shot(&ads1015_converter, 2);
        vTaskDelay(pdMS_TO_TICKS(20));
        a3 = ads1015_read_channel_single_shot(&ads1015_converter, 3);
        printf("A0 = %.2f V, A1 = %.2f V, A2 = %.2f V, A3 = %.2f V\r\n", a0, a1, a2, a3);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
