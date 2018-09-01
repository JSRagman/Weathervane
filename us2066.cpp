/*
 * us2066.cpp
 *
 *  Created on: Aug 26, 2018
 *      Author: JSRagman
 *
 *  Description:
 *    US2066 Controller used in Newhaven Display
 *    NHD-0420CW Character OLED Display Modules.
 *
 *  NOTE:
 *    This was written by JSRagman, who is NOT a professional
 *    programmer.  Use it, if you like, but don't stake your life on it.
 *
 *  And Another Thing:
 *    JSRagman is not associated in any way with the good people at
 *    Newhaven Display.
 *
 *    He is, however, grateful for the professional help that he
 *    received on their forum. He is extremely grateful that they
 *    declined to laugh at his truly rediculous mistakes.
 */

#include "bbb-i2c.h"
#include "us2066.h"
#include "us2066_defs.h"

using namespace std;
using namespace bbbi2c;



namespace nhd_us2066
{


// US2066 Constructor, Destructor
// -----------------------------------------------------------------

/*
 * US2066::US2066(I2CBus* bus, uint8_t addr)
 *
 * Description:
 *   Constructor. Assigns the I2C bus and the target device address.
 *
 * Parameters:
 *   bus  - pointer to an I2CBus object.
 *   addr - the target device I2C address.
 *
 * Namespace:
 *   nhd_us2066
 *
 * Header File(s)
 *   us2066.h
 *   us2066_defs.h
 */
US2066::US2066(I2CBus* bus, uint8_t addr)
{
	i2cbus  = bus;
	i2caddr = addr;
}

/*
 * US2066::~US2066()
 *
 * Description:
 *   Destructor. Move along. Nothing to see here.
 *
 * Namespace:
 *   nhd_us2066
 *
 * Header File(s)
 *   us2066.h
 *   us2066_defs.h
 */
US2066::~US2066()
{ }


// US2066 Public
// -----------------------------------------------------------------

/*
 * void US2066::Command(uint8_t cmdbyte)
 *
 * Description:
 *   Sends a control byte followed by one command byte
 *   to the US2066 controller.
 *
 * Parameters:
 *   cmd - an 8-bit US2066 command.
 *
 * Exceptions:
 *   I2CException
 *
 * Namespace:
 *   nhd_us2066
 *
 * Header File(s)
 *   us2066.h
 *   us2066_defs.h
 */
void US2066::Command(uint8_t cmdbyte)
{
	uint8_t dat[] {US2066_CTRL_CMD, cmdbyte};
	i2cbus->Write(dat, 2, i2caddr);
}

/*
 * void US2066::Data(uint8_t datbyte)
 *
 * Description:
 *   Sends a control byte followed by one data byte.
 *
 * Parameters:
 *   dat - 8-bits of data
 *
 * Exceptions:
 *   I2CException
 *
 * Namespace:
 *   nhd_us2066
 *
 * Header File(s)
 *   us2066.h
 *   us2066_defs.h
 */
void US2066::Data(uint8_t datbyte)
{
	uint8_t dat[] { US2066_CTRL_DATA, datbyte };
	i2cbus->Write(dat, 2, i2caddr);
}

/*
 * void US2066::Data(string dat)
 *
 * Description:
 *   Sends a control byte followed by one or more
 *   data bytes.
 *
 * Parameters:
 *   sdat - a data string.
 *
 * Exceptions:
 *   I2CException
 *
 * Namespace:
 *   nhd_us2066
 *
 * Header File(s)
 *   us2066.h
 *   us2066_defs.h
 */
void US2066::Data(string sdat)
{
	char ctrl = US2066_CTRL_DATA;

	string::iterator it = sdat.begin();
	sdat.insert(it, ctrl);
	i2cbus->Write(sdat, i2caddr);
}

/*
 * void US2066::Data(vector<uint8_t> vdat)
 *
 * Description:
 *   Sends a control byte followed by one or more bytes
 *   of data.
 *
 * Parameters:
 *   vdat - a vector containing one or more bytes of data.
 *
 * Exceptions:
 *   I2CException
 *
 * Namespace:
 *   nhd_us2066
 *
 * Header File(s);
 *   us2066.h
 *   us2066_defs.h
 */
void US2066::Data(vector<uint8_t> vdat)
{
	uint8_t ctrl = US2066_CTRL_DATA;

	vector<uint8_t>::iterator it = vdat.begin();
	vdat.insert(it, ctrl);
	i2cbus->Write(vdat.data(), vdat.size(), i2caddr);
}

/*
 * void US2066::Clear()
 *
 * Description:
 *   Clears the display and homes the cursor.
 *
 * Initial Conditions:
 *   IS = X
 *   RE = X
 *   SD = 0
 *
 * Exceptions:
 *   I2CException
 *
 * Namespace:
 *   nhd_us2066
 *
 * Header File(s);
 *   us2066.h
 *   us2066_defs.h
 */
void US2066::Clear()
{
	this->Command(US2066_CLEAR);
}

/*
 * void US2066::Init()
 *
 * Description:
 *   Display power-on initialization sequence from datasheet
 *   NHD-0420CW-AB3, Rev 3, 2/2/17.
 *
 *   Slightly modified:
 *     a. Select CGROM C instead of A.
 *     b. Leave display off after initialization.
 *
 * Exceptions:
 *   I2CException
 *
 * Namespace:
 *   nhd_us2066
 *
 * Header File(s);
 *   us2066.h
 *   us2066_defs.h
 */
void US2066::Init()
{
	this->Command(0x2A); // function set (extended command set)
	this->Command(0x71); // function selection A
	this->Data(0x00);    // disable internal VDD regulator (2.8V I/O). data(0x5C) = enable regulator (5V I/O)

	this->Command(0x28); // function set (fundamental command set)
	this->Command(0x08); // display off, cursor off, blink off
	this->Command(0x2A); // function set (extended command set)
	this->Command(0x79); // OLED command set enabled
	this->Command(0xD5); // set display clock divide ratio/oscillator frequency
	this->Command(0x70); // set display clock divide ratio/oscillator frequency
	this->Command(0x78); // OLED command set disabled
	this->Command(0x09); // extended function set (4-lines)
	this->Command(0x06); // COM SEG direction
	this->Command(0x72); // function selection B
//	this->Data(0x00);    // ROM CGRAM selection
	this->Data(0x08);    // Use CGROM C

	this->Command(0x2A); // function set (extended command set)
	this->Command(0x79); // OLED command set enabled
	this->Command(0xDA); // set SEG pins hardware configuration
	this->Command(0x10); // set SEG pins hardware configuration
	this->Command(0xDC); // function selection C
	this->Command(0x00); // function selection C
	this->Command(0x81); // set contrast control
	this->Command(0x7F); // set contrast control
	this->Command(0xD9); // set phase length
	this->Command(0xF1); // set phase length
	this->Command(0xDB); // set VCOMH deselect level
	this->Command(0x40); // set VCOMH deselect level
	this->Command(0x78); // OLED command set disabled
	this->Command(0x28); // function set (fundamental command set)
	this->Command(0x01); // clear display
	this->Command(0x80); // set DDRAM address to 0x00
//	this->Command(0x0C); // display ON
}

/*
 * void US2066::Off()
 *
 * Description:
 *   Turns the display off.
 *   Does not change DDRAM contents.
 *
 * Initial Conditions:
 *   IS = X
 *   RE = 0
 *   SD = 0
 *
 * Exceptions:
 *   I2CException
 *
 * Namespace:
 *   nhd_us2066
 *
 * Header File(s);
 *   us2066.h
 *   us2066_defs.h
 */
void US2066::Off()
{
	this->SetState(US2066_OFF);
}

/*
 * void US2066::On()
 *
 * Description:
 *   Turns the display on (no cursor).
 *   Does not change DDRAM contents.
 *
 * Initial Conditions:
 *   IS = X
 *   RE = 0
 *   SD = 0
 *
 * Exceptions:
 *   I2CException
 *
 * Namespace:
 *   nhd_us2066
 *
 * Header File(s);
 *   us2066.h
 *   us2066_defs.h
 */
void US2066::On()
{
	this->SetState(US2066_ON);
}

/*
 * void US2066::SetState(uint8_t st)
 *
 * Description:
 *   Sets the display ON/OFF state. Configures the cursor
 *   as off, on(solid), or on(blinking).
 *
 * Parameters:
 *   st - Display state code:
 *     DS_OFF       Display off.
 *     DS_ON        Display on, cursor off.
 *     DS_CURS      Display on, cursor on.
 *     DS_BLINK     Display on, cursor blinking.
 *
 * Initial Conditions:
 *   IS = X
 *   RE = 0
 *   SD = 0
 *
 * Exceptions:
 *   I2CException
 *
 * Namespace:
 *   nhd_us2066
 *
 * Header File(s);
 *   us2066.h
 *   us2066_defs.h
 */
void US2066::SetState(uint8_t st)
{
	this->Command(st);
}

/*
 * void US2066::SetPosition(int line, int col)
 *
 * Description:
 *   Sets the DDRAM address (cursor position).
 *
 *   The Set DDRAM command (0b_1000_0000) and DDRAM address
 *   data (the other 7 bits) are combined into one byte.
 *
 * Initial Conditions:
 *   IS = X
 *   RE = 0
 *   SD = 0
 *
 * Parameters:
 *   line - Line number 1,2,3, or 4. Values outside
 *          this range will select line 1.
 *   col  - optional. Column number 1 - 20, inclusive.
 *          Values outside this range will select
 *          column 1.
 *          The default value is 1.
 *
 * Exceptions:
 *   I2CException
 *
 * Namespace:
 *   nhd_us2066
 *
 * Header File(s);
 *   us2066.h
 *   us2066_defs.h
 */
void US2066::SetPosition(int line, int col)
{
	uint8_t cmd = US2066_LINE_1;

	if ( col < 1 || col > 20 ) col = 1;

	switch (line)
	{
	  case 2:
		cmd = US2066_LINE_2;
		break;
	  case 3:
		cmd = US2066_LINE_3;
		break;
	  case 4:
		cmd = US2066_LINE_4;
		break;
	  default:
		cmd = US2066_LINE_1;
		break;
	}

	cmd += (col-1);

	this->Command(cmd);
}

/*
 * void US2066::SplashScreen(string txt)
 *
 * Description:
 *   Display a splash screen. In this case, a splash line.
 *
 * Parameters:
 *   txt - Text to display.
 *
 * Exceptions:
 *   I2CException
 *
 * Namespace:
 *   nhd_us2066
 *
 * Header File(s);
 *   us2066.h
 *   us2066_defs.h
 */
void US2066::SplashScreen(string txt)
{
	this->Clear();
	this->Data(txt);
	this->On();
}



} // namespace nhd_us2066
