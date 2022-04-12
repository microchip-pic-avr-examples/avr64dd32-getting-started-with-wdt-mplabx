/*
    (c) 2022 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#define F_CPU                  4000000UL
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#define BUTTON_PRESSED         !(PORTF.IN & (PIN6_bm))
#define LED_ON()               PORTF.OUTCLR = PIN5_bm
#define LED_OFF()              PORTF.OUTSET = PIN5_bm
#define LONG_DELAY             1000
#define SMALL_DELAY            200       

/* Default fuses configuration:
- BOD disabled
- Oscillator in High-Frequency Mode
- UPDI pin active(WARNING: DO NOT CHANGE!)
- RESET pin used as GPIO
- CRC disabled
- MVIO enabled for dual supply

Specific fuse configuration for this example:
- Watchdog Timer Enabled: set Period of 4 seconds, WDT in Normal Mode (Window OFF) 
*/
FUSES =
{
.BODCFG = ACTIVE_DISABLE_gc | LVL_BODLEVEL0_gc | SAMPFREQ_128Hz_gc | SLEEP_DISABLE_gc,
.BOOTSIZE = 0x0,
.CODESIZE = 0x0,
.OSCCFG = CLKSEL_OSCHF_gc,
.SYSCFG0 = CRCSEL_CRC16_gc | CRCSRC_NOCRC_gc | RSTPINCFG_GPIO_gc | UPDIPINCFG_UPDI_gc,
.SYSCFG1 = MVSYSCFG_DUAL_gc | SUT_0MS_gc,
.WDTCFG  = PERIOD_4KCLK_gc | WINDOW_OFF_gc, 
};

void My_Application(void)
{
    /* Simulate the user application doing something. */
    _delay_ms(LONG_DELAY);

    while(BUTTON_PRESSED)
    {
        /*
         * The purpose of this is to make the program stop here, simulating a 
         * malfunction, that will cause it to miss th reset the WDT in time. 
         * The WDT will in turn reset the microcontroller, this is noticeable
         * by the LED blinking three time.
         */
        ;
    }
}

void LED_Blink(uint8_t counter)
{
    while(counter--)
    {
        LED_ON();
        _delay_ms(SMALL_DELAY);
        
        LED_OFF();
        _delay_ms(SMALL_DELAY); 
    }
}

int main(void)
{
    /* Configure the PF6 pin (button) as input. */
    PORTF.DIRCLR = PIN6_bm;
    /* Enable the internal pull-up for the PF6 pin (button). */
    PORTF.PIN6CTRL |= PORT_PULLUPEN_bm;
    
    /* Configure the PF5 pin (LED) as output. */
    PORTF.DIRSET = PIN5_bm;

    /* Start-up sequence. */
    LED_Blink(3);

    while (1)
    {
        /* Simulate an user application. */
        My_Application();
        
        /* Periodic LED blinking at ~1s shows normal functioning of the application. */
        LED_Blink(1);
        
        /* Clear WDT periodically.
         * Failing to this in time, will cause it to reset the microcontroller.
         */
        wdt_reset();
    }
}
