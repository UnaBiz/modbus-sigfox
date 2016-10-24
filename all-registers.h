//  COPY THE CONTENTS FROM https://docs.google.com/spreadsheets/d/1Spf1qlaAH2hzwa2MvuDNv2pzeSAhlU451IRfEV6stMI/edit?usp=sharing
#ifndef ALL_REGISTERS_H
#define ALL_REGISTERS_H

#include <Arduino.h>
#include "Flash/src/Flash.h"

FLASH_TABLE(uint16_t, all_registers, 6,  //  Generated list of registers based on Modbus table.
{ 0, 4098, 8, 2282, 23, 7 },  //  TC list :TC 1 / TC2 / TC3 / TC4 / TC5 /  / A

{ 0, 57346, 1, 609, 23, 4 },  //  Current Transformer primary / A

{ 0, 36353, 1, 637, 23, 8 },  //  Current Transformer secondary : 1: 1 A 5 / A
{ 0, 36354, 1, 609, 23, 8 },  //  Current Transformer primary / A

{ 0, 50512, 2, 768, 36, 6 },  //  Hour Meter / h / 100
{ 0, 50514, 2, 1312, 53, 6 },  //  Phase to Phase Voltage: U12 / V / 100
{ 0, 50516, 2, 1340, 53, 6 },  //  Phase to Phase Voltage: U23 / V / 100
{ 0, 50518, 2, 1368, 53, 6 },  //  Phase to Phase Voltage: U31 / V / 100
{ 1, 50520, 2, 2222, 53, 6 },  //  Simple voltage : V1 / V / 100
{ 0, 50522, 2, 2242, 53, 6 },  //  Simple voltage : V2 / V / 100
{ 0, 50524, 2, 2262, 53, 6 },  //  Simple voltage : V3 / V / 100
{ 1, 50526, 2, 739, 44, 6 },  //  Frequency : F / Hz / 100
{ 0, 50528, 2, 488, 25, 6 },  //  Current : I1 / A / 1000
{ 0, 50530, 2, 501, 25, 6 },  //  Current : I2 / A / 1000
{ 0, 50532, 2, 514, 25, 6 },  //  Current : I3 / A / 1000
{ 0, 50534, 2, 1027, 25, 6 },  //  Neutral Current : In / A / 1000
{ 0, 50536, 2, 75, 93, 3 },  //  Active Power +/- : P / W / 0.1
{ 0, 50538, 2, 2008, 70, 3 },  //  Reactive Power +/- : Q / var / 0.1
{ 0, 50540, 2, 258, 61, 6 },  //  Apparent Power : S / VA / 0.1
{ 0, 50542, 2, 1519, 0, 3 },  //  Power Factor : -: leading et + : lagging / - / 1000
{ 0, 50544, 2, 96, 93, 3 },  //  Active Power phase 1 +/- : P1 / W / 0.1
{ 0, 50546, 2, 126, 93, 3 },  //  Active Power phase 2 +/- : P2 / W / 0.1
{ 0, 50548, 2, 156, 93, 3 },  //  Active Power phase 3 +/- : P3 / W / 0.1
{ 0, 50550, 2, 2031, 70, 3 },  //  Reactive Power phase 1 +/- : Q1 / var / 0.1
{ 0, 50552, 2, 2063, 70, 3 },  //  Reactive Power phase 2 +/- : Q2 / var / 0.1
{ 0, 50554, 2, 2095, 70, 3 },  //  Reactive Power phase 3 +/- : Q3 / var / 0.1
{ 0, 50556, 2, 277, 61, 6 },  //  Apparent Power phase 1 : S1 / VA / 0.1
{ 0, 50558, 2, 305, 61, 6 },  //  Apparent Power phase 2 : S2 / VA / 0.1
{ 0, 50560, 2, 333, 61, 6 },  //  Apparent Power phase 3 : S3 / VA / 0.1
{ 0, 50562, 2, 1560, 0, 3 },  //  Power Factor phase 1 -: leading and + :  / - / 1000
{ 0, 50564, 2, 1683, 0, 3 },  //  Power Factor phase 2 -: leading and + :  / - / 1000
{ 0, 50566, 2, 1806, 0, 3 },  //  Power Factor phase 3 -: leading and + :  / - / 1000

{ 0, 50768, 2, 768, 36, 6 },  //  Hour meter / h / 100

{ 0, 50780, 2, 1090, 101, 6 },  //  Partial Positive Active Energy: Ea+ / Wh / 0.001
{ 0, 50782, 2, 1126, 80, 6 },  //  Partial Positive Reactive Energy: Er + / varh / 0.001

{ 0, 51070, 2, 910, 25, 6 },  //  Max/avg I1 / A / 1000
{ 0, 51072, 2, 921, 25, 6 },  //  Max/avg I2 / A / 1000
{ 0, 51074, 2, 932, 25, 6 },  //  Max/avg I3 / A / 1000
{ 0, 51076, 2, 943, 25, 6 },  //  Max/avg In / A / 1000
{ 0, 51078, 2, 954, 93, 6 },  //  Max/avg P+ / W / 0.1

{ 0, 51082, 2, 965, 70, 6 },  //  Max/avg Q+ / var / 0.1

{ 0, 51086, 2, 1001, 61, 6 },  //  Max/avg S / VA / 0.1

{ 0, 51280, 1, 768, 34, 4 },  //  Hour Meter / h
{ 0, 51281, 1, 1312, 53, 4 },  //  Phase to Phase Voltage: U12 / V / 100
{ 0, 51282, 1, 1340, 53, 4 },  //  Phase to Phase Voltage: U23 / V / 100
{ 0, 51283, 1, 1368, 53, 4 },  //  Phase to Phase Voltage: U31 / V / 100
{ 0, 51284, 1, 2222, 53, 4 },  //  Simple voltage : V1 / V / 100
{ 0, 51285, 1, 2242, 53, 4 },  //  Simple voltage : V2 / V / 100
{ 0, 51286, 1, 2262, 53, 4 },  //  Simple voltage : V3 / V / 100
{ 0, 51287, 1, 739, 44, 4 },  //  Frequency : F / Hz / 100
{ 0, 51288, 1, 488, 25, 4 },  //  Current : I1 / A / 1000
{ 0, 51289, 1, 501, 25, 4 },  //  Current : I2 / A / 1000
{ 0, 51290, 1, 514, 25, 4 },  //  Current : I3 / A / 1000
{ 0, 51291, 1, 1027, 25, 4 },  //  Neutral Current : In / A / 1000
{ 0, 51292, 1, 75, 93, 2 },  //  active Power +/- : P / W / 0.1
{ 0, 51293, 1, 2008, 70, 2 },  //  reactive Power +/- : Q / var / 0.1
{ 0, 51294, 1, 258, 61, 4 },  //  apparent power : S / VA / 0.1
{ 0, 51295, 1, 1478, 0, 2 },  //  power factor : -: leading and + : laggin / - / 1000
{ 0, 51296, 1, 96, 93, 2 },  //  Active Power phase 1 +/- : P1 / W / 0.1
{ 0, 51297, 1, 126, 93, 2 },  //  Active Power phase 2 +/- : P2 / W / 0.1
{ 0, 51298, 1, 156, 93, 2 },  //  Active Power phase 3 +/- : P3 / W / 0.1
{ 0, 51299, 1, 2031, 70, 2 },  //  Reactive Power phase 1 +/- : Q1 / var / 0.1
{ 0, 51300, 1, 2063, 70, 2 },  //  Reactive Power phase 2 +/- : Q2 / var / 0.1
{ 0, 51301, 1, 2095, 70, 2 },  //  Reactive Power phase 3 +/- : Q3 / var / 0.1
{ 0, 51302, 1, 277, 61, 4 },  //  Apparent power phase 1 : S1 / VA / 0.1
{ 0, 51303, 1, 305, 61, 4 },  //  Apparent power phase 2 : S2 / VA / 0.1
{ 0, 51304, 1, 333, 61, 4 },  //  Apparent power phase 3 : S3 / VA / 0.1
{ 0, 51305, 1, 1560, 0, 2 },  //  Power Factor phase 1 -: leading and + :  / - / 1000
{ 0, 51306, 1, 1683, 0, 2 },  //  Power Factor phase 2 -: leading and + :  / - / 1000
{ 0, 51307, 1, 1806, 0, 2 },  //  Power Factor phase 3 -: leading and + :  / - / 1000

{ 0, 51311, 1, 2482, 112, 4 },  //  Total Positive Active Energy (no resetab / Wh / 1E-06

{ 0, 51313, 1, 2441, 112, 4 },  //  Total Negative Active Energy (no resetab / Wh / 1E-06

{ 0, 51536, 1, 2344, 16, 4 },  //  thd U12 / % / 10
{ 0, 51537, 1, 2352, 16, 4 },  //  thd U23 / % / 10
{ 0, 51538, 1, 2360, 16, 4 },  //  thd U31 / % / 10
{ 0, 51539, 1, 2368, 16, 4 },  //  thd V1 / % / 10
{ 0, 51540, 1, 2375, 16, 4 },  //  thd V2 / % / 10
{ 0, 51541, 1, 2382, 16, 4 },  //  thd V3 / % / 10
{ 0, 51542, 1, 2323, 16, 4 },  //  thd I1 / % / 10
{ 0, 51543, 1, 2330, 16, 4 },  //  thd I2 / % / 10
{ 0, 51544, 1, 2337, 16, 4 },  //  thd I3 / % / 10

{ 0, 768, 2, 1165, 25, 6 },  //  Phase 1 Current / A / 1000
{ 0, 770, 2, 1181, 25, 6 },  //  Phase 2 Current / A / 1000
{ 0, 772, 2, 1197, 25, 6 },  //  Phase 3 Current / A / 1000
{ 0, 774, 2, 1011, 25, 6 },  //  Neutral Current / A / 1000
{ 0, 776, 2, 1312, 53, 6 },  //  Phase to Phase Voltage: U12 / V / 100
{ 0, 778, 2, 1340, 53, 6 },  //  Phase to Phase Voltage: U23 / V / 100
{ 0, 780, 2, 1368, 53, 6 },  //  Phase to Phase Voltage: U31 / V / 100
{ 0, 782, 2, 1213, 53, 6 },  //  Phase to Neutral voltage phase 1 / V / 100
{ 0, 784, 2, 1246, 53, 6 },  //  Phase to Neutral voltage phase 2 / V / 100
{ 0, 786, 2, 1279, 53, 6 },  //  Phase to Neutral voltage phase 3 / V / 100
{ 0, 788, 2, 729, 44, 6 },  //  Frequency / Hz / 100
{ 0, 790, 2, 75, 93, 3 },  //  active power +/- : P / W / 0.1
{ 0, 792, 2, 2008, 70, 3 },  //  reactive power +/- : Q / var / 0.1
{ 0, 794, 2, 258, 61, 6 },  //  apparent power : S / VA / 0.1
{ 0, 796, 2, 1437, 0, 3 },  //  power factor : -: leadiing et + : laggin / - / 1000
{ 0, 798, 2, 186, 93, 3 },  //  Active Power phase1 +/- / W / 0.1
{ 0, 800, 2, 210, 93, 3 },  //  Active Power phase2 +/- / W / 0.1
{ 0, 802, 2, 234, 93, 3 },  //  Active Power phase3 +/- / W / 0.1
{ 0, 804, 2, 2127, 70, 3 },  //  Reactive Power phase1 +/- / var / 0.1
{ 0, 806, 2, 2153, 70, 3 },  //  Reactive Power phase2 +/- / var / 0.1
{ 0, 808, 2, 2179, 70, 3 },  //  Reactive Power phase3 +/- / var / 0.1
{ 0, 810, 2, 361, 61, 6 },  //  Apparent Power phase1 / VA / 0.1
{ 0, 812, 2, 383, 61, 6 },  //  Apparent Power phase2 / VA / 0.1
{ 0, 814, 2, 405, 61, 6 },  //  Apparent Power phase3 / VA / 0.1
{ 0, 816, 2, 1601, 0, 3 },  //  Power factor phase 1 -:leading and +: la / - / 1000
{ 0, 818, 2, 1724, 0, 3 },  //  Power factor phase 2 -:leading and +: la / - / 1000
{ 0, 820, 2, 1847, 0, 3 },  //  Power factor phase 3 -:leading and +: la / - / 1000

{ 0, 838, 2, 910, 25, 6 },  //  max/avg I1 / A / 1000
{ 0, 840, 2, 921, 25, 6 },  //  max/avg I2 / A / 1000
{ 0, 842, 2, 932, 25, 6 },  //  max/avg I3 / A / 1000
{ 0, 844, 2, 864, 93, 6 },  //  max/avg active power + / W / 0.1

{ 0, 848, 2, 976, 70, 6 },  //  max/avg reactive power + / var / 0.1

{ 0, 852, 2, 887, 61, 6 },  //  max/avg apparent power / VA / 0.1
{ 0, 854, 2, 768, 36, 6 },  //  Hour Meter / h / 100
{ 0, 856, 2, 13, 101, 6 },  //  Active Energy + / Wh / 0.001
{ 0, 858, 2, 1940, 80, 6 },  //  Reactive Energy + / varh / 0.001

{ 0, 1792, 1, 1165, 25, 4 },  //  Phase 1 Current / A / 1000
{ 0, 1793, 1, 1181, 25, 4 },  //  Phase 2 Current / A / 1000
{ 0, 1794, 1, 1197, 25, 4 },  //  Phase 3 Current / A / 1000
{ 0, 1795, 1, 1011, 25, 4 },  //  Neutral Current / A / 1000
{ 0, 1796, 1, 1312, 53, 4 },  //  Phase to Phase Voltage: U12 / V / 100
{ 0, 1797, 1, 1340, 53, 4 },  //  Phase to Phase Voltage: U23 / V / 100
{ 0, 1798, 1, 1368, 53, 4 },  //  Phase to Phase Voltage: U31 / V / 100
{ 0, 1799, 1, 1213, 53, 4 },  //  Phase to Neutral voltage phase 1 / V / 100
{ 0, 1800, 1, 1246, 53, 4 },  //  Phase to Neutral voltage phase 2 / V / 100
{ 0, 1801, 1, 1279, 53, 4 },  //  Phase to Neutral voltage phase 3 / V / 100
{ 0, 1802, 1, 729, 44, 4 },  //  Frequency / Hz / 100
{ 0, 1803, 1, 75, 93, 2 },  //  active power +/- : P / W / 0.1
{ 0, 1804, 1, 2008, 70, 2 },  //  reactive power +/- : Q / var / 0.1
{ 0, 1805, 1, 258, 61, 4 },  //  apparent power : S / VA / 0.1
{ 0, 1806, 1, 1437, 0, 2 },  //  power factor : -: leadiing et + : laggin / - / 1000
{ 0, 1807, 1, 186, 93, 2 },  //  Active Power phase1 +/- / W / 0.1
{ 0, 1808, 1, 210, 93, 2 },  //  Active Power phase2 +/- / W / 0.1
{ 0, 1809, 1, 234, 93, 2 },  //  Active Power phase3 +/- / W / 0.1
{ 0, 1810, 1, 2127, 70, 2 },  //  Reactive Power phase1 +/- / var / 0.1
{ 0, 1811, 1, 2153, 70, 2 },  //  Reactive Power phase2 +/- / var / 0.1
{ 0, 1812, 1, 2179, 70, 2 },  //  Reactive Power phase3 +/- / var / 0.1
{ 0, 1813, 1, 361, 61, 4 },  //  Apparent Power phase1 / VA / 0.1
{ 0, 1814, 1, 383, 61, 4 },  //  Apparent Power phase2 / VA / 0.1
{ 0, 1815, 1, 405, 61, 4 },  //  Apparent Power phase3 / VA / 0.1
{ 0, 1816, 1, 1601, 0, 2 },  //  Power factor phase 1 -:leading and +: la / - / 1000
{ 0, 1817, 1, 1724, 0, 2 },  //  Power factor phase 2 -:leading and +: la / - / 1000
{ 0, 1818, 1, 1847, 0, 2 },  //  Power factor phase 3 -:leading and +: la / - / 1000

{ 0, 1827, 1, 910, 25, 4 },  //  max/avg I1 / A / 1000
{ 0, 1828, 1, 921, 25, 4 },  //  max/avg I2 / A / 1000
{ 0, 1829, 1, 932, 25, 4 },  //  max/avg I3 / A / 1000
{ 0, 1830, 1, 864, 93, 4 },  //  max/avg active power + / W / 0.1

{ 0, 1832, 1, 976, 70, 4 },  //  max/avg reactive power + / var / 0.1

{ 0, 1834, 1, 887, 61, 4 },  //  max/avg apparent power / VA / 0.1
{ 0, 1835, 1, 29, 101, 4 },  //  Active Energy +<10 000 / Wh / 0.001
{ 0, 1836, 1, 52, 101, 4 },  //  Active Energy +>10 000 / Wh / 0.001
{ 0, 1837, 1, 1983, 80, 4 },  //  Reactive Energy+< 10 000 / varh / 0.001
{ 0, 1838, 1, 1958, 80, 4 },  //  Reactive Energy +>10 000 / varh / 0.001

{ 0, 36176, 1, 527, 0, 8 },  //  Current alarm on lower threshold cause:0 / - / 1000

{ 0, 36179, 1, 568, 0, 8 },  //  Current alarm on upper threshold cause:0 / - / 1000
{ 0, 36820, 1, 0, 23, 8 },  //  Action Calib / A
{ 0, 36821, 1, 2205, 23, 8 },  //  Set Relais Impul / A
{ 0, 36822, 1, 427, 23, 8 },  //  Clr Relais Impulsion / A

{ 0, 36825, 4, 448, 23, 5 },  //  Coef Gnrale Courant / A
{ 0, 36829, 4, 468, 23, 5 },  //  Coef Gnrale Tension / A
{ 0, 36833, 4, 678, 23, 5 },  //  Dphasage Phase 1 / A
{ 0, 36837, 4, 695, 23, 5 },  //  Dphasage Phase 2 / A
{ 0, 36841, 4, 712, 23, 5 },  //  Dphasage Phase 3 / A

{ 0, 36385, 1, 1048, 9, 8 },  //  Offset Int/Ext ( default : 0 ) / ? / 10

{ 0, 2816, 1, 488, 25, 4 },  //  Current : I1 / A / 1000
{ 0, 2817, 1, 501, 25, 4 },  //  Current : I2 / A / 1000
{ 0, 2818, 1, 514, 25, 4 },  //  Current : I3 / A / 1000
{ 0, 2819, 1, 1027, 25, 4 },  //  Neutral Current : In / A / 1000
{ 0, 2820, 1, 1312, 53, 4 },  //  Phase to Phase Voltage: U12 / V / 100
{ 0, 2821, 1, 1340, 53, 4 },  //  Phase to Phase Voltage: U23 / V / 100
{ 0, 2822, 1, 1368, 53, 4 },  //  Phase to Phase Voltage: U31 / V / 100
{ 0, 2823, 1, 2222, 53, 4 },  //  Simple voltage : V1 / V / 100
{ 0, 2824, 1, 2242, 53, 4 },  //  Simple voltage : V2 / V / 100
{ 0, 2825, 1, 2262, 53, 4 },  //  Simple voltage : V3 / V / 100
{ 0, 2826, 1, 753, 44, 4 },  //  Frequency : Fr / Hz / 100
{ 0, 2827, 1, 2389, 93, 2 },  //  Total Active Power +/- : P / W / 0.1
{ 0, 2828, 1, 2523, 70, 2 },  //  Total Reactive Power +/- : Q / var / 0.1
{ 0, 2829, 1, 2416, 61, 4 },  //  Total apparent power : S / VA / 0.1
{ 0, 2830, 1, 1396, 0, 2 },  //  Power factor : -: capacitif et + : induc / - / 1000
{ 0, 2831, 1, 817, 25, 4 },  //  I 1 AVG MAX / A / 1000
{ 0, 2832, 1, 829, 25, 4 },  //  I 2 AVG MAX / A / 1000
{ 0, 2833, 1, 841, 25, 4 },  //  I 3 AVG MAX / A / 1000
{ 0, 2834, 1, 853, 25, 4 },  //  In AVG MAX / A / 1000
{ 0, 2835, 1, 1079, 93, 4 },  //  P+ AVG MAX / W / 0.1
{ 0, 2836, 1, 29, 101, 4 },  //  Active Energy +<10 000 / Wh / 0.001
{ 0, 2837, 1, 52, 101, 4 },  //  Active Energy +>10 000 / Wh / 0.001
{ 0, 2838, 1, 1983, 80, 4 },  //  Reactive Energy+< 10 000 / varh / 0.001
{ 0, 2839, 1, 1958, 80, 4 },  //  Reactive Energy +>10 000 / varh / 0.001
{ 0, 2840, 1, 779, 36, 4 },  //  Hour Meter +<10000 / h / 100
{ 0, 2841, 1, 798, 36, 4 },  //  Hour Meter +>10000 / h / 100
{ 0, 2842, 1, 186, 93, 2 },  //  Active Power phase1 +/- / W / 0.1
{ 0, 2843, 1, 210, 93, 2 },  //  Active Power phase2 +/- / W / 0.1
{ 0, 2844, 1, 234, 93, 2 },  //  Active Power phase3 +/- / W / 0.1
{ 0, 2845, 1, 2127, 70, 2 },  //  Reactive Power phase1 +/- / var / 0.1
{ 0, 2846, 1, 2153, 70, 2 },  //  Reactive Power phase2 +/- / var / 0.1
{ 0, 2847, 1, 2179, 70, 2 },  //  Reactive Power phase3 +/- / var / 0.1
{ 0, 2848, 1, 361, 61, 4 },  //  Apparent Power phase1 / VA / 0.1
{ 0, 2849, 1, 383, 61, 4 },  //  Apparent Power phase2 / VA / 0.1
{ 0, 2850, 1, 405, 61, 4 },  //  Apparent Power phase3 / VA / 0.1
{ 0, 2851, 1, 1642, 0, 2 },  //  Power factor phase 1-: leading and +: la / - / 1000
{ 0, 2852, 1, 1765, 0, 2 },  //  Power factor phase 2-: leading and +: la / - / 1000
{ 0, 2853, 1, 1888, 0, 2 },  //  Power factor phase 3-: leading and +: la / - / 1000
{ 0, 2854, 1, 1929, 70, 4 },  //  Q+ AVG MAX / var / 0.1

{ 0, 1280, 1, 527, 0, 8 },  //  Current alarm on lower threshold cause:0 / - / 1000

{ 0, 1283, 1, 568, 0, 8 },  //  Current alarm on upper threshold cause:0 / - / 1000

{ 0, 513, 1, 637, 23, 8 },  //  Current Transformer secondary : 1: 1 A 5 / A
{ 0, 514, 1, 609, 23, 8 },  //  Current Transformer primary / A
);

#endif  //  ALL_REGISTERS_H
