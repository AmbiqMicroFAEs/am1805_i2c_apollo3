"# am1805_i2c_apollo3" 

Project description: The basic functionality of the AM1805 is presented in this example. The RTC enters sleep mode and leaves it periodically every 3 seconds using the external interrupt EXTI input. The extern XTAL is used as the main oscillator. Current time is printed over the UART. The IOM/DMA is used to communicate with the RTC. The project can be compiled using the free Keil MDK Community Edition: https://www2.keil.com/mdk5/editions/community
