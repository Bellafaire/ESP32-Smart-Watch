/*****************************************************************************
The MIT License (MIT)

Copyright (c) 2020 Matthew James Bellafaire

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/
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
