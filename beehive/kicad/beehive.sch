EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Connector:Conn_01x04_Male J1
U 1 1 629CB63C
P 850 4300
F 0 "J1" H 743 3833 50  0000 C CNN
F 1 "GroveConnector" H 743 3924 50  0000 C CNN
F 2 "Grove:SEEED_110990030" H 850 4300 50  0001 L BNN
F 3 "" H 850 4300 50  0001 L BNN
F 4 "Manufacturer Recommendations" H 850 4300 50  0001 L BNN "STANDARD"
F 5 "Seeed Technology" H 850 4300 50  0001 L BNN "MANUFACTURER"
F 6 "8.1mm" H 850 4300 50  0001 L BNN "MAXIMUM_PACKAGE_HEIGHT"
F 7 "A" H 850 4300 50  0001 L BNN "PARTREV"
	1    850  4300
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0105
U 1 1 629CD3D5
P 1450 4800
F 0 "#PWR0105" H 1450 4550 50  0001 C CNN
F 1 "GND" H 1455 4627 50  0000 C CNN
F 2 "" H 1450 4800 50  0001 C CNN
F 3 "" H 1450 4800 50  0001 C CNN
	1    1450 4800
	1    0    0    -1  
$EndComp
Wire Wire Line
	1450 4500 1450 4800
$Comp
L power:+5V #PWR0106
U 1 1 629CD7F6
P 1650 4250
F 0 "#PWR0106" H 1650 4100 50  0001 C CNN
F 1 "+5V" H 1665 4423 50  0000 C CNN
F 2 "" H 1650 4250 50  0001 C CNN
F 3 "" H 1650 4250 50  0001 C CNN
	1    1650 4250
	1    0    0    -1  
$EndComp
Wire Wire Line
	1650 4400 1650 4250
Text GLabel 1350 4300 2    50   Input ~ 0
SDA
Text GLabel 1350 4200 2    50   Input ~ 0
SCL
$Comp
L Interface_Expansion:MCP23008-xP U1
U 1 1 629CEC19
P 1700 6400
F 0 "U1" H 1700 7181 50  0000 C CNN
F 1 "MCP23008-xP" H 1700 7090 50  0000 C CNN
F 2 "Package_DIP:DIP-18_W7.62mm" H 1700 5350 50  0001 C CNN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/MCP23008-MCP23S08-Data-Sheet-20001919F.pdf" H 3000 5200 50  0001 C CNN
	1    1700 6400
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0107
U 1 1 629CF4CB
P 1050 7300
F 0 "#PWR0107" H 1050 7050 50  0001 C CNN
F 1 "GND" H 1055 7127 50  0000 C CNN
F 2 "" H 1050 7300 50  0001 C CNN
F 3 "" H 1050 7300 50  0001 C CNN
	1    1050 7300
	1    0    0    -1  
$EndComp
Wire Wire Line
	1700 7000 1700 7150
Wire Wire Line
	1700 7150 1050 7150
Wire Wire Line
	1050 7150 1050 7300
Connection ~ 1050 7150
Wire Wire Line
	1050 6600 1200 6600
Connection ~ 1050 6700
Wire Wire Line
	1050 6700 1050 6600
Wire Wire Line
	1200 6700 1050 6700
Wire Wire Line
	1050 6700 1050 6800
Wire Wire Line
	1200 6800 1050 6800
Connection ~ 1050 6800
Wire Wire Line
	1050 6800 1050 7150
Text GLabel 1000 6100 0    50   Input ~ 0
SCL
Wire Wire Line
	1000 6100 1200 6100
Text GLabel 1000 6200 0    50   Input ~ 0
SDA
Wire Wire Line
	1200 6200 1000 6200
$Comp
L power:+5V #PWR0108
U 1 1 629D2743
P 1100 5450
F 0 "#PWR0108" H 1100 5300 50  0001 C CNN
F 1 "+5V" H 1115 5623 50  0000 C CNN
F 2 "" H 1100 5450 50  0001 C CNN
F 3 "" H 1100 5450 50  0001 C CNN
	1    1100 5450
	1    0    0    -1  
$EndComp
Wire Wire Line
	1200 6000 1100 6000
Wire Wire Line
	1100 6000 1100 5550
Wire Wire Line
	1700 5800 1700 5550
Wire Wire Line
	1700 5550 1100 5550
Connection ~ 1100 5550
Wire Wire Line
	1100 5550 1100 5450
Text GLabel 2750 6000 2    50   Input ~ 0
RedLight
Wire Wire Line
	2200 6000 2650 6000
Text GLabel 3650 6750 0    50   Input ~ 0
WhiteLight
Wire Wire Line
	3950 6750 3650 6750
$Comp
L power:GND #PWR0103
U 1 1 629E0522
P 4250 7050
F 0 "#PWR0103" H 4250 6800 50  0001 C CNN
F 1 "GND" H 4255 6877 50  0000 C CNN
F 2 "" H 4250 7050 50  0001 C CNN
F 3 "" H 4250 7050 50  0001 C CNN
	1    4250 7050
	1    0    0    -1  
$EndComp
Wire Wire Line
	4250 6850 4250 7050
$Comp
L Device:LED D0
U 1 1 629E052D
P 4250 5800
F 0 "D0" V 4289 5682 50  0000 R CNN
F 1 "LED" V 4198 5682 50  0000 R CNN
F 2 "LED_THT:LED_Rectangular_W7.62mm_H4.55mm_P5.08mm_R3" H 4250 5800 50  0001 C CNN
F 3 "~" H 4250 5800 50  0001 C CNN
	1    4250 5800
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R0
U 1 1 629E0537
P 4250 6200
F 0 "R0" H 4320 6246 50  0000 L CNN
F 1 "43" H 4320 6155 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 4180 6200 50  0001 C CNN
F 3 "~" H 4250 6200 50  0001 C CNN
	1    4250 6200
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0104
U 1 1 629E0541
P 4250 5450
F 0 "#PWR0104" H 4250 5300 50  0001 C CNN
F 1 "+5V" H 4265 5623 50  0000 C CNN
F 2 "" H 4250 5450 50  0001 C CNN
F 3 "" H 4250 5450 50  0001 C CNN
	1    4250 5450
	1    0    0    -1  
$EndComp
Wire Wire Line
	4250 5450 4250 5650
Wire Wire Line
	4250 6050 4250 5950
Wire Wire Line
	4250 6350 4250 6450
$Comp
L FQP30N06L-ND:FQP30N06L-ND Q0
U 1 1 629E054E
P 4250 6650
F 0 "Q0" H 4358 6703 60  0000 L CNN
F 1 "FQP30N06L-ND" H 4358 6597 60  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-220-3_Horizontal_TabDown" H 4475 6956 60  0001 L CNN
F 3 "" H 4450 6950 60  0001 L CNN
	1    4250 6650
	1    0    0    -1  
$EndComp
Text GLabel 2750 6100 2    50   Input ~ 0
WhiteLight
Wire Wire Line
	2750 6100 2350 6100
Wire Wire Line
	1550 2600 1250 2600
$Comp
L FQP30N06L-ND:FQP30N06L-ND Q1
U 1 1 629C6FF6
P 1850 2500
F 0 "Q1" H 1958 2553 60  0000 L CNN
F 1 "FQP30N06L-ND" H 1958 2447 60  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-220-3_Horizontal_TabDown" H 2075 2806 60  0001 L CNN
F 3 "" H 2050 2800 60  0001 L CNN
	1    1850 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	1850 1800 1850 2050
Wire Wire Line
	1850 900  1850 1100
$Comp
L power:+5V #PWR0102
U 1 1 629D4592
P 1850 900
F 0 "#PWR0102" H 1850 750 50  0001 C CNN
F 1 "+5V" H 1865 1073 50  0000 C CNN
F 2 "" H 1850 900 50  0001 C CNN
F 3 "" H 1850 900 50  0001 C CNN
	1    1850 900 
	1    0    0    -1  
$EndComp
$Comp
L Device:R R1
U 1 1 629CC243
P 1850 1650
F 0 "R1" H 1920 1696 50  0000 L CNN
F 1 "150" H 1920 1605 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 1780 1650 50  0001 C CNN
F 3 "~" H 1850 1650 50  0001 C CNN
	1    1850 1650
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D1
U 1 1 629CA9E5
P 1850 1250
F 0 "D1" V 1889 1132 50  0000 R CNN
F 1 "LED" V 1798 1132 50  0000 R CNN
F 2 "LED_THT:LED_D10.0mm" H 1850 1250 50  0001 C CNN
F 3 "~" H 1850 1250 50  0001 C CNN
	1    1850 1250
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1850 2700 1850 2900
$Comp
L power:GND #PWR0101
U 1 1 629C9C96
P 1850 2900
F 0 "#PWR0101" H 1850 2650 50  0001 C CNN
F 1 "GND" H 1855 2727 50  0000 C CNN
F 2 "" H 1850 2900 50  0001 C CNN
F 3 "" H 1850 2900 50  0001 C CNN
	1    1850 2900
	1    0    0    -1  
$EndComp
Text GLabel 1250 2600 0    50   Input ~ 0
RedLight
Wire Wire Line
	2550 1500 2550 1400
Wire Wire Line
	2550 900  2550 1100
$Comp
L power:+5V #PWR0109
U 1 1 62A10749
P 2550 900
F 0 "#PWR0109" H 2550 750 50  0001 C CNN
F 1 "+5V" H 2565 1073 50  0000 C CNN
F 2 "" H 2550 900 50  0001 C CNN
F 3 "" H 2550 900 50  0001 C CNN
	1    2550 900 
	1    0    0    -1  
$EndComp
$Comp
L Device:R R2
U 1 1 62A10753
P 2550 1650
F 0 "R2" H 2620 1696 50  0000 L CNN
F 1 "150" H 2620 1605 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 2480 1650 50  0001 C CNN
F 3 "~" H 2550 1650 50  0001 C CNN
	1    2550 1650
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D2
U 1 1 62A1075D
P 2550 1250
F 0 "D2" V 2589 1132 50  0000 R CNN
F 1 "LED" V 2498 1132 50  0000 R CNN
F 2 "LED_THT:LED_D10.0mm" H 2550 1250 50  0001 C CNN
F 3 "~" H 2550 1250 50  0001 C CNN
	1    2550 1250
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3150 1500 3150 1400
Wire Wire Line
	3150 900  3150 1100
$Comp
L power:+5V #PWR0111
U 1 1 62A2FC2A
P 3150 900
F 0 "#PWR0111" H 3150 750 50  0001 C CNN
F 1 "+5V" H 3165 1073 50  0000 C CNN
F 2 "" H 3150 900 50  0001 C CNN
F 3 "" H 3150 900 50  0001 C CNN
	1    3150 900 
	1    0    0    -1  
$EndComp
$Comp
L Device:R R3
U 1 1 62A2FC34
P 3150 1650
F 0 "R3" H 3220 1696 50  0000 L CNN
F 1 "150" H 3220 1605 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 3080 1650 50  0001 C CNN
F 3 "~" H 3150 1650 50  0001 C CNN
	1    3150 1650
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D3
U 1 1 62A2FC3E
P 3150 1250
F 0 "D3" V 3189 1132 50  0000 R CNN
F 1 "LED" V 3098 1132 50  0000 R CNN
F 2 "LED_THT:LED_D10.0mm" H 3150 1250 50  0001 C CNN
F 3 "~" H 3150 1250 50  0001 C CNN
	1    3150 1250
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3750 1500 3750 1400
Wire Wire Line
	3750 900  3750 1100
$Comp
L power:+5V #PWR0113
U 1 1 62A2FC62
P 3750 900
F 0 "#PWR0113" H 3750 750 50  0001 C CNN
F 1 "+5V" H 3765 1073 50  0000 C CNN
F 2 "" H 3750 900 50  0001 C CNN
F 3 "" H 3750 900 50  0001 C CNN
	1    3750 900 
	1    0    0    -1  
$EndComp
$Comp
L Device:R R4
U 1 1 62A2FC6C
P 3750 1650
F 0 "R4" H 3820 1696 50  0000 L CNN
F 1 "150" H 3820 1605 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 3680 1650 50  0001 C CNN
F 3 "~" H 3750 1650 50  0001 C CNN
	1    3750 1650
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D4
U 1 1 62A2FC76
P 3750 1250
F 0 "D4" V 3789 1132 50  0000 R CNN
F 1 "LED" V 3698 1132 50  0000 R CNN
F 2 "LED_THT:LED_D10.0mm" H 3750 1250 50  0001 C CNN
F 3 "~" H 3750 1250 50  0001 C CNN
	1    3750 1250
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4400 1500 4400 1400
Wire Wire Line
	4400 900  4400 1100
$Comp
L power:+5V #PWR0115
U 1 1 62A4527D
P 4400 900
F 0 "#PWR0115" H 4400 750 50  0001 C CNN
F 1 "+5V" H 4415 1073 50  0000 C CNN
F 2 "" H 4400 900 50  0001 C CNN
F 3 "" H 4400 900 50  0001 C CNN
	1    4400 900 
	1    0    0    -1  
$EndComp
$Comp
L Device:R R5
U 1 1 62A45287
P 4400 1650
F 0 "R5" H 4470 1696 50  0000 L CNN
F 1 "150" H 4470 1605 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 4330 1650 50  0001 C CNN
F 3 "~" H 4400 1650 50  0001 C CNN
	1    4400 1650
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D5
U 1 1 62A45291
P 4400 1250
F 0 "D5" V 4439 1132 50  0000 R CNN
F 1 "LED" V 4348 1132 50  0000 R CNN
F 2 "LED_THT:LED_D10.0mm" H 4400 1250 50  0001 C CNN
F 3 "~" H 4400 1250 50  0001 C CNN
	1    4400 1250
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5050 1500 5050 1400
Wire Wire Line
	5050 900  5050 1100
$Comp
L power:+5V #PWR0117
U 1 1 62A452B5
P 5050 900
F 0 "#PWR0117" H 5050 750 50  0001 C CNN
F 1 "+5V" H 5065 1073 50  0000 C CNN
F 2 "" H 5050 900 50  0001 C CNN
F 3 "" H 5050 900 50  0001 C CNN
	1    5050 900 
	1    0    0    -1  
$EndComp
$Comp
L Device:R R6
U 1 1 62A452BF
P 5050 1650
F 0 "R6" H 5120 1696 50  0000 L CNN
F 1 "150" H 5120 1605 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 4980 1650 50  0001 C CNN
F 3 "~" H 5050 1650 50  0001 C CNN
	1    5050 1650
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D6
U 1 1 62A452C9
P 5050 1250
F 0 "D6" V 5089 1132 50  0000 R CNN
F 1 "LED" V 4998 1132 50  0000 R CNN
F 2 "LED_THT:LED_D10.0mm" H 5050 1250 50  0001 C CNN
F 3 "~" H 5050 1250 50  0001 C CNN
	1    5050 1250
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5700 1500 5700 1400
Wire Wire Line
	5700 900  5700 1100
$Comp
L power:+5V #PWR0119
U 1 1 62A452ED
P 5700 900
F 0 "#PWR0119" H 5700 750 50  0001 C CNN
F 1 "+5V" H 5715 1073 50  0000 C CNN
F 2 "" H 5700 900 50  0001 C CNN
F 3 "" H 5700 900 50  0001 C CNN
	1    5700 900 
	1    0    0    -1  
$EndComp
$Comp
L Device:R R7
U 1 1 62A452F7
P 5700 1650
F 0 "R7" H 5770 1696 50  0000 L CNN
F 1 "150" H 5770 1605 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 5630 1650 50  0001 C CNN
F 3 "~" H 5700 1650 50  0001 C CNN
	1    5700 1650
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D7
U 1 1 62A45301
P 5700 1250
F 0 "D7" V 5739 1132 50  0000 R CNN
F 1 "LED" V 5648 1132 50  0000 R CNN
F 2 "LED_THT:LED_D10.0mm" H 5700 1250 50  0001 C CNN
F 3 "~" H 5700 1250 50  0001 C CNN
	1    5700 1250
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6350 1500 6350 1400
Wire Wire Line
	6350 900  6350 1100
$Comp
L power:+5V #PWR0121
U 1 1 62A45325
P 6350 900
F 0 "#PWR0121" H 6350 750 50  0001 C CNN
F 1 "+5V" H 6365 1073 50  0000 C CNN
F 2 "" H 6350 900 50  0001 C CNN
F 3 "" H 6350 900 50  0001 C CNN
	1    6350 900 
	1    0    0    -1  
$EndComp
$Comp
L Device:R R8
U 1 1 62A4532F
P 6350 1650
F 0 "R8" H 6420 1696 50  0000 L CNN
F 1 "150" H 6420 1605 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 6280 1650 50  0001 C CNN
F 3 "~" H 6350 1650 50  0001 C CNN
	1    6350 1650
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D8
U 1 1 62A45339
P 6350 1250
F 0 "D8" V 6389 1132 50  0000 R CNN
F 1 "LED" V 6298 1132 50  0000 R CNN
F 2 "LED_THT:LED_D10.0mm" H 6350 1250 50  0001 C CNN
F 3 "~" H 6350 1250 50  0001 C CNN
	1    6350 1250
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1850 1400 1850 1500
Wire Wire Line
	2550 2050 1850 2050
Wire Wire Line
	2550 1800 2550 2050
Connection ~ 1850 2050
Wire Wire Line
	1850 2050 1850 2300
Wire Wire Line
	3150 2050 2550 2050
Wire Wire Line
	3150 1800 3150 2050
Connection ~ 2550 2050
Wire Wire Line
	3750 2050 3150 2050
Connection ~ 3150 2050
Wire Wire Line
	3750 1800 3750 2050
Wire Wire Line
	4400 2050 3750 2050
Wire Wire Line
	4400 1800 4400 2050
Connection ~ 3750 2050
Wire Wire Line
	5050 2050 4400 2050
Wire Wire Line
	5050 1800 5050 2050
Connection ~ 4400 2050
Wire Wire Line
	5700 2050 5050 2050
Wire Wire Line
	5700 1800 5700 2050
Connection ~ 5050 2050
Wire Wire Line
	5700 2100 5700 2050
Connection ~ 5700 2050
Wire Wire Line
	6350 1800 6350 2050
Wire Wire Line
	6350 2050 5700 2050
Wire Wire Line
	1050 4200 1350 4200
Wire Wire Line
	1050 4300 1350 4300
Wire Wire Line
	1050 4400 1650 4400
Wire Wire Line
	1050 4500 1450 4500
$Comp
L Connector:Conn_01x04_Male J2
U 1 1 62B506F8
P 2350 4300
F 0 "J2" H 2243 3833 50  0000 C CNN
F 1 "GroveConnector" H 2243 3924 50  0000 C CNN
F 2 "Grove:SEEED_110990030" H 2350 4300 50  0001 L BNN
F 3 "" H 2350 4300 50  0001 L BNN
F 4 "Manufacturer Recommendations" H 2350 4300 50  0001 L BNN "STANDARD"
F 5 "Seeed Technology" H 2350 4300 50  0001 L BNN "MANUFACTURER"
F 6 "8.1mm" H 2350 4300 50  0001 L BNN "MAXIMUM_PACKAGE_HEIGHT"
F 7 "A" H 2350 4300 50  0001 L BNN "PARTREV"
	1    2350 4300
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0110
U 1 1 62B50A1E
P 2950 4800
F 0 "#PWR0110" H 2950 4550 50  0001 C CNN
F 1 "GND" H 2955 4627 50  0000 C CNN
F 2 "" H 2950 4800 50  0001 C CNN
F 3 "" H 2950 4800 50  0001 C CNN
	1    2950 4800
	1    0    0    -1  
$EndComp
Wire Wire Line
	2950 4500 2950 4800
$Comp
L power:+5V #PWR0112
U 1 1 62B50A29
P 3150 4250
F 0 "#PWR0112" H 3150 4100 50  0001 C CNN
F 1 "+5V" H 3165 4423 50  0000 C CNN
F 2 "" H 3150 4250 50  0001 C CNN
F 3 "" H 3150 4250 50  0001 C CNN
	1    3150 4250
	1    0    0    -1  
$EndComp
Wire Wire Line
	3150 4400 3150 4250
Text GLabel 2850 4300 2    50   Input ~ 0
SDA
Text GLabel 2850 4200 2    50   Input ~ 0
SCL
Wire Wire Line
	2550 4200 2850 4200
Wire Wire Line
	2550 4300 2850 4300
Wire Wire Line
	2550 4400 3150 4400
Wire Wire Line
	2550 4500 2950 4500
$Comp
L Connector:Conn_01x04_Male J3
U 1 1 62B781FF
P 3950 4300
F 0 "J3" H 3843 3833 50  0000 C CNN
F 1 "GroveConnector" H 3843 3924 50  0000 C CNN
F 2 "Grove:SEEED_110990030" H 3950 4300 50  0001 L BNN
F 3 "" H 3950 4300 50  0001 L BNN
F 4 "Manufacturer Recommendations" H 3950 4300 50  0001 L BNN "STANDARD"
F 5 "Seeed Technology" H 3950 4300 50  0001 L BNN "MANUFACTURER"
F 6 "8.1mm" H 3950 4300 50  0001 L BNN "MAXIMUM_PACKAGE_HEIGHT"
F 7 "A" H 3950 4300 50  0001 L BNN "PARTREV"
	1    3950 4300
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0114
U 1 1 62B78463
P 4550 4800
F 0 "#PWR0114" H 4550 4550 50  0001 C CNN
F 1 "GND" H 4555 4627 50  0000 C CNN
F 2 "" H 4550 4800 50  0001 C CNN
F 3 "" H 4550 4800 50  0001 C CNN
	1    4550 4800
	1    0    0    -1  
$EndComp
Wire Wire Line
	4550 4500 4550 4800
$Comp
L power:+5V #PWR0116
U 1 1 62B7846E
P 4750 4250
F 0 "#PWR0116" H 4750 4100 50  0001 C CNN
F 1 "+5V" H 4765 4423 50  0000 C CNN
F 2 "" H 4750 4250 50  0001 C CNN
F 3 "" H 4750 4250 50  0001 C CNN
	1    4750 4250
	1    0    0    -1  
$EndComp
Wire Wire Line
	4750 4400 4750 4250
Text GLabel 4450 4300 2    50   Input ~ 0
SDA
Text GLabel 4450 4200 2    50   Input ~ 0
SCL
Wire Wire Line
	4150 4200 4450 4200
Wire Wire Line
	4150 4300 4450 4300
Wire Wire Line
	4150 4400 4750 4400
Wire Wire Line
	4150 4500 4550 4500
$Comp
L power:GND #PWR?
U 1 1 62B97621
P 2550 6700
F 0 "#PWR?" H 2550 6450 50  0001 C CNN
F 1 "GND" H 2555 6527 50  0000 C CNN
F 2 "" H 2550 6700 50  0001 C CNN
F 3 "" H 2550 6700 50  0001 C CNN
	1    2550 6700
	1    0    0    -1  
$EndComp
$Comp
L Device:R R10
U 1 1 62B98026
P 2650 6400
F 0 "R10" H 2720 6446 50  0000 L CNN
F 1 "10K" H 2720 6355 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 2580 6400 50  0001 C CNN
F 3 "~" H 2650 6400 50  0001 C CNN
	1    2650 6400
	1    0    0    -1  
$EndComp
Wire Wire Line
	2650 6000 2650 6250
Connection ~ 2650 6000
Wire Wire Line
	2650 6000 2750 6000
Wire Wire Line
	2650 6550 2650 6600
Wire Wire Line
	2650 6600 2550 6600
Wire Wire Line
	2550 6600 2550 6700
$Comp
L Device:R R9
U 1 1 62BA6C85
P 2350 6400
F 0 "R9" H 2420 6446 50  0000 L CNN
F 1 "10K" H 2420 6355 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 2280 6400 50  0001 C CNN
F 3 "~" H 2350 6400 50  0001 C CNN
	1    2350 6400
	1    0    0    -1  
$EndComp
Wire Wire Line
	2350 6250 2350 6100
Connection ~ 2350 6100
Wire Wire Line
	2350 6100 2200 6100
Wire Wire Line
	2350 6550 2350 6600
Wire Wire Line
	2350 6600 2550 6600
Connection ~ 2550 6600
$EndSCHEMATC
