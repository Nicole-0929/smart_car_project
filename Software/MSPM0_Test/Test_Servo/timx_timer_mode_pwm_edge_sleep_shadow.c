/*
 * Copyright (c) 2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ti_msp_dl_config.h"

#define SERVO_PERIOD_US             (20000U)
#define SERVO_MIN_PULSE_US          (1000U)
#define SERVO_CENTER_PULSE_US       (1500U)
#define SERVO_MAX_PULSE_US          (2000U)
#define SERVO_SWEEP_STEP_US         (20U)
#define SERVO_STEP_DELAY_CYCLES     (CPUCLK_FREQ / 20U)
#define SERVO_ENDPOINT_DELAY_CYCLES (CPUCLK_FREQ)

static void Servo_setPulseUs(uint32_t pulseUs)
{
    if (pulseUs < SERVO_MIN_PULSE_US) {
        pulseUs = SERVO_MIN_PULSE_US;
    } else if (pulseUs > SERVO_MAX_PULSE_US) {
        pulseUs = SERVO_MAX_PULSE_US;
    }

    /*
     * This TI timer setup outputs a high pulse of (period - compare) ticks.
     * With a 1 MHz timer clock, ticks are microseconds.
     */
    DL_TimerA_setCaptureCompareValue(
        PWM_0_INST, SERVO_PERIOD_US - pulseUs, DL_TIMER_CC_0_INDEX);
}

static void Servo_delay(uint32_t cycles)
{
    delay_cycles(cycles);
}

int main(void)
{
    uint32_t pulseUs;

    SYSCFG_DL_init();

    DL_TimerA_startCounter(PWM_0_INST);

    DL_GPIO_clearPins(
        GPIO_LEDS_PORT, GPIO_LEDS_USER_LED_1_PIN | GPIO_LEDS_USER_TEST_PIN);

    while (1) {
        Servo_setPulseUs(SERVO_CENTER_PULSE_US);
        Servo_delay(SERVO_ENDPOINT_DELAY_CYCLES);

        for (pulseUs = SERVO_CENTER_PULSE_US; pulseUs >= SERVO_MIN_PULSE_US;
             pulseUs -= SERVO_SWEEP_STEP_US) {
            Servo_setPulseUs(pulseUs);
            DL_GPIO_togglePins(GPIO_LEDS_PORT, GPIO_LEDS_USER_TEST_PIN);
            Servo_delay(SERVO_STEP_DELAY_CYCLES);
        }
        Servo_delay(SERVO_ENDPOINT_DELAY_CYCLES);

        for (pulseUs = SERVO_MIN_PULSE_US; pulseUs <= SERVO_MAX_PULSE_US;
             pulseUs += SERVO_SWEEP_STEP_US) {
            Servo_setPulseUs(pulseUs);
            DL_GPIO_togglePins(GPIO_LEDS_PORT, GPIO_LEDS_USER_TEST_PIN);
            Servo_delay(SERVO_STEP_DELAY_CYCLES);
        }
        Servo_delay(SERVO_ENDPOINT_DELAY_CYCLES);

        for (pulseUs = SERVO_MAX_PULSE_US; pulseUs >= SERVO_CENTER_PULSE_US;
             pulseUs -= SERVO_SWEEP_STEP_US) {
            Servo_setPulseUs(pulseUs);
            DL_GPIO_togglePins(GPIO_LEDS_PORT, GPIO_LEDS_USER_TEST_PIN);
            Servo_delay(SERVO_STEP_DELAY_CYCLES);
        }
    }
}
