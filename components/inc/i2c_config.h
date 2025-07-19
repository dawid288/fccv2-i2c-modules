#ifndef I2C_CONFIG_H
#define I2C_CONFIG_H

#include <driver/i2c_master.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define I2C_PORT    I2C_NUM_0
#define I2C_SDA_PIN GPIO_NUM_35
#define I2C_SCL_PIN GPIO_NUM_34

void i2c_master_config(i2c_master_bus_handle_t* mst_bus_handle, i2c_port_num_t port, gpio_num_t sda, gpio_num_t scl);
void i2c_device_config(i2c_master_bus_handle_t mst_bus_handle, i2c_master_dev_handle_t* dev_handle, uint16_t dev_addr);
void i2c_read(i2c_master_dev_handle_t dev, uint8_t reg, uint8_t* data, size_t data_len);
void i2c_write(i2c_master_dev_handle_t dev, uint8_t reg, uint8_t* data, size_t data_len);

#endif