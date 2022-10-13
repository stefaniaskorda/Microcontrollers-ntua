# Microcontrollers-ntua

This project was built at Microchip Studio for the ATmega16 microcontroller and to be compatible with EasyAVR6 development board. This project was for the lesson of Microcontrollers Lab at National Technical University of Athens.

### Generator producing a changing electrical voltage using AtMega16

## PWM
A PWM electrical voltage waveform, with a varying Duty Cycle, can used as a power source for an electrical load, with the aim of regulating it absorbed power. If e.g. the electric charge is a bulb then changingthe Duty Cycle, its brightness can be controlled. A low power varying electrical voltage can be produced if a PWM voltage waveform, with varying Duty Cycle, connected to the input of a deep-pass filter.


## Pulse width modulation with ATmega16
The ATmega16 has three timers/counters that can be used for generating PWM voltage waveforms, with varying Duty Cycle. Of them, Timer/Counter0 and Timer/Counter2 are 8-bit, while Timer/Counter1 can be set as 8-bit or as 16-bit. Additionally Timer/Counter1 has two outputs. The Timer/Counter0 and Timer/Counter1 have synchronous operation while Timer/Counter2 is asynchronous. The three Timer/Counters have a similar PWM mode of operation. One way will be considered here mode by which a high frequency PWM voltage waveform is generated. At this mode (Fast PWM Mode) the TCNTn increases starting from the BOTTOM value and when it reaches the MAX value then it takes the BOTTOM value again and the process repeats.

In the non-inverting mode, when TCNTn, as it increases, takes a value equal to its value register OCRn then the OCn flag is reset and the corresponding interrupt is triggered (if 3 is enabled by the OCIEn bit of the TIMSK register). Then when TCNTn, as it continues to increase, it takes a value equal to the MAX value then the OCn flag is set, TCNTn is set to BOTTOM, the TOVn overflow flag is set and triggered the corresponding interrupt (as long as it is enabled by the TOIEn bit of the TIMSK register). 

In reverse mode exactly the same process is performed as in non-reversal operation with the difference that the OCn flag is set when TCNTn gets a value equal to the value of the OCRn register and reset the OCn flag when TCNTn gets a value equal to the value MAX. The state of the OCn flag can be displayed as a waveform on the corresponding pin of the appropriate I/O port if that pin is initialized as exit.

The output waveform will be pulses with a constant frequency (𝑓𝑃𝑊𝑀) whose value depends on from system clock frequency (𝑓𝑐𝑙𝑘) and prescaler initialization as results from the following formula:
𝑓𝑃𝑊𝑀 = 𝑓𝑐𝑙𝑘 / 𝑁. (1 + 𝑇𝑂𝑃)

The variable N represents the value of the prescaler (1, 8, 64, 256, or 1024). The Duty Cycle is set, via the OCRn register, from a minimum value (almost zero)
to 1. The extreme values ​​for the OCRn register are BOTTOM, where the output will be an extremely small pulse at the start of each period and MAX where the output will be constant high in non-inverting mode or constant low in inverting mode, An example, in the C programming language, of production is then presented of PWM pulses on pin PB3, PD5 and PD7 for the easyAVR6 training system. 

The PWM_init() routine initializes timers TMR0, TMR1A and TMR2 appropriately to they work in Fast PWM Mode, with non-inverting mode and with a value of 8 for the prescaler. It initializes the TMR1A in 8-bit mode. TMR1B is not used. In each of an LED is connected to terminals PB3, PD5 and PD7. The main() routine increments continuously the Duty Cycle of the PWM waveforms correspondingly increasing and decreasing the brightness of LEDs connected to terminals PB3, PD5 and PD7

## Implementation: 

We created a code in C language to initialize Timer/Counter1 to produce a PWM waveform with a frequency of 4KHz. Each time key 1 is pressed it will be incremented by 1 the Duty Cycle of the waveform at pin PB3. Every time it is pressed key 2 it decreases the Duty Cycle of the same waveform by 1. Duty Cycle takes values ​​from 0 to 255. Also the code initializes the built-in ADC converter to read the value of the voltage at pin PA0 and display it on the LCD screen with an accuracy of two decimal places digits, in the format shown in the figure below.
