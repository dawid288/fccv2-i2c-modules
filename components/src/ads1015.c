#include "ads1015.h"

void ads1015_init(ads1015_type_t* ads1015_dev)
{
    ads1015_dev->i2c_mst_handle = fccu_i2c_mst_handle;
    ads1015_dev->i2c_addr = ADS1015_I2C_ADDRESS;
    i2c_device_config(ads1015_dev->i2c_mst_handle, ads1015_dev->i2c_dev_handle, ads1015_dev->i2c_addr);

    ads1015_dev->config.mux_config = AIN0;
    ads1015_dev->config.gain = FSR_4_096V;
    ads1015_dev->config.device_mode = SINGLE_SHOT;
    ads1015_dev->config.data_rate = SPS_1600;
    ads1015_dev->comparator.comp_queue = DISABLE;

    uint8_t config_reg[2];
    config_reg[0]
        = (ads1015_dev->config.mux_config << 4) | (ads1015_dev->config.gain << 1) | ads1015_dev->config.device_mode;
    config_reg[1] = (ads1015_dev->config.data_rate << 5) | (ads1015_dev->comparator.comp_queue);

    ads1015_write(ads1015_dev, CONFIG_REGISTER, config_reg);
}

void ads1015_write(ads1015_type_t* ads1015_dev, uint8_t reg, uint8_t data)
{
    i2c_write(ads1015_dev->i2c_dev_handle, reg, data);
}

void ads1015_read(ads1015_type_t* ads1015_dev, uint8_t reg, uint8_t* data, size_t data_len)
{
    i2c_read(ads1015_dev->i2c_dev_handle, reg, data, data_len);
}

int16_t ads1015_read_channel_single_ended(ads1015_type_t* ads1015_dev, ads1015_adc_channel_t channel)
{
    if (channel > 3)
        channel = 3;

    uint8_t adc_single_ended[] = {
        AIN0,
        AIN1,
        AIN2,
        AIN3,
    };

    uint8_t config_reg[2];

    ads1015_dev->config.mux_config = adc_single_ended[channel];

    ads1015_read(ads1015_dev, CONFIG_REGISTER, config_reg, sizeof(config_reg));
    config_reg[0] &= 0x0F;
    config_reg[0] |= (ads1015_dev->config.mux_config << 4) | (START_SINGLE_CONV << 7);

    ads1015_write(ads1015_dev, CONFIG_REGISTER, config_reg);

    do
    {
        ads1015_read(ads1015_dev, CONFIG_REGISTER, config_reg, sizeof(config_reg));
    }
    while ((config_reg[0] & 0x80) == CONV_IN_PROGRESS);

    uint8_t adc_data[2];

    ads1015_read(ads1015_dev, CONVERSION_REGISTER, adc_data, sizeof(adc_data));

    int16_t result = ((adc_data[0] << 8) | adc_data[1]) >> 4;

    if (result & 0x0800)
    {
        result |= 0xF000;
    }
    return result;
}
