#include "fccu_i2c.h"

i2c_master_bus_handle_t fccu_i2c_mst_handle;

void fccu_i2c_master_init()
{
    i2c_master_config(&fccu_i2c_mst_handle, FCCU_I2C_PORT, FCCU_I2C_SDA_PIN, FCCU_I2C_SCL_PIN);
}