/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_HARDWARE_TEST_FIXTURE_
#define _H_COM_DIAG_DIMINUTO_HARDWARE_TEST_FIXTURE_

/**
 * @file
 *
 * Copyright 2018-2020 Digital Aggregates Corporation, Colorado USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * Defines the pinins and pinouts of the hardware test fixture
 * on which I run the functional tests. The HTF is a breadboard
 * I purpose built so I could test Diminuto features like Pin
 * (GPIO), Modulator (PWM), and Controller (PID), that really
 * reauire actual hardware for verification.
 *
 * SEE ALSO: hardware_test_fixture.sh
 */

#include "avago/apds9301.h"
#include "ti/ads1115.h"

enum HardwareTestFixture {
    HARDWARE_TEST_FIXTURE_BUS_I2C           = 1, /* Specific to Raspberry Pi. */
    HARDWARE_TEST_FIXTURE_DEV_I2C_LUX       = AVAGO_APDS9301_ADDRESS_FLOAT,
    HARDWARE_TEST_FIXTURE_DEV_I2C_ADC       = TI_ADS1115_ADDRESS_LOW,
    HARDWARE_TEST_FIXTURE_PIN_LED_1         = 16,
    HARDWARE_TEST_FIXTURE_PIN_PPS           = 18,
    HARDWARE_TEST_FIXTURE_PIN_LED_2         = 20,
    HARDWARE_TEST_FIXTURE_PIN_LED_3         = 21,
    HARDWARE_TEST_FIXTURE_PIN_BUT_LOW       = 22, /* Active low, pulled high. */
    HARDWARE_TEST_FIXTURE_PIN_BUT_HIGH      = 27, /* Active high. */
    HARDWARE_TEST_FIXTURE_PIN_PWM_LED       = 12,
    HARDWARE_TEST_FIXTURE_PIN_PWM_ADC       = 13,
    HARDWARE_TEST_FIXTURE_PIN_INT_LUX       = 26, 
    HARDWARE_TEST_FIXTURE_PIN_INT_ADC       = 19, 
};

#endif
