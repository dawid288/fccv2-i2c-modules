#include "bmp280.h"

static uint8_t bmp280_get_id(bmp280_type_t* bmp280_dev)
{
    uint8_t id;
    bmp280_read_reg(bmp280_dev, BMP280_CHIP_ID_REG, &id, 1);

    return id;
}

void bmp280_init(bmp280_type_t* bmp280_dev)
{
    bmp280_dev->i2c_mst_handle = fccu_i2c_mst_handle;
    bmp280_dev->i2c_addr = BMP280_I2C_ADDRESS;
    i2c_device_config(bmp280_dev->i2c_mst_handle, &bmp280_dev->i2c_dev_handle, bmp280_dev->i2c_addr);

    if (bmp280_get_id(bmp280_dev) == I_AM_BMP280)
        printf("found bmp280 device\r\n");
    else
        printf("Error, bmp280 not found\r\n");

    bmp280_soft_reset(bmp280_dev);
    bmp280_calibrate(bmp280_dev);

    bmp280_dev->ctrl_meas.osrs_press = OSRS_x4;
    bmp280_dev->ctrl_meas.osrs_tmp = OSRS_x1;
    bmp280_dev->ctrl_meas.pwr_mode = POWER_MODE_NORMAL;

    uint8_t ctrl_meas = (bmp280_dev->ctrl_meas.osrs_tmp << 5) | (bmp280_dev->ctrl_meas.osrs_press << 2)
                        | bmp280_dev->ctrl_meas.pwr_mode;

    bmp280_write_reg(bmp280_dev, BMP280_CTRL_MEAS_REG, &ctrl_meas, sizeof(ctrl_meas));

    bmp280_dev->config.t_sb = T_SB_500_ms;
    bmp280_dev->config.filter = FILTER_16;

    uint8_t config = (bmp280_dev->config.t_sb << 5) | (bmp280_dev->config.filter << 2);

    bmp280_write_reg(bmp280_dev, BMP280_CONFIG_REG, &config, sizeof(config));
}

void bmp280_write_reg(bmp280_type_t* bmp280_dev, uint8_t reg, uint8_t* data, size_t data_len)
{
    i2c_write(bmp280_dev->i2c_dev_handle, reg, data, data_len);
}

void bmp280_read_reg(bmp280_type_t* bmp280_dev, uint8_t reg, uint8_t* data, size_t data_len)
{
    i2c_read(bmp280_dev->i2c_dev_handle, reg, data, data_len);
}

void bmp280_soft_reset(bmp280_type_t* bmp280_dev)
{
    uint8_t reset = BMP280_DO_SOFT_RESET;
    bmp280_write_reg(bmp280_dev, BMP280_RESET_REG, &reset, 1);
    vTaskDelay(pdMS_TO_TICKS(10));
}

void bmp280_calibrate(bmp280_type_t* bmp280_dev)
{
    uint8_t data[24];
    bmp280_read_reg(bmp280_dev, BMP280_CALIB_REG, data, 24);

    bmp280_dev->calib.dig_t1 = (data[1] << 8) | data[0];
    bmp280_dev->calib.dig_t2 = (data[3] << 8) | data[2];
    bmp280_dev->calib.dig_t3 = (data[5] << 8) | data[4];
    bmp280_dev->calib.dig_p1 = (data[7] << 8) | data[6];
    bmp280_dev->calib.dig_p2 = (data[9] << 8) | data[8];
    bmp280_dev->calib.dig_p3 = (data[11] << 8) | data[10];
    bmp280_dev->calib.dig_p4 = (data[13] << 8) | data[12];
    bmp280_dev->calib.dig_p5 = (data[15] << 8) | data[14];
    bmp280_dev->calib.dig_p6 = (data[17] << 8) | data[16];
    bmp280_dev->calib.dig_p7 = (data[19] << 8) | data[18];
    bmp280_dev->calib.dig_p8 = (data[21] << 8) | data[20];
    bmp280_dev->calib.dig_p9 = (data[23] << 8) | data[22];
}

static int32_t bmp280_read_pressure_raw(bmp280_type_t* bmp280_dev)
{
    uint8_t press_data[3];
    bmp280_read_reg(bmp280_dev, BMP280_PRESS_MSB_REG, press_data, sizeof(press_data));
    return (press_data[0] << 12) | (press_data[1] << 4) | (press_data[2] >> 4);
}

static int32_t bmp280_read_temperature_raw(bmp280_type_t* bmp280_dev)
{
    uint8_t temp_data[3];
    bmp280_read_reg(bmp280_dev, BMP280_TEMP_MSB_REG, temp_data, sizeof(temp_data));
    return (temp_data[0] << 12) | (temp_data[1] << 4) | (temp_data[2] >> 4);
}

void bmp280_read_pressure_temperature(bmp280_type_t* bmp280_dev, bmp280_data_t* data)
{
    int32_t t_fine, press_raw, temp_raw;
    press_raw = bmp280_read_pressure_raw(bmp280_dev);
    temp_raw = bmp280_read_temperature_raw(bmp280_dev);

    int32_t var1_t, var2_t, t;
    int64_t var1_p, var2_p, p;
    var1_t = ((((temp_raw >> 3) - ((int32_t) bmp280_dev->calib.dig_t1 << 1))) * ((int32_t) bmp280_dev->calib.dig_t2))
             >> 11;
    var2_t = (((((temp_raw >> 4) - ((int32_t) bmp280_dev->calib.dig_t1))
                   * ((temp_raw >> 4) - ((int32_t) bmp280_dev->calib.dig_t1)))
                  >> 12)
                 * ((int32_t) bmp280_dev->calib.dig_t3))
             >> 14;
    t_fine = var1_t + var2_t;
    t = (t_fine * 5 + 128) >> 8;
    data->temperature = t / 100.0f;

    var1_p = ((int64_t) t_fine) - 128000;
    var2_p = var1_p * var1_p * (int64_t) bmp280_dev->calib.dig_p6;
    var2_p = var2_p + ((var1_p * (int64_t) bmp280_dev->calib.dig_p5) << 17);
    var2_p = var2_p + (((int64_t) bmp280_dev->calib.dig_p4) << 35);
    var1_p = ((var1_p * var1_p * (int64_t) bmp280_dev->calib.dig_p3) >> 8)
             + ((var1_p * (int64_t) bmp280_dev->calib.dig_p2) << 12);
    var1_p = (((((int64_t) 1) << 47) + var1_p)) * ((int64_t) bmp280_dev->calib.dig_p1) >> 33;

    if (var1_p == 0)
        return; // avoid exception caused by division by zero

    p = 1048576 - press_raw;
    p = (((p << 31) - var2_p) * 3125) / var1_p;
    var1_p = (((int64_t) bmp280_dev->calib.dig_p9) * (p >> 13) * (p >> 13)) >> 25;
    var2_p = (((int64_t) bmp280_dev->calib.dig_p8) * p) >> 19;
    p = ((p + var1_p + var2_p) >> 8) + (((int64_t) bmp280_dev->calib.dig_p7) << 4);
    data->pressure = ((uint32_t) p) / 25600.0f;
}