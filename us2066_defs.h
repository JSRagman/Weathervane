/* C++
 * us2066_defs.h
 *
 *  Created on: Aug 26, 2018
 *      Author: JSRagman
 *
 *  Description:
 *    Definitions for the US2066 OLED controller used in
 *    Newhaven Display NHD-0420CW display modules.
 *
 *    Currently, only a small subset of commands are defined
 *    here. More will be added as I figure out how to use
 *    them.
 *
 *  NOTE:
 *    This header was written by JSRagman, who is NOT a professional
 *    programmer.  Use it, if you like, but don't stake your life on it.
 *
 *  And Another Thing:
 *    JSRagman is not associated in any way with the good people at
 *    Newhaven Display.
 *
 *    He is, however, grateful for the professional help that he
 *    received on their forum. He is extremely grateful that they
 *    declined to laugh at his truly rediculous mistakes.
 *
 *    It almost makes up for the fact that their NHD_US2066 repository
 *    is written for Arduino.
 */

#ifndef US2066_DEFS_H_
#define US2066_DEFS_H_

// Display I2C Address:
#define US2066_I2C0          0x3C
#define US2066_I2C1          0x3D

// Display ON/OFF Control
#define US2066_OFF           0x08   //**POR. Display off.
#define US2066_ON            0x0C   //  Display on, no cursor.
#define US2066_CURS          0x0E   //  Display on, cursor on.
#define US2066_BLINK         0x0F   //  Display on, cursor on, blinking.


// Control Bytes:
#define US2066_CTRL_CMD      0x00   // D/C# = 0
#define US2066_CTRL_DATA     0x40   // D/C# = 1


// Command Definitions:

// IS,RE,SD
//  X  X  0
#define US2066_CLEAR         0x01   // Clear display
#define US2066_HOME          0x02   // Home cursor

// IS,RE,SD
//  X  0  0
#define US2066_FUNCSET_EXT   0x2A   // Function Set, extended (sets RE)
#define US2066_SET_DDRAM     0x80   // Set DDRAM address

// DDRAM Display Line Selection:
#define US2066_LINE_1        0x80
#define US2066_LINE_2        0xA0
#define US2066_LINE_3        0xC0
#define US2066_LINE_4        0xE0


// IS,RE,SD
//  0  0  0
// Cursor or Display Shift
#define US2066_CURS_LEFT     0x10   // Enable cursor shift left
#define US2066_CURS_RIGHT    0x14   // Enable cursor shift right
#define US2066_DISP_LEFT     0x18   // Enable display shift right.
#define US2066_DISP_RIGHT    0x1C   // Enable display shift right.


// IS,RE,SD
//  X  1  0
#define US2066_FUNCSEL_A     0x71
#define US2066_FUNCSEL_B     0x72
#define US2066_FUNCSET_FUND  0x28   // Function Set, fundamental (clears RE)



#endif /* US2066_DEFS_H_ */
