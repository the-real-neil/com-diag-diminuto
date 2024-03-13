#!/bin/bash
# Copyright 2018-2020 Digital Aggregates Corporation, Colorado USA<BR>
# Licensed under the terms in LICENSE.txt<BR>
# Chip Overclock <coverclock@diag.com><BR>
# https://github.com/coverclock/com-diag-diminuto<BR>
#
# ABSTRACT
# 
# Defines the pinins and pinouts of the hardware test fixture
# on which I run the functional tests. This is specific to the
# Raspberru Pi.
#
# USAGE
#
# . hardware_test_fixture.sh
#
# SEE ALSO
#
# hardware_test_fixture.h
#

HARDWARE_TEST_FIXTURE_DEVICE="/dev/gpiochip4"

HARDWARE_TEST_FIXTURE_BUS_I2C=1
HARDWARE_TEST_FIXTURE_DEV_I2C_LUX=0x39
HARDWARE_TEST_FIXTURE_DEV_I2C_ADC=0x48
HARDWARE_TEST_FIXTURE_PIN_LED_1=16
HARDWARE_TEST_FIXTURE_PIN_PPS=18
HARDWARE_TEST_FIXTURE_PIN_LED_2=20
HARDWARE_TEST_FIXTURE_PIN_LED_3=21
HARDWARE_TEST_FIXTURE_PIN_BUT_LOW=22
HARDWARE_TEST_FIXTURE_PIN_BUT_HIGH=27
HARDWARE_TEST_FIXTURE_PIN_PWM_LED=12
HARDWARE_TEST_FIXTURE_PIN_PWM_ADC=13
HARDWARE_TEST_FIXTURE_PIN_INT_LUX=26
HARDWARE_TEST_FIXTURE_PIN_INT_ADC=19
HARDWARE_TEST_FIXTURE_DEBOUNCE_CUE_USEC=10000
HARDWARE_TEST_FIXTURE_DEBOUNCE_DEV_USEC=10000
HARDWARE_TEST_FIXTURE_DEBOUNCE_MUX_USEC=1000000
