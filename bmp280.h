/*
 * C++
 * bmp280.h
 *
 *  Created on: Aug 26, 2018
 *      Author: JSRagman
 *
 *  Description:
 *    Bosch BMP280 Temperature/Pressure Sensor and
 *    supporting data structures.
 *
 *  NOTE:
 *    This header was written by JSRagman, who is NOT a professional
 *    programmer.  Use it, if you like, but don't stake your life on it.
 *
 *  And Another Thing:
 *    JSRagman is not associated in any way with the good people at
 *    Bosch, although sometimes he is a bit free with the drivers
 *    that are available on their GitHub site.
 */

#ifndef BMP280_H_
#define BMP280_H_


#include  <ctime>
#include  <mutex>
#include  <stdint.h>

#include "bbb-i2c.h"



namespace bosch_bmp280
{

/*
 * struct CalParams
 *
 * Description:
 *   A structure to hold the BMP280 calibration parameters.
 */
struct CalParams
{
	uint16_t  t1;
	 int16_t  t2;
	 int16_t  t3;

	uint16_t  p1;
	 int16_t  p2;
	 int16_t  p3;
	 int16_t  p4;
	 int16_t  p5;
	 int16_t  p6;
	 int16_t  p7;
	 int16_t  p8;
	 int16_t  p9;

	bool loaded;

	CalParams();
};

/*
 * struct TPData
 *
 * Description:
 *   A structure for recording 32-bit fixed-point temperature
 *   and pressure data with a time stamp.
 */
struct TPData
{
	  time_t  timer;
	 int32_t  temperature;
	uint32_t  pressure;
};


class BMP280
{

protected:
	 bbbi2c::I2CBus*  i2cbus;
	uint8_t          i2caddr;

	 void   GetRegs ( uint8_t startaddr, uint8_t* data, int len );
	 void   SetRegs ( uint8_t* data, int len );
	 TPData GetUncompData ();

	 int32_t   tfine;
	 CalParams cparams;

	 void     LoadCalParams ();
	 int32_t  Comp32FixedTemp  (  int32_t unctemp  );
	uint32_t  Comp32FixedPress ( uint32_t uncpress );

public:
	 std::mutex mtx;

	 BMP280 ( bbbi2c::I2CBus* bus, uint8_t addr );
	~BMP280 ();

	 TPData GetReading ();
	 bool   IsBusy ();

	 void  Reset ();
	 void  SetConfig ( int preset );
	 void  SetConfig ( uint8_t ctrl, uint8_t conf );

}; // class BMP280


} // namespace bosch_bmp280b

#endif /* BMP280_H_ */
