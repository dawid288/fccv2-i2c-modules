#ifndef FCCU_I2C_H
#define FCCU_I2C_H

#include "i2c_config.h"

#define FCCU_I2C_PORT    I2C_PORT
#define FCCU_I2C_SDA_PIN I2C_SDA_PIN
#define FCCU_I2C_SCL_PIN I2C_SCL_PIN

extern i2c_master_bus_handle_t fccu_i2c_mst_handle;

void fccu_i2c_master_init();

#endif