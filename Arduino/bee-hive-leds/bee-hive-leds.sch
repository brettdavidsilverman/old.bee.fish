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
L Interface_Expansion:MCP23008-xP U1
U 1 1 6129951E
P 3050 2500
F 0 "U1" H 3050 3281 50  0000 C CNN
F 1 "MCP23008-xP" H 3050 3190 50  0000 C CNN
F 2 "Package_DIP:DIP-18_W7.62mm" H 3050 1450 50  0001 C CNN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/MCP23008-MCP23S08-Data-Sheet-20001919F.pdf" H 4350 1300 50  0001 C CNN
	1    3050 2500
	1    0    0    -1  
$EndComp
Text GLabel 2150 2200 0    50   Input ~ 0
I2C_SCL
Text GLabel 2150 2300 0    50   Input ~ 0
I2C_SDA
Wire Wire Line
	2550 2300 2150 2300
$Comp
L power:GND #PWR0101
U 1 1 6129CC9A
P 2100 3300
F 0 "#PWR0101" H 2100 3050 50  0001 C CNN
F 1 "GND" H 2105 3127 50  0000 C CNN
F 2 "" H 2100 3300 50  0001 C CNN
F 3 "" H 2100 3300 50  0001 C CNN
	1    2100 3300
	1    0    0    -1  
$EndComp
Wire Wire Line
	2550 2700 2100 2700
Wire Wire Line
	2100 2700 2100 2800
Wire Wire Line
	2550 2800 2100 2800
Connection ~ 2100 2800
Wire Wire Line
	2100 2800 2100 2900
Wire Wire Line
	2550 2900 2100 2900
Connection ~ 2100 2900
Wire Wire Line
	2100 2900 2100 3200
$Comp
L power:+5V #PWR0102
U 1 1 6129D615
P 2400 1650
F 0 "#PWR0102" H 2400 1500 50  0001 C CNN
F 1 "+5V" H 2415 1823 50  0000 C CNN
F 2 "" H 2400 1650 50  0001 C CNN
F 3 "" H 2400 1650 50  0001 C CNN
	1    2400 1650
	1    0    0    -1  
$EndComp
Wire Wire Line
	2150 2200 2550 2200
Wire Wire Line
	2550 2100 2400 2100
Wire Wire Line
	2400 2100 2400 1900
Wire Wire Line
	2400 1900 3050 1900
Connection ~ 2400 1900
Wire Wire Line
	2400 1900 2400 1650
Wire Wire Line
	3050 3100 3050 3200
Wire Wire Line
	3050 3200 2100 3200
Connection ~ 2100 3200
Wire Wire Line
	2100 3200 2100 3300
$Comp
L power:GND #PWR0103
U 1 1 612A5045
P 4850 3300
F 0 "#PWR0103" H 4850 3050 50  0001 C CNN
F 1 "GND" H 4855 3127 50  0000 C CNN
F 2 "" H 4850 3300 50  0001 C CNN
F 3 "" H 4850 3300 50  0001 C CNN
	1    4850 3300
	1    0    0    -1  
$EndComp
$Comp
L Device:Battery_Cell BT1
U 1 1 612ACE83
P 5850 2150
F 0 "BT1" H 5968 2246 50  0000 L CNN
F 1 "Battery_Cell" H 5968 2155 50  0000 L CNN
F 2 "" V 5850 2210 50  0001 C CNN
F 3 "~" V 5850 2210 50  0001 C CNN
	1    5850 2150
	1    0    0    -1  
$EndComp
Wire Wire Line
	5850 1950 5850 1750
Wire Wire Line
	5850 3200 4850 3200
Connection ~ 4850 3200
Wire Wire Line
	4850 3200 4850 3300
Wire Wire Line
	5850 2250 5850 3200
Connection ~ 3050 3200
$Comp
L Transistor_FET:IRF540N Q1
U 1 1 612A1034
P 4750 2100
F 0 "Q1" H 4954 2146 50  0000 L CNN
F 1 "IRF1405N" H 4954 2055 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-220-3_Vertical" H 5000 2025 50  0001 L CIN
F 3 "http://www.irf.com/product-info/datasheets/data/irf540n.pdf" H 4750 2100 50  0001 L CNN
	1    4750 2100
	1    0    0    -1  
$EndComp
Wire Wire Line
	4850 2300 4850 3200
$Comp
L Device:LED D1
U 1 1 612E874E
P 5100 1750
F 0 "D1" H 5093 1967 50  0000 C CNN
F 1 "LED" H 5093 1876 50  0000 C CNN
F 2 "LED_THT:LED_D5.0mm_Horizontal_O1.27mm_Z3.0mm_Clear" H 5100 1750 50  0001 C CNN
F 3 "~" H 5100 1750 50  0001 C CNN
	1    5100 1750
	1    0    0    -1  
$EndComp
Wire Wire Line
	4950 1750 4850 1750
Wire Wire Line
	4850 1750 4850 1900
Wire Wire Line
	3050 3200 4250 3200
Connection ~ 4250 3200
Wire Wire Line
	4250 3200 4850 3200
$Comp
L Device:R_US R2
U 1 1 6138F7A0
P 5550 1750
F 0 "R2" V 5345 1750 50  0000 C CNN
F 1 "100" V 5436 1750 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P5.08mm_Horizontal" V 5590 1740 50  0001 C CNN
F 3 "~" H 5550 1750 50  0001 C CNN
	1    5550 1750
	0    1    1    0   
$EndComp
Wire Wire Line
	5850 1750 5700 1750
Wire Wire Line
	5400 1750 5250 1750
$Comp
L Device:R_US R1
U 1 1 613933C6
P 4250 2500
F 0 "R1" H 4318 2546 50  0000 L CNN
F 1 "10K" H 4318 2455 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P5.08mm_Horizontal" V 4290 2490 50  0001 C CNN
F 3 "~" H 4250 2500 50  0001 C CNN
	1    4250 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	3550 2100 4250 2100
Wire Wire Line
	4250 2650 4250 3200
Wire Wire Line
	4250 2350 4250 2100
Connection ~ 4250 2100
Wire Wire Line
	4250 2100 4550 2100
$EndSCHEMATC
