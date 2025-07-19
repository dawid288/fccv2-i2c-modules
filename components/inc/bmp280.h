#ifndef BMP280_H
#define BMP280_H

#include <stdio.h>
#include <stdlib.h>
#include "fccu_i2c.h"
#include "i2c_config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define BMP280_I2C_ADDRESS   0x76
#define BMP280_CHIP_ID_REG   0xD0
#define I_AM_BMP280          0x58
#define BMP280_RESET_REG     0xE0
#define BMP280_DO_SOFT_RESET 0xB6
#define BMP280_CALIB_REG     0x88
#define BMP280_CTRL_MEAS_REG 0xF4
#define BMP280_CONFIG_REG    0xF5
#define BMP280_TEMP_MSB_REG  0xFA
#define BMP280_PRESS_MSB_REG 0xF7

typedef struct
{
    uint16_t dig_t1;
    int16_t dig_t2;
    int16_t dig_t3;
    uint16_t dig_p1;
    int16_t dig_p2;
    int16_t dig_p3;
    int16_t dig_p4;
    int16_t dig_p5;
    int16_t dig_p6;
    int16_t dig_p7;
    int16_t dig_p8;
    int16_t dig_p9;
} bmp280_calib_t;

typedef enum {
    T_SB_0_5_ms = 0x00,
    T_SB_62_5_ms = 0x01,
    T_SB_125_ms = 0x02,
    T_SB_250_ms = 0x03,
    T_SB_500_ms = 0x04,
    T_SB_1000_ms = 0x05,
    T_SB_2000_ms = 0x06,
    T_SB_4000_ms = 0x07,
} bmp280_sb_time_t;

typedef enum {
    FILTER_OFF = 0x00,
    FILTER_2 = 0x01,
    FILTER_4 = 0x02,
    FILTER_8 = 0x03,
    FILTER_16 = 0x04,
} bmp280_filter_t;

typedef struct
{
    bmp280_sb_time_t t_sb : 3;
    bmp280_filter_t filter : 3;
} bmp280_config_t;

typedef struct
{
    uint8_t measuring : 1;
    uint8_t im_update : 1;
} bmp280_status_t;

typedef enum {
    POWER_MODE_SLEEP = 0x00,
    POWER_MODE_FORCED = 0x01,
    POWER_MODE_NORMAL = 0x03,
} bmp280_pwr_mode_t;

typedef enum {
    OSRS_x0 = 0x00,
    OSRS_x1 = 0x01,
    OSRS_x2 = 0x02,
    OSRS_x4 = 0x03,
    OSRS_x8 = 0x04,
    OSRS_x16 = 0x05,
} bmp280_osrs_t;

typedef struct
{
    bmp280_osrs_t osrs_tmp : 3;
    bmp280_osrs_t osrs_press : 3;
    bmp280_pwr_mode_t pwr_mode : 2;
} bmp280_ctrl_meas_t;

typedef struct
{
    float pressure;
    float temperature;
} bmp280_data_t;

typedef struct bmp280
{
    uint16_t i2c_addr;
    i2c_master_dev_handle_t i2c_dev_handle;
    i2c_master_bus_handle_t i2c_mst_handle;
    bmp280_calib_t calib;
    bmp280_config_t config;
    bmp280_ctrl_meas_t ctrl_meas;
} bmp280_type_t;

void bmp280_init(bmp280_type_t* bmp280_dev);
void bmp280_write_reg(bmp280_type_t* bmp280_dev, uint8_t reg, uint8_t* data, size_t data_len);
void bmp280_read_reg(bmp280_type_t* bmp280_dev, uint8_t reg, uint8_t* data, size_t data_len);
void bmp280_soft_reset(bmp280_type_t* bmp280_dev);
void bmp280_calibrate(bmp280_type_t* bmp280_dev);
void bmp280_read_pressure_temperature(bmp280_type_t* bmp280_dev, bmp280_data_t* data);

#endif