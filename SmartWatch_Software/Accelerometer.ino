#include <driver/adc.h>

int readXAccel() {
  int read_raw;
  adc2_config_channel_atten((adc2_channel_t) X_ACCEL_ADC_CH, ADC_ATTEN_11db );
  adc2_get_raw((adc2_channel_t) X_ACCEL_ADC_CH, ADC_WIDTH_BIT_12, &read_raw);
  return read_raw;
}

int readYAccel() {
  int read_raw;
  adc2_config_channel_atten((adc2_channel_t) Y_ACCEL_ADC_CH, ADC_ATTEN_11db );
  adc2_get_raw((adc2_channel_t) Y_ACCEL_ADC_CH, ADC_WIDTH_BIT_12, &read_raw);
  return read_raw;
}
int readZAccel() {
  int read_raw;
  adc2_config_channel_atten((adc2_channel_t) Z_ACCEL_ADC_CH, ADC_ATTEN_11db );
  adc2_get_raw((adc2_channel_t) Z_ACCEL_ADC_CH, ADC_WIDTH_BIT_12, &read_raw);
  return read_raw;
}
