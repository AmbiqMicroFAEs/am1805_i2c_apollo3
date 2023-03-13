"# am1805_i2c_apollo3" 

Project description: The basic functionality of the AM1805 is presented in this example. The RTC enters sleep mode and leaves it periodically every 3 seconds using the external interrupt EXTI input. The extern XTAL is used as the main oscillator. Current time is printed over the UART. The IOM/DMA is used to communicate with the RTC. The project can be compiled using the free Keil MDK Community Edition: https://www2.keil.com/mdk5/editions/community

Board configuration (AMA3B1KK-KBR + Microbus Development Shield + AM1805RTCEVB):

![board](https://user-images.githubusercontent.com/69169627/224769968-34f13347-1cbc-4ce1-8395-9cd324e5b1aa.jpg)

Project folders:


![project](https://user-images.githubusercontent.com/69169627/224771774-62866f46-3eaa-4216-adb9-04cf23d1a782.png)

Average current consumption o of the AM1805 = 70 nA /3.3V (includes I2C transaction time):

![current](https://user-images.githubusercontent.com/69169627/224772048-7f530fa7-d2fe-4f05-822b-4c608ae81526.jpg)

