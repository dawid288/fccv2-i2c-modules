#include "i2c_config.h"

void i2c_master_config(i2c_master_bus_handle_t mst_bus_handle, i2c_port_num_t port, gpio_num_t sda, gpio_num_t scl)
{
    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = port,
        .scl_io_num = scl,
        .sda_io_num = sda,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &mst_bus_handle));
}

void i2c_device_config(i2c_master_bus_handle_t mst_bus_handle, i2c_master_dev_handle_t dev_handle, uint16_t dev_addr)
{
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = dev_addr,
        .scl_speed_hz = 100000,
    };

    ESP_ERROR_CHECK(i2c_master_bus_add_device(mst_bus_handle, &dev_cfg, &dev_handle));
}

void i2c_read(i2c_master_dev_handle_t dev, uint8_t reg, uint8_t* data, size_t data_len)
{
    ESP_ERROR_CHECK(i2c_master_transmit_receive(dev, &reg, 1, data, data_len, -1));
}

void i2c_write(i2c_master_dev_handle_t dev, uint8_t reg, uint8_t data)
{
    uint8_t data_wr[2];
    data_wr[0] = reg;
    data_wr[1] = data;
    ESP_ERROR_CHECK(i2c_master_transmit(dev, data_wr, 2, -1));
}
