/* C++
 * us2066.h
 *
 *  Created on: Aug 26, 2018
 *      Author: JSRagman
 *
 *  Description:
 *    US2066 Controller used in Newhaven Display
 *    NHD-0420CW Character OLED Display Modules.
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
 */

#ifndef US2066_H_
#define US2066_H_


#include <stdint.h>
#include <string>
#include <vector>

#include "bbb-i2c.h"


namespace nhd_us2066
{

class US2066
{
  private:
	bbbi2c::I2CBus*  i2cbus;
	uint8_t          i2caddr;

  public:
	 US2066 ( bbbi2c::I2CBus* bus, uint8_t addr );
	~US2066 ();

	void Command ( uint8_t cmd );

	void Data ( uint8_t dat );
	void Data ( std::string dat );
	void Data ( std::vector<uint8_t> dat );

	void Clear ();
	void Init  ();
	void Off   ();
	void On    ();

	void SetState     ( uint8_t state );
	void SetPosition  ( int line, int col=1 );
	void SplashScreen ( std::string txt );
};


} // namespace nhd_us2066


#endif /* US2066_H_ */
