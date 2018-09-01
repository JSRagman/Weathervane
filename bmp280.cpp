/*
 * bmp280.cpp
 *
 *  Created on: Aug 26, 2018
 *      Author: JSRagman
 *
 *  Description:
 *    Bosch BMP280 Temperature/Pressure Sensor
 *
 *  Notes:
 *    1. Separated the functions dealing with temperature and
 *       pressure compensation into a dedicated source file.
 *       Seriously. It needed to be done.
 */


#include  <unistd.h>

#include "bbb-i2c.h"
#include "bmp280.h"
#include "bmp280_defs.h"


using namespace bbbi2c;


namespace bosch_bmp280
{

/*
 * CalParams::CalParams()
 *
 * Description:
 *   Constructor. Initializes all parameters to zero,
 *   loaded = false.
 *
 * Namespace:
 *   bosch_bmp280
 *
 * Header File(s);
 *   bmp280.h
 */
CalParams::CalParams()
{
	t1 = 0;
	t2 = 0;
	t3 = 0;

	p1 = 0;
	p2 = 0;
	p3 = 0;
	p4 = 0;
	p5 = 0;
	p6 = 0;
	p7 = 0;
	p8 = 0;
	p9 = 0;

	loaded = false;
}


// BMP280 Constructor, Destructor
// -----------------------------------------------------------------

/*
 * BMP280::BMP280(I2CBus* bus, uint8_t addr)
 *
 * Description:
 *   Constructor. Assigns the I2C bus and the target device address.
 *
 * Parameters:
 *   bus  - pointer to an I2CBus object.
 *   addr - the target device I2C address.
 *
 * Namespace:
 *   bosch_bmp280
 *
 * Header File(s):
 *   bmp280.h
 */
BMP280::BMP280(I2CBus* bus, uint8_t addr)
{
    i2cbus  = bus;
    i2caddr = addr;
    tfine   = 0;
}

/*
 * BMP280::~BMP280()
 *
 * Description:
 *   Destructor. Move along. Nothing to see here.
 *
 * Namespace:
 *   bosch_bmp280
 *
 * Header File(s):
 *   bmp280.h
 */
BMP280::~BMP280()
{ }


// BMP280 Protected
// -----------------------------------------------------------------

/*
 * void BMP280::GetRegs(uint8_t startaddr, uint8_t* data, int len)
 *
 * Description:
 *   Reads the contents of one or more consecutive device registers.
 *
 * Parameters:
 *   startaddr - address of the first register to be read.
 *   data      - pointer to a buffer that will receive data.
 *   len       - the number of bytes to read.
 *
 * Exceptions:
 *   I2CException
 *
 * Namespace:
 *   bosch_bmp280
 *
 * Header File(s);
 *   bmp280.h
 */
void BMP280::GetRegs(uint8_t startaddr, uint8_t* data, int len)
{
    i2cbus->Xfer(startaddr, data, len, i2caddr);
}

/*
 * void BMP280::SetRegs(uint8_t* data, int len)
 *
 * Description:
 *   Writes to one or more device registers. Outgoing bytes must
 *   be organized in pairs - a register address is written first,
 *   followed by a data byte for that register.
 *   This {address, data} sequence is repeated for each register
 *   to be written.
 *
 * Parameters:
 *   data - pointer to a buffer containing {address, data}
 *          pairs to be written to the device.
 *   len  - the total number of bytes to be written.
 *
 * Exceptions:
 *   I2CException
 *
 * Namespace:
 *   bosch_bmp280
 *
 * Header File(s);
 *   bmp280.h
 */
void BMP280::SetRegs(uint8_t* data, int len)
{
	i2cbus->Write(data, len, i2caddr);
}

/*
 * TPData BMP280::GetUncompData()
 *
 * Description:
 *   Retrieves temperature and pressure data from the sensor.
 *
 * Returns:
 *   Returns a structure containing uncompensated temperature
 *   and pressure data and a time stamp.
 *
 * Exceptions:
 *   I2CException
 *
 * Namespace:
 *   bosch_bmp280
 *
 * Header File(s);
 *   bmp280.h
 *   bmp280_defs.h
 */
TPData BMP280::GetUncompData()
{
    TPData unc;
    uint8_t dat[6]{0};

    this->GetRegs(BMP280_R_PMSB, dat, 6);

    unc.timer = time(nullptr);

    unc.pressure =
        (int32_t) (
            (((uint32_t)dat[0]) << 12) |
            (((uint32_t)dat[1]) <<  4) |
            (((uint32_t)dat[2]) >>  4)
            );

    unc.temperature =
        (int32_t) (
            (((int32_t)dat[3]) << 12) |
            (((int32_t)dat[4]) <<  4) |
            (((int32_t)dat[5]) >>  4)
            );

    return unc;
}


// BMP280 Public
// -----------------------------------------------------------------

/*
 * void BMP280::Reset()
 *
 * Description:
 *   Resets the device. Allows time for the
 *   reset process to complete before returning.
 *
 * Exceptions:
 *   I2CException
 *
 * Namespace:
 *   bosch_bmp280
 *
 * Header File(s):
 *   bmp280.h
 *   bmp280_defs.h
 */
void BMP280::Reset()
{
    uint8_t dat[] { BMP280_R_RESET, BMP280_CMD_RESET };
    i2cbus->Write(dat, 2, i2caddr);

    usleep(BMP280_RESET_DELAY);
}

/*
 * void BMP280::SetConfig(uint8_t ctrl, uint8_t conf)
 *
 * Description:
 *   Resets the BMP280 to get it into Sleep mode, then writes the
 *   ctrl and conf parameters to the ctrl_meas and config registers.
 *
 *   Note that the first write to ctrl_meas is done with the mode bits
 *   masked out (ctrx). The config register is written next, followed
 *   by the ctrl_meas register, this time with the mode bits included.
 *
 *   The CONFIG_DELAY constant is currently a number that I pulled out
 *   of a hat as a reasonable amount of time before the first data
 *   read should be attempted. The BMP280 datasheet gives the precise
 *   method of determining what this time should be.
 *
 * Parameters:
 *   ctrl - 8-bits to be written to the ctrl_meas register.
 *   conf - 8-bits to be written to the config register.
 *
 * Exceptions:
 *   I2CException
 *
 * Namespace:
 *   bosch_bmp280
 *
 * Header File(s);
 *   bmp280.h
 *   bmp280_defs.h
 */
void BMP280::SetConfig(uint8_t ctrl, uint8_t conf)
{
    uint8_t ctrx = (ctrl & BMP280_MODE_MSK_OUT);
    uint8_t dat[] { BMP280_R_CTRL, ctrx, BMP280_R_CONF, conf, BMP280_R_CTRL, ctrl };

	this->Reset();
    this->SetRegs(dat, 6);
    usleep(BMP280_CONFIG_DELAY);
}

/*
 * void BMP280::SetConfig(int preset)
 *
 * Description:
 *   Sets the device to one of the six available preset configurations.
 *
 * Parameters:
 *   preset - Preset configuration number.
 *            An integer value between one and six, inclusive.
 *            Values outside of this range will select the default
 *            configuration (PRE1).
 *
 * Exceptions:
 *   I2CException
 *
 * Namespace:
 *   bosch_bmp280
 *
 * Header File(s);
 *   bmp280.h
 *   bmp280_defs.h
 */
void BMP280::SetConfig(int preset)
{
	uint8_t ctrl;
	uint8_t conf;

    switch (preset)
    {
      case 1:
        ctrl = BMP280_CTRL_PRE1;
        conf = BMP280_CONF_PRE1;
        break;
      case 2:
        ctrl = BMP280_CTRL_PRE2;
        conf = BMP280_CONF_PRE2;
        break;
      case 3:
        ctrl = BMP280_CTRL_PRE3;
        conf = BMP280_CONF_PRE3;
        break;
      case 4:
        ctrl = BMP280_CTRL_PRE4;
        conf = BMP280_CONF_PRE4;
        break;
      case 5:
        ctrl = BMP280_CTRL_PRE5;
        conf = BMP280_CONF_PRE5;
        break;
      case 6:
        ctrl = BMP280_CTRL_PRE6;
        conf = BMP280_CONF_PRE6;
        break;
      default:
        ctrl = BMP280_CTRL_PRE1;
        conf = BMP280_CONF_PRE1;
        break;
	}

    this->SetConfig(ctrl, conf);
}

/*
 * bool BMP280::IsBusy()
 *
 * Description:
 *   Reads the device status register and returns true if
 *   either bit 0 (im_update) or bit 3 (measuring) is set.
 *
 * Exceptions:
 *   I2CException
 *
 * Namespace:
 *   bosch_bmp280
 *
 * Header File(s);
 *   bmp280.h
 *   bmp280_defs.h
 */
bool BMP280::IsBusy()
{
	uint8_t status;
	this->GetRegs(BMP280_R_STAT, &status, 1);

	return ((status & BMP280_STATUS_MSK) != 0);
}

/*
 * TPData BMP280::GetReading()
 *
 * Description:
 *   Retrieves a compensated temperature and pressure reading.
 *
 * Returns:
 *   Returns a TPData structure containing a time stamp, a temperature
 *   reading, in 1/100 degrees centigrade, and a pressure reading, in
 *   pascals.
 *
 * Exceptions:
 *   I2CException
 *
 * Namespace:
 *   bosch_bmp280
 *
 * Header File(s);
 *   bmp280.h
 */
TPData BMP280::GetReading()
{
	TPData   reading;
    TPData   unc = this->GetUncompData();

    reading.timer       = unc.timer;
    reading.temperature = this->Comp32FixedTemp(unc.temperature);
    reading.pressure    = this->Comp32FixedPress(unc.pressure);

    return reading;
}



} // namespace bosch_bmp280
