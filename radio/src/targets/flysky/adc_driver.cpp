/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "opentx.h"

#if defined(SIMU)
  // not needed
#else
const uint8_t ana_mapping[NUM_ANALOGS] = { 0 /*STICK1*/, 1 /*STICK2*/, 2 /*STICK3*/, 3 /*STICK4*/,
                                       10 /*POT1*/, 4 /*POT2*/, //5 /*POT3*/, 6 /*POT4*/,
                                       9 /*TX_VOLTAGE*/ };
const int8_t ana_direction[NUM_ANALOGS] = {1,-1,1,-1,  1,-1,0};
#endif

#if NUM_PWMANALOGS > 0
  #define FIRST_ANALOG_ADC             (ANALOGS_PWM_ENABLED() ? NUM_PWMANALOGS : 0)
  #define NUM_ANALOGS_ADC              (ANALOGS_PWM_ENABLED() ? (NUM_ANALOGS - NUM_PWMANALOGS) : NUM_ANALOGS)
#elif defined(PCBX9E)
  #define FIRST_ANALOG_ADC             0
  #define NUM_ANALOGS_ADC              10
  #define NUM_ANALOGS_ADC_EXT          (NUM_ANALOGS - 10)
#else
  #define FIRST_ANALOG_ADC             0
  #define NUM_ANALOGS_ADC              NUM_ANALOGS
#endif

uint16_t adcValues[NUM_ANALOGS] __DMA;

void adcInit()
{

}
void adcSingleRead()
{

}
void adcRead()
{
  uint16_t temp[NUM_ANALOGS] = { 0 };

  for (int i=0; i<4; i++) {
    adcSingleRead();
    for (uint8_t x=FIRST_ANALOG_ADC; x<NUM_ANALOGS; x++) {
      uint16_t val = adcValues[x];
#if defined(JITTER_MEASURE)
      if (JITTER_MEASURE_ACTIVE()) {
        rawJitter[x].measure(val);
      }
#endif
      temp[x] += val;
    }
  }

  for (uint8_t x=FIRST_ANALOG_ADC; x<NUM_ANALOGS; x++) {
    adcValues[x] = temp[x] >> 2;
  }

#if NUM_PWMANALOGS > 0
  if (ANALOGS_PWM_ENABLED()) {
    analogPwmRead(adcValues);
  }
#endif
}

// TODO
void adcStop()
{
}

#if !defined(SIMU)
uint16_t getAnalogValue(uint8_t index)
{
  if (IS_POT(index) && !IS_POT_SLIDER_AVAILABLE(index)) {
    // Use fixed analog value for non-existing and/or non-connected pots.
    // Non-connected analog inputs will slightly follow the adjacent connected analog inputs,
    // which produces ghost readings on these inputs.
    return 0;
  }
#if defined(PCBX9E)
  index = ana_mapping[index];
#endif
  if (ana_direction[index] < 0)
    return 4095 - adcValues[index];
  else
    return adcValues[index];
}
#endif // #if !defined(SIMU)