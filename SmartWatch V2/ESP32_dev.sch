EESchema Schematic File Version 4
LIBS:SmartWatch-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 2
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
L SmartWatch-rescue:ESP32-PICO-D4-ESP32-PICO-D4-laser_tanks_r2-rescue U12
U 1 1 5D9A0DC8
P 6650 3600
F 0 "U12" H 6650 5167 50  0000 C CNN
F 1 "ESP32-PICO-D4" H 6650 5076 50  0000 C CNN
F 2 "SmartWatch:PQFN50P700X700X104-49N" H 6650 3600 50  0001 L BNN
F 3 "Unavailable" H 6650 3600 50  0001 L BNN
F 4 "SMD-48 Espressif Systems" H 6650 3600 50  0001 L BNN "Field4"
F 5 "ESP32-PICO-D4" H 6650 3600 50  0001 L BNN "Field5"
F 6 "WIFi/Bluetooth SIP" H 6650 3600 50  0001 L BNN "Field6"
F 7 "Espressif Systems" H 6650 3600 50  0001 L BNN "Field7"
F 8 "None" H 6650 3600 50  0001 L BNN "Field8"
F 9 "1904-1029-1-ND" H 6650 3600 50  0001 C CNN "DigiKey Part Number"
F 10 "https://www.digikey.com/product-detail/en/espressif-systems/ESP32-PICO-D4/1904-1029-1-ND/9381738" H -250 -350 50  0001 C CNN "Digikey"
	1    6650 3600
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 5D9CEE7A
P 1600 1400
F 0 "#PWR0102" H 1600 1150 50  0001 C CNN
F 1 "GND" H 1700 1400 50  0000 C CNN
F 2 "" H 1600 1400 50  0001 C CNN
F 3 "" H 1600 1400 50  0001 C CNN
	1    1600 1400
	1    0    0    -1  
$EndComp
NoConn ~ 1550 1300
Wire Wire Line
	1700 900  1900 900 
Wire Wire Line
	2250 900  2500 900 
Text Label 3100 1200 0    50   ~ 0
USB_DP
Text Label 3100 1100 0    50   ~ 0
USB_DN
$Comp
L Device:D_TVS_ALT D1
U 1 1 5D9F32A5
P 1900 1600
F 0 "D1" V 1850 1450 50  0000 L CNN
F 1 "LESD5D5.0CT1G" V 1950 2100 50  0000 L CNN
F 2 "Diode_SMD:D_SOD-523" H 1900 1600 50  0001 C CNN
F 3 "~" H 1900 1600 50  0001 C CNN
	1    1900 1600
	0    1    1    0   
$EndComp
$Comp
L Device:D_TVS_ALT D3
U 1 1 5D9F3DDC
P 2100 1600
F 0 "D3" V 2050 1450 50  0000 L CNN
F 1 "LESD5D5.0CT1G" V 2350 1900 50  0000 L CNN
F 2 "Diode_SMD:D_SOD-523" H 2100 1600 50  0001 C CNN
F 3 "~" H 2100 1600 50  0001 C CNN
	1    2100 1600
	0    1    1    0   
$EndComp
$Comp
L Device:D_TVS_ALT D4
U 1 1 5D9F40EF
P 2300 1600
F 0 "D4" V 2250 1450 50  0000 L CNN
F 1 "LESD5D5.0CT1G" V 2450 1700 50  0000 L CNN
F 2 "Diode_SMD:D_SOD-523" H 2300 1600 50  0001 C CNN
F 3 "~" H 2300 1600 50  0001 C CNN
	1    2300 1600
	0    1    1    0   
$EndComp
Wire Wire Line
	1900 1450 1900 900 
Connection ~ 1900 900 
Wire Wire Line
	1900 900  1950 900 
Wire Wire Line
	2100 1450 2100 1100
Connection ~ 2100 1100
Wire Wire Line
	2300 1450 2300 1200
Connection ~ 2300 1200
Wire Wire Line
	1900 1750 1900 1850
Wire Wire Line
	1900 1850 2100 1850
Wire Wire Line
	2100 1850 2100 1750
Wire Wire Line
	2100 1850 2300 1850
Wire Wire Line
	2300 1850 2300 1750
Connection ~ 2100 1850
$Comp
L power:GND #PWR0103
U 1 1 5D9FB768
P 2100 1850
F 0 "#PWR0103" H 2100 1600 50  0001 C CNN
F 1 "GND" H 2105 1677 50  0000 C CNN
F 2 "" H 2100 1850 50  0001 C CNN
F 3 "" H 2100 1850 50  0001 C CNN
	1    2100 1850
	1    0    0    -1  
$EndComp
$Comp
L Device:R R7
U 1 1 5D99D30B
P 1800 2850
F 0 "R7" H 1870 2896 50  0000 L CNN
F 1 "0(NC)" H 1870 2805 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 1730 2850 50  0001 C CNN
F 3 "~" H 1800 2850 50  0001 C CNN
F 4 "311-0.0GRCT-ND" H 0   0   50  0001 C CNN "DigiKey Part Number"
	1    1800 2850
	1    0    0    -1  
$EndComp
$Comp
L Device:R R5
U 1 1 5D99E43F
P 1650 2850
F 0 "R5" H 1581 2804 50  0000 R CNN
F 1 "0(NC)" H 1581 2895 50  0000 R CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 1580 2850 50  0001 C CNN
F 3 "~" H 1650 2850 50  0001 C CNN
F 4 "311-0.0GRCT-ND" H 0   0   50  0001 C CNN "DigiKey Part Number"
	1    1650 2850
	1    0    0    1   
$EndComp
$Comp
L Device:R R2
U 1 1 5D99E786
P 1100 2850
F 0 "R2" H 1030 2804 50  0000 R CNN
F 1 "1k" H 1030 2895 50  0000 R CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 1030 2850 50  0001 C CNN
F 3 "~" H 1100 2850 50  0001 C CNN
F 4 "311-1.0KGRCT-ND" H 0   0   50  0001 C CNN "DigiKey Part Number"
	1    1100 2850
	-1   0    0    1   
$EndComp
$Comp
L Device:R R1
U 1 1 5D99EBB4
P 950 2850
F 0 "R1" H 881 2804 50  0000 R CNN
F 1 "1k" H 881 2895 50  0000 R CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 880 2850 50  0001 C CNN
F 3 "~" H 950 2850 50  0001 C CNN
F 4 "311-1.0KGRCT-ND" H 0   0   50  0001 C CNN "DigiKey Part Number"
	1    950  2850
	1    0    0    1   
$EndComp
Wire Wire Line
	950  2700 950  2500
Wire Wire Line
	1100 2700 1100 2500
Wire Wire Line
	1650 2700 1650 2500
Wire Wire Line
	1800 2700 1800 2500
Wire Wire Line
	950  3000 950  3300
Wire Wire Line
	1100 3000 1100 3300
Wire Wire Line
	1650 3000 1650 3300
Wire Wire Line
	1800 3000 1800 3300
Text Label 950  2500 2    50   ~ 0
TXD0
Text Label 1100 2500 0    50   ~ 0
RXD0
Text Label 1800 2500 0    50   ~ 0
IO15
Text Label 1650 2500 2    50   ~ 0
IO13
Text Label 950  3300 2    50   ~ 0
RXD
Text Label 1100 3300 0    50   ~ 0
TXD
Text Label 1800 3300 0    50   ~ 0
CTS
Text Label 1650 3300 2    50   ~ 0
RTS
$Comp
L Device:R R13
U 1 1 5D9A9899
P 3100 2350
F 0 "R13" V 3307 2350 50  0000 C CNN
F 1 "10k" V 3216 2350 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 3030 2350 50  0001 C CNN
F 3 "~" H 3100 2350 50  0001 C CNN
F 4 "311-10KGRCT-ND" H 0   0   50  0001 C CNN "DigiKey Part Number"
	1    3100 2350
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R14
U 1 1 5D9A9C86
P 3100 3150
F 0 "R14" V 3307 3150 50  0000 C CNN
F 1 "10k" V 3216 3150 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 3030 3150 50  0001 C CNN
F 3 "~" H 3100 3150 50  0001 C CNN
F 4 "311-10KGRCT-ND" H 0   0   50  0001 C CNN "DigiKey Part Number"
	1    3100 3150
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3700 2350 3250 2350
Wire Wire Line
	2950 2350 2750 2350
Wire Wire Line
	2750 2350 2750 2800
Wire Wire Line
	2750 2800 4000 2800
Wire Wire Line
	4000 2800 4000 2950
Connection ~ 2750 2350
Wire Wire Line
	2750 2350 2600 2350
Wire Wire Line
	4000 2550 4000 2700
Wire Wire Line
	4000 2700 2850 2700
Wire Wire Line
	2850 2700 2850 3150
Wire Wire Line
	2850 3150 2950 3150
Wire Wire Line
	2850 3150 2600 3150
Connection ~ 2850 3150
Wire Wire Line
	3250 3150 3700 3150
Wire Wire Line
	4000 2150 4000 2050
Wire Wire Line
	4000 2050 4350 2050
Wire Wire Line
	4000 3350 4000 3450
Wire Wire Line
	4000 3450 4400 3450
Text Label 2600 3150 2    50   ~ 0
RTS
Text Label 2600 2350 2    50   ~ 0
DTR
Text Label 4350 2050 2    50   ~ 0
EN
Text Label 4400 3450 2    50   ~ 0
IO0
Text Notes 3350 1700 0    50   ~ 0
Auto program\nDTR RTS --> EN IO0\n 1    1        1    1\n 0    0        1    1\n 1    0        0    1\n 0    1        1    0\n
Wire Notes Line width 20 style solid
	550  500  4500 500 
Wire Notes Line width 20 style solid
	4500 500  4500 6300
Wire Notes Line width 20 style solid
	4500 6300 550  6300
Text Notes 3050 6200 0    50   ~ 0
Micro-USB and USB-UART Interface\n
$Comp
L Device:C C6
U 1 1 5DAF32B1
P 1150 7300
F 0 "C6" H 1265 7346 50  0000 L CNN
F 1 "0.1u (NC)" H 1265 7255 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 1188 7150 50  0001 C CNN
F 3 "~" H 1150 7300 50  0001 C CNN
F 4 "1276-1258-1-ND" H 0   0   50  0001 C CNN "DigiKey Part Number"
	1    1150 7300
	0    1    1    0   
$EndComp
$Comp
L Device:R R8
U 1 1 5DAF4F8B
P 1800 6950
F 0 "R8" V 1700 6950 50  0000 C CNN
F 1 "0R" V 1600 6950 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 1730 6950 50  0001 C CNN
F 3 "~" H 1800 6950 50  0001 C CNN
F 4 "311-0.0GRCT-ND" H 0   0   50  0001 C CNN "DigiKey Part Number"
	1    1800 6950
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0108
U 1 1 5DAFD213
P 750 7500
F 0 "#PWR0108" H 750 7250 50  0001 C CNN
F 1 "GND" H 755 7327 50  0000 C CNN
F 2 "" H 750 7500 50  0001 C CNN
F 3 "" H 750 7500 50  0001 C CNN
	1    750  7500
	1    0    0    -1  
$EndComp
Connection ~ 750  7300
Wire Wire Line
	750  7300 750  7500
Wire Wire Line
	1300 7300 1550 7300
Wire Wire Line
	1550 7300 1550 7150
Wire Wire Line
	1550 6950 1650 6950
Wire Wire Line
	1950 6950 2250 6950
Text Label 2250 6950 2    50   ~ 0
IO0
$Comp
L Device:C C7
U 1 1 5DB19C58
P 3000 7300
F 0 "C7" H 3115 7346 50  0000 L CNN
F 1 "0.1u" H 3115 7255 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 3038 7150 50  0001 C CNN
F 3 "~" H 3000 7300 50  0001 C CNN
F 4 "1276-1258-1-ND" H 0   0   50  0001 C CNN "DigiKey Part Number"
	1    3000 7300
	0    1    1    0   
$EndComp
$Comp
L Device:R R19
U 1 1 5DB19C5E
P 3650 6950
F 0 "R19" V 3550 6950 50  0000 C CNN
F 1 "0R" V 3450 6950 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 3580 6950 50  0001 C CNN
F 3 "~" H 3650 6950 50  0001 C CNN
F 4 "311-0.0GRCT-ND" H 0   0   50  0001 C CNN "DigiKey Part Number"
	1    3650 6950
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0109
U 1 1 5DB19C64
P 2600 7500
F 0 "#PWR0109" H 2600 7250 50  0001 C CNN
F 1 "GND" H 2605 7327 50  0000 C CNN
F 2 "" H 2600 7500 50  0001 C CNN
F 3 "" H 2600 7500 50  0001 C CNN
	1    2600 7500
	1    0    0    -1  
$EndComp
Connection ~ 2600 7300
Wire Wire Line
	2600 7300 2600 7500
Wire Wire Line
	3150 7300 3400 7300
Wire Wire Line
	3400 7300 3400 7150
Wire Wire Line
	3400 6950 3500 6950
Wire Wire Line
	3800 6950 4100 6950
Text Label 4100 6950 2    50   ~ 0
EN
Wire Notes Line width 20 style solid
	550  6350 4500 6350
Wire Notes Line width 20 style solid
	4500 6350 4500 7800
Wire Notes Line width 20 style solid
	4500 7800 550  7800
Wire Notes Line width 20 style solid
	550  7800 550  6350
Text Notes 3850 7750 0    50   ~ 0
Switch Buttons\n
Wire Wire Line
	7450 4900 7550 4900
Wire Wire Line
	7550 4900 7550 5000
$Comp
L power:GND #PWR0110
U 1 1 5D98F09E
P 7550 5000
F 0 "#PWR0110" H 7550 4750 50  0001 C CNN
F 1 "GND" H 7555 4827 50  0000 C CNN
F 2 "" H 7550 5000 50  0001 C CNN
F 3 "" H 7550 5000 50  0001 C CNN
	1    7550 5000
	1    0    0    -1  
$EndComp
Text Label 4900 850  0    50   ~ 0
VDD33
Text Label 6450 850  2    50   ~ 0
VDD33A
$Comp
L Device:C C10
U 1 1 5D9D6736
P 5700 1100
F 0 "C10" H 5815 1146 50  0000 L CNN
F 1 "10u" H 5815 1055 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 5738 950 50  0001 C CNN
F 3 "~" H 5700 1100 50  0001 C CNN
F 4 "478-10766-1-ND" H -250 -350 50  0001 C CNN "DigiKey Part Number"
	1    5700 1100
	1    0    0    -1  
$EndComp
$Comp
L Device:C C11
U 1 1 5D9D877D
P 6100 1100
F 0 "C11" H 6215 1146 50  0000 L CNN
F 1 "0.1u" H 6215 1055 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 6138 950 50  0001 C CNN
F 3 "~" H 6100 1100 50  0001 C CNN
F 4 "1276-1258-1-ND" H -250 -350 50  0001 C CNN "DigiKey Part Number"
	1    6100 1100
	1    0    0    -1  
$EndComp
Wire Wire Line
	5700 950  5700 850 
Connection ~ 5700 850 
Wire Wire Line
	5700 850  6100 850 
Wire Wire Line
	6100 950  6100 850 
Connection ~ 6100 850 
Wire Wire Line
	6100 850  6450 850 
$Comp
L power:GND #PWR0120
U 1 1 5D9E0C3A
P 5700 1450
F 0 "#PWR0120" H 5700 1200 50  0001 C CNN
F 1 "GND" H 5705 1277 50  0000 C CNN
F 2 "" H 5700 1450 50  0001 C CNN
F 3 "" H 5700 1450 50  0001 C CNN
	1    5700 1450
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0121
U 1 1 5D9E0FFA
P 6100 1450
F 0 "#PWR0121" H 6100 1200 50  0001 C CNN
F 1 "GND" H 6105 1277 50  0000 C CNN
F 2 "" H 6100 1450 50  0001 C CNN
F 3 "" H 6100 1450 50  0001 C CNN
	1    6100 1450
	1    0    0    -1  
$EndComp
Wire Wire Line
	5700 1250 5700 1450
Wire Wire Line
	6100 1250 6100 1450
Wire Wire Line
	7450 2300 7750 2300
Wire Wire Line
	7450 2400 7750 2400
Wire Wire Line
	7450 2500 7750 2500
Wire Wire Line
	7750 2500 7750 2400
Connection ~ 7750 2400
Wire Wire Line
	7750 2400 7750 2300
NoConn ~ 7450 2600
Wire Wire Line
	7450 2700 7750 2700
Wire Wire Line
	7750 2700 7750 2500
Connection ~ 7750 2500
Wire Wire Line
	7750 2300 8200 2300
Connection ~ 7750 2300
Text Label 8200 2300 2    50   ~ 0
VDD33A
$Comp
L Device:R R22
U 1 1 5DA3C213
P 8250 4200
F 0 "R22" H 8320 4246 50  0000 L CNN
F 1 "10k" H 8320 4155 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 8180 4200 50  0001 C CNN
F 3 "~" H 8250 4200 50  0001 C CNN
F 4 "311-10KGRCT-ND" H -150 -350 50  0001 C CNN "DigiKey Part Number"
	1    8250 4200
	0    1    1    0   
$EndComp
Text Label 8750 4200 2    50   ~ 0
VDD33
Wire Wire Line
	8750 4200 8400 4200
$Comp
L Device:R R20
U 1 1 5DA4C3F7
P 4900 3000
F 0 "R20" H 4970 3046 50  0000 L CNN
F 1 "10k" H 4970 2955 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 4830 3000 50  0001 C CNN
F 3 "~" H 4900 3000 50  0001 C CNN
F 4 "311-10KGRCT-ND" H -250 -350 50  0001 C CNN "DigiKey Part Number"
	1    4900 3000
	-1   0    0    1   
$EndComp
$Comp
L Device:C C9
U 1 1 5DA4C877
P 4900 3600
F 0 "C9" H 5015 3646 50  0000 L CNN
F 1 "0.1u" H 5015 3555 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 4938 3450 50  0001 C CNN
F 3 "~" H 4900 3600 50  0001 C CNN
F 4 "1276-1258-1-ND" H -250 -350 50  0001 C CNN "DigiKey Part Number"
	1    4900 3600
	1    0    0    -1  
$EndComp
Wire Wire Line
	4900 3300 4900 3450
Wire Wire Line
	4900 3300 4900 3150
Connection ~ 4900 3300
Wire Wire Line
	4900 2850 4900 2550
Text Label 4900 2550 3    50   ~ 0
VDD33A
$Comp
L power:GND #PWR0122
U 1 1 5DA59317
P 4900 3750
F 0 "#PWR0122" H 4900 3500 50  0001 C CNN
F 1 "GND" H 4905 3577 50  0000 C CNN
F 2 "" H 4900 3750 50  0001 C CNN
F 3 "" H 4900 3750 50  0001 C CNN
	1    4900 3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	7450 2900 7900 2900
Wire Wire Line
	7450 3000 7900 3000
Text Label 7900 2900 2    50   ~ 0
TXD0
Text Label 7900 3000 2    50   ~ 0
RXD0
Wire Wire Line
	7450 3100 7900 3100
Text Label 7900 3100 2    50   ~ 0
LNA_IN
Wire Wire Line
	7450 3200 7900 3200
Wire Wire Line
	7450 3300 7900 3300
Wire Wire Line
	7450 3400 7900 3400
Wire Wire Line
	7450 3500 7900 3500
Text Label 7900 3200 2    50   ~ 0
FLASH_CLK
Text Label 7900 3300 2    50   ~ 0
FLASH_SD2
Text Label 7900 3400 2    50   ~ 0
IO10
Text Label 7900 3500 2    50   ~ 0
IO9
Wire Wire Line
	7450 3600 7900 3600
Wire Wire Line
	7450 3700 7900 3700
Text Label 7900 3600 2    50   ~ 0
FLASH_SD1
Text Label 7900 3700 2    50   ~ 0
FLASH_SD3
Wire Wire Line
	7450 3800 7900 3800
Wire Wire Line
	7450 3900 7900 3900
Wire Wire Line
	7450 4000 7900 4000
Text Label 7900 3800 2    50   ~ 0
IO0
Text Label 7900 3900 2    50   ~ 0
IO2
Text Label 7900 4000 2    50   ~ 0
IO4
Wire Wire Line
	7450 4100 7900 4100
Text Label 7900 4100 2    50   ~ 0
IO5
Wire Wire Line
	7450 4300 7900 4300
Wire Wire Line
	7450 4400 7900 4400
Wire Wire Line
	7450 4500 7900 4500
Wire Wire Line
	7450 4600 7900 4600
Text Label 7900 4600 2    50   ~ 0
FLASH_CS
Text Label 7900 4500 2    50   ~ 0
IO15
Text Label 7900 4400 2    50   ~ 0
IO14
Text Label 7900 4300 2    50   ~ 0
IO13
Wire Wire Line
	5850 3400 5400 3400
Wire Wire Line
	5850 3500 5400 3500
Wire Wire Line
	5850 3700 5400 3700
Wire Wire Line
	5850 3800 5400 3800
Text Label 5400 3400 0    50   ~ 0
IO35
Text Label 5400 3500 0    50   ~ 0
IO34
Text Label 5400 3700 0    50   ~ 0
IO33
Text Label 5400 3800 0    50   ~ 0
IO32
Wire Wire Line
	5850 3900 5400 3900
Wire Wire Line
	5850 4000 5400 4000
Text Label 5400 3900 0    50   ~ 0
IO27
Text Label 5400 4000 0    50   ~ 0
IO26
Wire Wire Line
	5850 4100 5400 4100
Text Label 5400 4100 0    50   ~ 0
IO25
Wire Wire Line
	5850 4200 5400 4200
Wire Wire Line
	5850 4300 5400 4300
Text Label 5400 4200 0    50   ~ 0
IO23
Text Label 5400 4300 0    50   ~ 0
IO22
Wire Wire Line
	5850 4400 5400 4400
Text Label 5400 4400 0    50   ~ 0
IO21
Wire Wire Line
	5850 4700 5400 4700
Text Label 5400 4700 0    50   ~ 0
FLASH_SD0
Wire Wire Line
	5850 4600 5400 4600
Wire Wire Line
	5850 4500 5400 4500
Text Label 5400 4500 0    50   ~ 0
IO19
Text Label 5400 4600 0    50   ~ 0
IO18
Text Label 5400 3100 0    50   ~ 0
IO38
Text Label 5400 3000 0    50   ~ 0
IO37
Wire Wire Line
	5850 3200 5200 3200
Text Label 5400 3200 0    50   ~ 0
SENSOR_VN
Wire Wire Line
	5850 2900 5200 2900
Text Label 5400 2900 0    50   ~ 0
SENSOR_VP
Wire Notes Line width 20 style solid
	4600 500  8900 500 
Wire Notes Line width 20 style solid
	8900 500  8900 5950
Wire Notes Line width 20 style solid
	8900 5950 4600 5950
Wire Notes Line width 20 style solid
	4600 5950 4600 500 
Text Notes 7650 5800 0    50   ~ 0
ESP-32-PICO Diagram
NoConn ~ 5400 4700
NoConn ~ 7900 4600
NoConn ~ 7900 3700
NoConn ~ 7900 3600
NoConn ~ 7900 3200
NoConn ~ 7900 3300
Text Label 5200 2900 0    50   ~ 0
IO36
Text Label 5200 3200 0    50   ~ 0
IO39
NoConn ~ 5400 3500
NoConn ~ 5200 3200
$Comp
L Device:L L1
U 1 1 605DB103
P 8050 1350
F 0 "L1" H 8006 1304 50  0000 R CNN
F 1 "1.6nH" H 8006 1395 50  0000 R CNN
F 2 "Inductor_SMD:L_0603_1608Metric" H 8050 1350 50  0001 C CNN
F 3 "~" H 8050 1350 50  0001 C CNN
F 4 "https://www.digikey.com/product-detail/en/murata-electronics/LQW18AS1N6J0ZD/490-15806-6-ND/6801711" H 8050 1350 50  0001 C CNN "Field4"
F 5 "490-15806-1-ND" H -250 -350 50  0001 C CNN "DigiKey Part Number"
	1    8050 1350
	-1   0    0    1   
$EndComp
$Comp
L Device:C C12
U 1 1 605DBA3C
P 7400 1350
F 0 "C12" H 7515 1396 50  0000 L CNN
F 1 "2.7pF" H 7515 1305 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 7438 1200 50  0001 C CNN
F 3 "~" H 7400 1350 50  0001 C CNN
F 4 "311-3854-1-ND" H -250 -350 50  0001 C CNN "DigiKey Part Number"
	1    7400 1350
	1    0    0    -1  
$EndComp
$Comp
L Device:C C13
U 1 1 605DBE3A
P 7750 1050
F 0 "C13" V 7498 1050 50  0000 C CNN
F 1 "4.7pF" V 7589 1050 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 7788 900 50  0001 C CNN
F 3 "~" H 7750 1050 50  0001 C CNN
F 4 "311-1740-1-ND" H -250 -350 50  0001 C CNN "DigiKey Part Number"
	1    7750 1050
	0    1    1    0   
$EndComp
Wire Wire Line
	8050 1200 8050 1050
Wire Wire Line
	8050 1050 7900 1050
Wire Wire Line
	7600 1050 7400 1050
Wire Wire Line
	7400 1050 7400 1200
$Comp
L Connector_Generic:Conn_01x04 J3
U 1 1 6067CAF9
P 6800 1050
F 0 "J3" H 6718 1367 50  0000 C CNN
F 1 "Conn_01x04" H 6718 1276 50  0000 C CNN
F 2 "SmartWatch:ANT_PRO-OB-440" H 6800 1050 50  0001 C CNN
F 3 "http://www.proant.se/files/user/Documents/Datasheets%20and%20appnotes/OnBoard%20SMD/Datasheets/2019-09-25/OnBoard%20SMD%202400%20rev1.2.pdf" H 6800 1050 50  0001 C CNN
F 4 "1532-1001-1-ND" H -250 -350 50  0001 C CNN "DigiKey Part Number"
	1    6800 1050
	-1   0    0    -1  
$EndComp
Wire Wire Line
	7400 1050 7000 1050
Connection ~ 7400 1050
Wire Wire Line
	7400 1500 7400 1600
Wire Wire Line
	7400 1600 8050 1600
Wire Wire Line
	8050 1600 8050 1500
Wire Wire Line
	7400 1600 7150 1600
Wire Wire Line
	7150 1600 7150 950 
Wire Wire Line
	7150 950  7000 950 
Connection ~ 7400 1600
$Comp
L power:GND #PWR0123
U 1 1 60758279
P 7400 1600
F 0 "#PWR0123" H 7400 1350 50  0001 C CNN
F 1 "GND" H 7405 1427 50  0000 C CNN
F 2 "" H 7400 1600 50  0001 C CNN
F 3 "" H 7400 1600 50  0001 C CNN
	1    7400 1600
	1    0    0    -1  
$EndComp
Wire Wire Line
	8050 1050 8200 1050
Connection ~ 8050 1050
Text Label 8200 1050 2    50   ~ 0
LNA_IN
NoConn ~ 7000 1150
NoConn ~ 7000 1250
Text HLabel 8200 2300 2    50   Input ~ 0
3.3V
Text HLabel 7900 3400 2    50   Input ~ 0
IO10
Text HLabel 7900 3500 2    50   Input ~ 0
IO9
Text HLabel 7900 4000 2    50   Input ~ 0
IO4
Text HLabel 7900 4100 2    50   Input ~ 0
IO5
Text HLabel 7900 4300 2    50   Input ~ 0
IO13
Text HLabel 7900 4400 2    50   Input ~ 0
IO14
Text HLabel 5200 2900 0    50   Input ~ 0
IO36
Text HLabel 5200 3000 0    50   Input ~ 0
IO36
Text HLabel 5200 3100 0    50   Input ~ 0
IO38
Text HLabel 5400 3400 0    50   Input ~ 0
IO35
Text HLabel 5400 3700 0    50   Input ~ 0
IO33
Text HLabel 5400 3800 0    50   Input ~ 0
IO32
Text HLabel 5400 3900 0    50   Input ~ 0
IO27
Text HLabel 5400 4000 0    50   Input ~ 0
IO26
Text HLabel 5400 4100 0    50   Input ~ 0
IO25
Text HLabel 5400 4200 0    50   Input ~ 0
IO23
Text HLabel 5400 4300 0    50   Input ~ 0
IO22
Text HLabel 5400 4400 0    50   Input ~ 0
IO21
Text HLabel 5400 4500 0    50   Input ~ 0
IO19
Text HLabel 5400 4600 0    50   Input ~ 0
IO18
Wire Wire Line
	4900 3300 5850 3300
$Comp
L Transistor_BJT:MMBT5551L Q4
U 1 1 5DC58515
P 3900 3150
F 0 "Q4" H 4091 3196 50  0000 L CNN
F 1 "MMBT5551L" H 4091 3105 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 4100 3075 50  0001 L CIN
F 3 "www.onsemi.com/pub/Collateral/MMBT5550LT1-D.PDF" H 3900 3150 50  0001 L CNN
F 4 "MMBT5551LT1GOSCT-ND" H 0   0   50  0001 C CNN "DigiKey Part Number"
	1    3900 3150
	1    0    0    1   
$EndComp
$Comp
L Transistor_BJT:MMBT5551L Q2
U 1 1 5DC806E2
P 3900 2350
F 0 "Q2" H 4091 2396 50  0000 L CNN
F 1 "MMBT5551L" H 4091 2305 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 4100 2275 50  0001 L CIN
F 3 "www.onsemi.com/pub/Collateral/MMBT5550LT1-D.PDF" H 3900 2350 50  0001 L CNN
F 4 "MMBT5551LT1GOSCT-ND" H 0   0   50  0001 C CNN "DigiKey Part Number"
	1    3900 2350
	1    0    0    -1  
$EndComp
Wire Wire Line
	2500 750  2500 900 
Text HLabel 2500 750  2    50   Input ~ 0
VUSB
Wire Wire Line
	2600 7300 2850 7300
Wire Wire Line
	750  7300 1000 7300
Wire Wire Line
	750  7150 750  7300
Wire Wire Line
	2600 7150 2600 7300
$Comp
L Device:D_Schottky D2
U 1 1 5D9D1A20
P 2100 900
F 0 "D2" H 2100 684 50  0000 C CNN
F 1 "FSV340FP" H 2100 775 50  0000 C CNN
F 2 "Diode_SMD:D_SOD-123F" H 2100 900 50  0001 C CNN
F 3 "https://www.onsemi.com/pub/Collateral/FSV360FP-D.PDF" H 2100 900 50  0001 C CNN
F 4 "FSV340FPCT-ND" H 0   0   50  0001 C CNN "DigiKey Part Number"
	1    2100 900 
	-1   0    0    1   
$EndComp
Wire Notes Line width 20 style solid
	550  6300 550  500 
$Comp
L SmartWatch-rescue:10118192-0001LF J1
U 1 1 5DD404D8
P 1250 1300
F 0 "J1" H 1306 1865 50  0000 C CNN
F 1 "10118192-0001LF" H 1306 1774 50  0000 C CNN
F 2 "SmartWatch:AMPHENOL_10118192-0001LF" H 1250 2300 50  0001 L BNN
F 3 "Amphenol ICC" H 1250 2200 50  0001 L BNN
F 4 "None" H 1250 2450 50  0001 L BNN "Field4"
F 5 "Single Port 5 Contact Horizontal Right Angle Shielded Micro B USB 2.0 Connector" H 1250 2100 50  0001 L BNN "Field5"
F 6 "609-4613-1-ND" H 1250 2000 50  0001 L BNN "Field6"
F 7 "10118192-0001LF" H 1250 1900 50  0001 L BNN "Field7"
F 8 "https://www.digikey.com/product-detail/en/amphenol-icc-fci/10118192-0001LF/609-4613-1-ND/2785378?utm_source=snapeda&utm_medium=aggregator&utm_campaign=symbol" H 1250 1800 50  0001 L BNN "Field8"
F 9 "609-4613-1-ND" H 0   0   50  0001 C CNN "DigiKey Part Number"
	1    1250 1300
	1    0    0    -1  
$EndComp
NoConn ~ 1550 1600
Wire Wire Line
	1550 1400 1600 1400
Wire Wire Line
	1550 1200 2300 1200
Wire Wire Line
	1550 1100 2100 1100
Wire Wire Line
	1550 1000 1700 1000
Wire Wire Line
	1700 1000 1700 900 
$Comp
L SmartWatch-rescue:TL3305AF260QG SW4
U 1 1 5DD92C10
P 1150 7050
F 0 "SW4" H 1150 7328 50  0000 C CNN
F 1 "TL3305AF260QG" H 1150 7237 50  0000 C CNN
F 2 "SmartWatch:SW_TL3305AF260QG" H 400 6750 50  0001 L BNN
F 3 "E-Switch" H 400 6450 50  0001 L BNN
F 4 "LQFP-48 E-Switch" H 400 6650 50  0001 L BNN "Field4"
F 5 "Switch Tactile Spst-No 50ma 12v" H 400 6150 50  0001 L BNN "Field5"
F 6 "EG5353CT-ND" H 400 6350 50  0001 L BNN "Field6"
F 7 "TL3305AF260QG" H 400 6550 50  0001 L BNN "Field7"
F 8 "https://www.digikey.com/product-detail/en/e-switch/TL3305AF260QG/EG5353CT-ND/5816198?utm_source=snapeda&utm_medium=aggregator&utm_campaign=symbol" H 400 6250 50  0001 L BNN "Field8"
F 9 "EG5353CT-ND" H 0   0   50  0001 C CNN "DigiKey Part Number"
	1    1150 7050
	1    0    0    -1  
$EndComp
$Comp
L SmartWatch-rescue:TL3305AF260QG SW5
U 1 1 5DD92CBE
P 3000 7050
F 0 "SW5" H 3000 7328 50  0000 C CNN
F 1 "TL3305AF260QG" H 3000 7237 50  0000 C CNN
F 2 "SmartWatch:SW_TL3305AF260QG" H 2250 6750 50  0001 L BNN
F 3 "E-Switch" H 2250 6450 50  0001 L BNN
F 4 "LQFP-48 E-Switch" H 2250 6650 50  0001 L BNN "Field4"
F 5 "Switch Tactile Spst-No 50ma 12v" H 2250 6150 50  0001 L BNN "Field5"
F 6 "EG5353CT-ND" H 2250 6350 50  0001 L BNN "Field6"
F 7 "TL3305AF260QG" H 2250 6550 50  0001 L BNN "Field7"
F 8 "https://www.digikey.com/product-detail/en/e-switch/TL3305AF260QG/EG5353CT-ND/5816198?utm_source=snapeda&utm_medium=aggregator&utm_campaign=symbol" H 2250 6250 50  0001 L BNN "Field8"
F 9 "EG5353CT-ND" H 0   0   50  0001 C CNN "DigiKey Part Number"
	1    3000 7050
	1    0    0    -1  
$EndComp
Wire Wire Line
	4750 850  5700 850 
NoConn ~ 2600 6950
NoConn ~ 750  6950
Text Label 2300 900  0    50   ~ 0
USB5V
Wire Wire Line
	2100 1100 3100 1100
Wire Wire Line
	2300 1200 3100 1200
Wire Wire Line
	7450 4200 8100 4200
Wire Wire Line
	5200 3000 5850 3000
Wire Wire Line
	5200 3100 5850 3100
Text Label 5000 3300 0    50   ~ 0
EN
Text Notes 4600 6450 0    50   ~ 0
Note 1: \nConfigure CBUS0 to BCD Charger, this will prevent the watch\nfrom charging off of a standard PC USB port but will remove \nany interference in communication that may result.
Text HLabel 3450 5050 2    50   Input ~ 0
Charge_Enable
Wire Wire Line
	3250 5050 3450 5050
$Comp
L Device:C C4
U 1 1 5DE23027
P 1200 3950
F 0 "C4" H 1315 3996 50  0000 L CNN
F 1 "0.1uF" H 1350 3900 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 1238 3800 50  0001 C CNN
F 3 "~" H 1200 3950 50  0001 C CNN
F 4 "1276-1258-1-ND" H -1500 -50 50  0001 C CNN "DigiKey Part Number"
	1    1200 3950
	1    0    0    -1  
$EndComp
$Comp
L Device:C C14
U 1 1 5E16885C
P 1300 4950
F 0 "C14" H 1500 5000 50  0000 L CNN
F 1 "47p" H 1500 4900 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 1338 4800 50  0001 C CNN
F 3 "~" H 1300 4950 50  0001 C CNN
F 4 "311-1065-1-ND" H -5050 3500 50  0001 C CNN "DigiKey Part Number"
	1    1300 4950
	1    0    0    -1  
$EndComp
Wire Wire Line
	1200 5200 1300 5200
$Comp
L power:GND #PWR0104
U 1 1 5E16862B
P 1200 5200
F 0 "#PWR0104" H 1200 4950 50  0001 C CNN
F 1 "GND" H 1205 5027 50  0000 C CNN
F 2 "" H 1200 5200 50  0001 C CNN
F 3 "" H 1200 5200 50  0001 C CNN
	1    1200 5200
	1    0    0    -1  
$EndComp
Wire Wire Line
	1300 5200 1300 5100
Connection ~ 1200 5200
Wire Wire Line
	1050 5200 1200 5200
Wire Wire Line
	1050 5100 1050 5200
Wire Wire Line
	1300 4550 1350 4550
Wire Wire Line
	1300 4800 1300 4550
Wire Wire Line
	1050 4450 1350 4450
Wire Wire Line
	1050 4800 1050 4450
$Comp
L Device:C C8
U 1 1 5E153113
P 1050 4950
F 0 "C8" H 850 5000 50  0000 L CNN
F 1 "47p" H 800 4900 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 1088 4800 50  0001 C CNN
F 3 "~" H 1050 4950 50  0001 C CNN
F 4 "311-1065-1-ND" H -5300 3500 50  0001 C CNN "DigiKey Part Number"
	1    1050 4950
	1    0    0    -1  
$EndComp
Connection ~ 1300 4550
Wire Wire Line
	700  4550 1300 4550
Connection ~ 1050 4450
Wire Wire Line
	700  4450 1050 4450
Text Label 4100 4550 2    50   ~ 0
DTR
Wire Wire Line
	3250 4550 4100 4550
Text Label 700  4550 0    50   ~ 0
USB_DP
Text Label 700  4450 0    50   ~ 0
USB_DN
Text Label 4100 4450 2    50   ~ 0
CTS
Text Label 4100 4350 2    50   ~ 0
RTS
Text Label 4100 4250 2    50   ~ 0
RXD
Text Label 4100 4150 2    50   ~ 0
TXD
Wire Wire Line
	3250 4450 4100 4450
Wire Wire Line
	3250 4350 4100 4350
Wire Wire Line
	4100 4250 3250 4250
Wire Wire Line
	3250 4150 4100 4150
$Comp
L power:GND #PWR08
U 1 1 5DE233AB
P 1200 4100
F 0 "#PWR08" H 1200 3850 50  0001 C CNN
F 1 "GND" H 1205 3927 50  0000 C CNN
F 2 "" H 1200 4100 50  0001 C CNN
F 3 "" H 1200 4100 50  0001 C CNN
	1    1200 4100
	1    0    0    -1  
$EndComp
$Comp
L Device:R R10
U 1 1 5D9D6BDB
P 1500 4550
F 0 "R10" V 1300 4550 50  0000 C CNN
F 1 "27" V 1400 4550 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 1430 4550 50  0001 C CNN
F 3 "~" H 1500 4550 50  0001 C CNN
F 4 "311-27.0HRCT-ND" H -1350 3350 50  0001 C CNN "DigiKey Part Number"
	1    1500 4550
	0    1    -1   0   
$EndComp
$Comp
L Device:R R9
U 1 1 5D9D5414
P 1500 4450
F 0 "R9" V 1707 4450 50  0000 C CNN
F 1 "27" V 1616 4450 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 1430 4450 50  0001 C CNN
F 3 "~" H 1500 4450 50  0001 C CNN
F 4 "311-27.0HRCT-ND" H -1100 3350 50  0001 C CNN "DigiKey Part Number"
	1    1500 4450
	0    -1   -1   0   
$EndComp
$Comp
L Interface_USB:FT231XS U?
U 1 1 5E1CC8A7
P 2550 4750
F 0 "U?" H 1800 5950 50  0000 C CNN
F 1 "FT231XS" H 1900 5850 50  0000 C CNN
F 2 "Package_SO:SSOP-20_3.9x8.7mm_P0.635mm" H 3150 3800 50  0001 C CNN
F 3 "http://www.ftdichip.com/Products/ICs/FT231X.html" H 2550 4750 50  0001 C CNN
F 4 "768-1129-1-ND" H 2550 4750 50  0001 C CNN "digikey"
	1    2550 4750
	1    0    0    -1  
$EndComp
Wire Wire Line
	1650 4450 1850 4450
Wire Wire Line
	1650 4550 1850 4550
Wire Wire Line
	2450 3850 2450 3650
$Comp
L power:+3V3 #PWR?
U 1 1 5E22AAE7
P 2450 3650
F 0 "#PWR?" H 2450 3500 50  0001 C CNN
F 1 "+3V3" H 2465 3823 50  0000 C CNN
F 2 "" H 2450 3650 50  0001 C CNN
F 3 "" H 2450 3650 50  0001 C CNN
	1    2450 3650
	1    0    0    -1  
$EndComp
Wire Wire Line
	1850 4150 1700 4150
Wire Wire Line
	1700 4150 1700 4750
Wire Wire Line
	1700 4750 1850 4750
Wire Wire Line
	1700 4150 1700 3800
Wire Wire Line
	1700 3800 2650 3800
Wire Wire Line
	2650 3800 2650 3850
Connection ~ 1700 4150
Wire Wire Line
	1200 3800 1700 3800
Connection ~ 1700 3800
$Comp
L power:GND #PWR?
U 1 1 5E253BEB
P 2550 5700
F 0 "#PWR?" H 2550 5450 50  0001 C CNN
F 1 "GND" H 2555 5527 50  0000 C CNN
F 2 "" H 2550 5700 50  0001 C CNN
F 3 "" H 2550 5700 50  0001 C CNN
	1    2550 5700
	1    0    0    -1  
$EndComp
Wire Wire Line
	2450 5650 2450 5700
Wire Wire Line
	2450 5700 2550 5700
Wire Wire Line
	2650 5650 2650 5700
Wire Wire Line
	2650 5700 2550 5700
Connection ~ 2550 5700
$Comp
L Device:R R?
U 1 1 5E262DE4
P 4300 5300
F 0 "R?" H 4370 5346 50  0000 L CNN
F 1 "10k" H 4370 5255 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 4230 5300 50  0001 C CNN
F 3 "~" H 4300 5300 50  0001 C CNN
F 4 "311-10KGRCT-ND" H -850 1950 50  0001 C CNN "DigiKey Part Number"
	1    4300 5300
	-1   0    0    1   
$EndComp
$Comp
L Device:R R?
U 1 1 5E262F34
P 4300 5000
F 0 "R?" H 4370 5046 50  0000 L CNN
F 1 "47k" H 4370 4955 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 4230 5000 50  0001 C CNN
F 3 "~" H 4300 5000 50  0001 C CNN
F 4 "311-47GRCT-ND" H -850 1650 50  0001 C CNN "DigiKey Part Number"
	1    4300 5000
	-1   0    0    1   
$EndComp
Text Label 4050 4800 0    50   ~ 0
USB5V
Wire Wire Line
	4050 4800 4300 4800
Wire Wire Line
	4300 4800 4300 4850
$Comp
L power:GND #PWR?
U 1 1 5E26B30E
P 4300 5450
F 0 "#PWR?" H 4300 5200 50  0001 C CNN
F 1 "GND" H 4305 5277 50  0000 C CNN
F 2 "" H 4300 5450 50  0001 C CNN
F 3 "" H 4300 5450 50  0001 C CNN
	1    4300 5450
	1    0    0    -1  
$EndComp
Wire Wire Line
	3250 5150 4300 5150
Connection ~ 4300 5150
Text Notes 3500 5000 0    50   ~ 0
See Note 1
NoConn ~ 3250 4650
NoConn ~ 3250 4750
NoConn ~ 3250 4850
$EndSCHEMATC
