#ifndef DRIVER_ADC_H
#define DRIVER_ADC_H

#include <stdint.h>

/* Measured VDDA and the input voltage corresponding to 100%, in millivolts. */
#define DRIVER_ADC_REFERENCE_MV 3241U
#define DRIVER_ADC_INPUT_MAX_MV 3241U
#define DRIVER_ADC_FULL_SCALE   4095U

void Driver_ADC_Init(void);
uint16_t Driver_ADC_ReadAverage(void);
uint32_t Driver_ADC_ToMillivolts(uint16_t raw);
uint8_t Driver_ADC_ToPercentage(uint16_t raw);

#endif /* DRIVER_ADC_H */
