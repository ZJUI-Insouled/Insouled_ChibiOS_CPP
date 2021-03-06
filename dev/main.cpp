/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "ch.hpp"
#include "hal.h"

#include "button_monitor.hpp"
#include "led.hpp"
#include "serial_shell.hpp"

using namespace chibios_rt;

/**
 * Blinks two LEDs, a demonstration of how to write a thread in C++.
 */
class BlinkLEDThread : public chibios_rt::BaseStaticThread<128> {
protected:
    void main(void) override {
        setName("blink_led");
        while(!shouldTerminate()) {
            sleep(TIME_MS2I(100));
            if(buttonK0.toggle) {
                LED_D2_ON;
                LED_D3_OFF;
                sleep(TIME_MS2I((1 + buttonK1.counter % 3) * 100));
                LED_D2_OFF;
                LED_D3_ON;
                sleep(TIME_MS2I((1 + buttonK1.counter % 3) * 100));
            } else {
                LED_D2_ON;
                LED_D3_ON;
                sleep(TIME_MS2I((1 + buttonK1.counter % 3) * 100));
                LED_D2_OFF;
                LED_D3_OFF;
                sleep(TIME_MS2I((1 + buttonK1.counter % 3) * 100));
            }
        }
    }
public:
    BlinkLEDThread() {}
};

static BlinkLEDThread leds;

int main(void) {
    halInit();
    System::init();

    // Initialize GPIO to the LEDs.
    palSetPadMode(GPIOA, 6, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(GPIOA, 7, PAL_MODE_OUTPUT_PUSHPULL);

    // Start button monitor threads.
    buttonK0.start(NORMALPRIO);
    buttonK1.start(NORMALPRIO);

    // Start LED blink thread, defined above.
    leds.start(NORMALPRIO - 1);

    // Start ChibiOS shell at high priority,
    // so even if a thread stucks, we still have access to shell.
    serialShell.start(HIGHPRIO);

    // See chconf.h for what this #define means.
    #if CH_CFG_NO_IDLE_THREAD
        // ChibiOS idle thread has been disabled,
        // main() should implement infinite loop
        while(true) {}
    #else
        // When main() quits, the main thread will somehow
        // enter an infinite loop, so we set the priority to lowest
        // before quitting, to let other threads run normally
        BaseThread::setPriority(1);
    #endif
    return 0;
}
