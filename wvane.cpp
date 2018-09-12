/*
 * wvane.cpp
 *
 *  Created on: Sep 5, 2018
 *      Author: JSRagman
 *
 *  Description:
 *    Weathervane, Rev D
 */


#include <chrono>           // std::chrono::seconds
#include <iomanip>          // dec, fixed, setprecision()
#include <iostream>         // cout, cin
#include <mutex>            // mutex, lock_guard
#include <thread>           // this_thread

#include "bbb-i2c.hpp"       // I2CBus
#include "bmp280.hpp"        // BMP280
#include "us2066.hpp"        // US2066



using namespace std;
using namespace std::chrono;

using namespace bbbi2c;
using namespace bosch_bmp280;
using namespace nhd_us2066;




I2CBus i2cbus  { BBB_I2C2_FILE };
US2066 display { &i2cbus, US2066_I2C0 };
BMP280 sensor  { &i2cbus, BMP280_I2C1 };

TP32DataQueue datacue { 1440 };
unsigned int  readperiod;



/*
 * string FormatTime(time_t timer)
 *
 * Description:
 *   Converts a time_t value into a local time formatted
 *   string hh:mm.
 */
string FormatTime(time_t thetime)
{
	struct tm* tinfo = localtime(&thetime);

    stringstream sstime;
    sstime << setprecision(0) << tinfo->tm_hour << ":";
    if (tinfo->tm_min < 10)
    {
    	sstime << "0";
    }
    sstime << tinfo->tm_min;

    return sstime.str();
}

/*
 * void UpdateDisplay(TP32Data rdg)
 *
 * Description:
 *   Sends temperature and pressure readings to the display.
 */
void UpdateDisplay(TP32Data rdg)
{
	char degchar  = 0xDF;  // degrees character
    char inchchar = 0x22;  // inches character

    double temp      = 9.0*(rdg.temperature/500.0) + 32.0;
	double press     = rdg.pressure/3386.39;
	double highpress = datacue.pressure_high()/3386.39;
	double lowpress  = datacue.pressure_low()/3386.39;
	double avgpress  = datacue.pressure_average()/3386.39;

	stringstream sshg, sshigh, sslow, ssavg, sstemp;

	sstemp << dec << fixed << setprecision(1) << temp << degchar;

	sshg   << "* " << dec << fixed << setprecision(2) << press << inchchar << " Hg";
	sshigh << "H " << dec << fixed << setprecision(2) << highpress;
	ssavg  << "A " << dec << fixed << setprecision(2) << lowpress;
	sslow  << "L " << dec << fixed << setprecision(2) << avgpress;

    display.Clear();
    display.Data(sshg.str());
    display.SetPosition(1,14);
    display.Data(sstemp.str());

    display.SetPosition(2,1);
    display.Data(sshigh.str());
    display.SetPosition(3,1);
    display.Data(ssavg.str());
    display.SetPosition(4,1);
    display.Data(sslow.str());

    display.SetPosition(3,15);
    display.Data(FormatTime(datacue.timestart()));

	time_t timer = time(nullptr);
    display.SetPosition(4,15);
    display.Data(FormatTime(timer));
}


/*
 * void ReadSensorProc()
 *
 * Description:
 *   Retrieves a reading from the sensor and pushes it onto
 *   the data queue at fixed intervals.
 *
 *   ReadSensorProc is intended to run in its own thread, and will
 *   continue to run for as long as readperiod is greater than zero.
 *
 * Global Parameters:
 *   readperiod - Sets the time, in seconds, between sensor readings.
 *                A value of zero will cause ReadSensorProc() to exit.
 *
 */
void ReadSensorProc()
{
	if (readperiod <= 0)
		return;

	do
	{
		sensor.Force();
		this_thread::sleep_for(milliseconds(45));

		TP32Data reading = sensor.GetComp32FixedData();

		if (true)                                // lock_guard scope
		{
			lock_guard<mutex> lck(datacue.mtx);
			datacue.push(reading);
		}

		UpdateDisplay(reading);
		this_thread::sleep_for(seconds{readperiod});

	} while (readperiod > 0);
}


/*
 * Sequence:
 *   1. Initialize display
 *   2. Initialize sensor
 *   3. Pause to admire the startup message.
 *   4. Start the sensor reading process.
 *   5. Enter the command loop.
 *   6. "q" exits the command loop.
 *   7. Setting readperiod to zero signals ReadSensorProc to exit.
 *   8. Wait for it.
 *   9. Be polite.
 *   10. Turn out the lights.
 */
int main(int argc, char* argv[])
{
	display.Init();                              // 1.
    display.Data("Weathervane...");
    display.On();

	readperiod = 60;                             // 2.
	sensor.SetConfig(3);

	this_thread::sleep_for(seconds{10});         // 3.

    thread t1 {ReadSensorProc};                  // 4.

    string cmd = "";                             // 5.
    while (cmd != "q")
    {
    	cout << "Weathervane: ";
    	cin >> cmd;
    	//ExecuteCommand(cmd);
    }                                            // 6.

    cout << endl << "Exiting Weathervane...";

    readperiod = 0;                              // 7.
    t1.join();                                   // 8.

    cout << endl << "bye" << endl;               // 9.
    display.Off();                               // 10.

    return 0;
}






















