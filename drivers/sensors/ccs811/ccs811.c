/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Project : A_os
*/
/*
 * ccs811.c
 *
 *  Created on: Jan 22, 2024
 *      Author: Luigi F
 */

#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A_exported_functions.h"
#include "../../../kernel/HwDevices/hw_i2c.h"



#ifdef SENSORS_CCS8118
	#include "ccs811.h"
	#include <stdint.h>

    extern	void DWT_Delay_us(uint32_t au32_microseconds);

	// Timings
	#define CCS811_WAIT_AFTER_RESET_US     2000 // The CCS811 needs a wait after reset
	#define CCS811_WAIT_AFTER_APPSTART_US  1000 // The CCS811 needs a wait after app start
	#define CCS811_WAIT_AFTER_WAKE_US        50 // The CCS811 needs a wait after WAKE signal
	#define CCS811_WAIT_AFTER_APPERASE_MS   500 // The CCS811 needs a wait after app erase (300ms from spec not enough)
	#define CCS811_WAIT_AFTER_APPVERIFY_MS   70 // The CCS811 needs a wait after app verify
	#define CCS811_WAIT_AFTER_APPDATA_MS     50 // The CCS811 needs a wait after writing app data

    uint8_t BurnIn_Time_Complete=0;
	uint8_t RunIn_Time_Complete=0;
	uint8_t Baseline_Time_Complete=0;
	uint8_t EBaseline_Time_Complete=0;

	//These are the air quality values obtained from the sensor
	unsigned int tVOC = 0;
	unsigned int CO2 = 0;
	unsigned int hwID=0;
	uint16_t adc_raw=0;
	uint8_t current_value=0;
	uint8_t dummyread=0;
	uint8_t appvalue=0;
	uint8_t errvalue=0;
	uint8_t mosetting=0;
	uint8_t dtvalue =0;
	uint8_t appStart=0;
	uint32_t ELBaseline_period=0;
	uint32_t ALBaseline_period=0;
	uint8_t  Mode_CCS811=1;

	float relativeHumidity=65.0,temperature=25.0;

	// Get measurement results from the CCS811, check status via errstat, e.g. ccs811_errstat(errstat)
	/*
	void read( uint16_t*eco2, uint16_t*etvoc, uint16_t*errstat,uint16_t*raw) {
	  bool    ok;
	  uint8_t buf[8];
	  uint8_t stat;
	  wake_up();
	    if( _appversion<0x2000 ) {
	      ok= i2cread(CCS811_STATUS,1,&stat); // CCS811 with pre 2.0.0 firmware has wrong STATUS in CCS811_ALG_RESULT_DATA
	      if( ok && stat==CCS811_ERRSTAT_OK ) ok= i2cread(CCS811_ALG_RESULT_DATA,8,buf); else buf[5]=0;
	      buf[4]= stat; // Update STATUS field with correct STATUS
	    } else {
	      ok = i2cread(CCS811_ALG_RESULT_DATA,8,buf);
	    }
	  wake_down();
	  // Status and error management
	  uint16_t combined = buf[5]*256+buf[4];
	  if( combined & ~(CCS811_ERRSTAT_HWERRORS|CCS811_ERRSTAT_OK) ) ok= false; // Unused bits are 1: I2C transfer error
	  combined &= CCS811_ERRSTAT_HWERRORS|CCS811_ERRSTAT_OK; // Clear all unused bits
	  if( !ok ) combined |= CCS811_ERRSTAT_I2CFAIL;
	  // Clear ERROR_ID if flags are set
	  if( combined & CCS811_ERRSTAT_HWERRORS ) {
	      int err = get_errorid();
	      if( err==-1 ) combined |= CCS811_ERRSTAT_I2CFAIL; // Propagate I2C error
	  }
	  // Outputs
	  if( eco2   ) *eco2   = buf[0]*256+buf[1];
	  if( etvoc  ) *etvoc  = buf[2]*256+buf[3];
	  if( errstat) *errstat= combined;
	  if( raw    ) *raw    = buf[6]*256+buf[7];;
	}
	*/
		
/*
  * @brief  Updates the total voltatile organic compounds (TVOC) in parts per billion (PPB) and the CO2 value.
  * @param  NONE.
  * @retval None.
 */	
uint8_t readAlgorithmResults()
{
	 
	uint8_t data_rq[4];
	uint8_t status = hw_i2c_MemGet8(SENSORS_CCS818_I2C_INDEX, CCS811_ADDRD, CSS811_ALG_RESULT_DATA, data_rq, 4);

	if (status == HW_I2C_ERROR_NONE)
	{
		CO2   = data_rq[0]*256+data_rq[1]; //TVOC value, in parts per billion (ppb)
		tVOC  = data_rq[2]*256+data_rq[3]; //eC02 value, in parts per million (ppm)
	}
	else
	{
		CO2   = 0;
		tVOC  = 0;
	}
	return status;
}

/*
  * @brief  configureCCS811.
  * @param  NONE.
  * @retval None.
 */
uint8_t configureCCS811(void)
{
	uint8_t    lodata[1];

	// Invoke a SW reset (bring CCS811 in a know state)
	SwReset();
	DWT_Delay_us(CCS811_WAIT_AFTER_RESET_US);
	//Verify the hardware ID is what we expect

	hwID = readRegister(CSS811_HW_ID); //Hardware ID should be 0x81
	if (hwID != 0x81)
		return 0;
	else
	{
		lodata[0]= CSS811_APP_START;
		hw_i2c_Send(SENSORS_CCS818_I2C_INDEX, 0xB6, lodata, 1);

		DWT_Delay_us(CCS811_WAIT_AFTER_APPSTART_US);

		setDriveMode(Mode_CCS811); //Read every second

		DWT_Delay_us(CCS811_WAIT_AFTER_APPSTART_US);

		if (checkForError() == 1)
		{
			return 0;
		}
	}

	return 1;
}

	//Checks to see if error bit is set
	FlagStatus checkForError()
	{
		errvalue = readRegister(CSS811_ERROR_ID);
	    errvalue = readRegister(CSS811_STATUS);
		return (errvalue & 1 << 0);
	}

	 

	//Checks to see if DATA_READ flag is set in the status register
	FlagStatus dataAvailable()
	{
	    dtvalue = readRegister(CSS811_STATUS);
		return (dtvalue & 1 << 3);
	}

	//Checks to see if APP_VALID flag is set in the status register
	FlagStatus appValid()
	{
		appvalue = readRegister(CSS811_STATUS);
		return (appvalue & (1 << 4));
	}

	//Enable the nINT signal
	void enableInterrupts(void)
	{
		uint8_t setting = readRegister(CSS811_MEAS_MODE); //Read what's currently there
		setting |= (1 << 3); //Set INTERRUPT bit
		writeRegister(CSS811_MEAS_MODE, setting);
	}

	//Disable the nINT signal
	void disableInterrupts(void)
	{
		uint8_t setting = readRegister(CSS811_MEAS_MODE); //Read what's currently there
		setting &= ~(1 << 3); //Clear INTERRUPT bit
		writeRegister(CSS811_MEAS_MODE, setting);
	}

/*
  * @brief  //Mode 0 = Idle
	    //Mode 1 = read every 1s
	    //Mode 2 = every 10s
	    //Mode 3 = every 60s
	    //Mode 4 = RAW mode.
  * @param  MODE.
  * @retval None.
 */
void setDriveMode(uint8_t mode)
{
		if (mode > 4) mode = 4; //Error correction
	 
		mosetting = readRegister(CSS811_MEAS_MODE); //Read what's currently there
	 
		mosetting &=~(7<<4); //Clear DRIVE_MODE bits
		mosetting |= (mode << 4); //Mask in mode
	 
		writeRegister(CSS811_MEAS_MODE, mosetting);
		mosetting = readRegister(CSS811_MEAS_MODE); //Read what's currently there
	 
}

 
 /*
  * @brief //Given a temp and humidity, write this data to the CSS811 for better compensation
	 //This function expects the humidity and temp to come in as floats
  * @param  relativeHumidity HUMIDITY.
  * @param  temperature TEMPERATURE.
  * @retval None.
 */
void setEnvironmentalData(float relativeHumidity, float temperature)
{
		int rH = relativeHumidity * 1000; //42.348 becomes 42348
		int temp = temperature * 1000; //23.2 becomes 23200

		uint8_t envData[4];


		//Split value into 7-bit integer and 9-bit fractional
		envData[0] = ((rH % 1000) / 100) > 7 ? (rH / 1000 + 1) << 1 : (rH / 1000) << 1;
		envData[1] = 0; //CCS811 only supports increments of 0.5 so bits 7-0 will always be zero
		if (((rH % 1000) / 100) > 2 && (((rH % 1000) / 100) < 8))
		{
			envData[0] |= 1; //Set 9th bit of fractional to indicate 0.5%
		}

		temp += 25000; //Add the 25C offset
		//Split value into 7-bit integer and 9-bit fractional
		envData[2] = ((temp % 1000) / 100) > 7 ? (temp / 1000 + 1) << 1 : (temp / 1000) << 1;
		envData[3] = 0;
		if (((temp % 1000) / 100) > 2 && (((temp % 1000) / 100) < 8))
		{
			envData[2] |= 1;  //Set 9th bit of fractional to indicate 0.5C
		}

		hw_i2c_MemSend8(SENSORS_CCS818_I2C_INDEX, CCS811_ADDWR, CSS811_ENV_DATA, envData, 4);
		while (hw_i2c_check_presence(SENSORS_CCS818_I2C_INDEX, CCS811_ADDWR));
		 
	}


 /*
  * @brief  reading_Multiple_Register
  * @param  addr ADDRESS.
  * @param  val VALUE.
  * @param  size SIZE.
  * @retval None.
 */
void read_Mul_Register(uint8_t addr, uint8_t * val,uint8_t size)
{
	hw_i2c_MemGet8(SENSORS_CCS818_I2C_INDEX, CCS811_ADDRD, addr,  val, size );
}

 /*
  * @brief  softRest
  * @param  NONE.
  * @retval None.
 */
void SwReset(void ) {
     
	uint8_t rstCMD[4] = {0x11,0xE5,0x72,0x8A};
	hw_i2c_MemSend8(SENSORS_CCS818_I2C_INDEX, CCS811_ADDWR, CSS811_SW_RESET, rstCMD, 4);
	while (hw_i2c_check_presence(SENSORS_CCS818_I2C_INDEX, CCS811_ADDWR));

	      
}	


 /*
  * @brief  get_Sensor_Resistance
  * @param  NONE.
  * @retval RESISTANCE OF SENSOR.
 */
uint32_t get_Sensor_Resistance(){
	uint8_t sensor_Resistance_raw[2];

	uint32_t sensor_Resistance;
	
	//HAL_I2C_Mem_Read( &hi2c1, CCS811_ADDRD, CSS811_RAW_DATA , I2C_MEMADD_SIZE_8BIT, sensor_Resistance_raw, 2,100 );

	hw_i2c_MemGet8(SENSORS_CCS818_I2C_INDEX, CCS811_ADDRD, CSS811_RAW_DATA,  sensor_Resistance_raw, 2 );
	current_value=sensor_Resistance_raw[0]>>2;


    sensor_Resistance_raw[0]=sensor_Resistance_raw[0]&0x03;
	
	adc_raw=(sensor_Resistance_raw[0]<<8)|sensor_Resistance_raw[1];
	
	sensor_Resistance=((165*adc_raw)*10000)/(current_value*1023);
 
	return sensor_Resistance;
}

 /*
  * @brief  sleep
  * @param  NONE.
  * @retval NONE.
 */
void sleep()
{
  // sets sensor to idle; measurements are disabled; lowest power mode
   writeRegister(CSS811_MEAS_MODE, 0);
}



 /*
  * @brief  Reads from a give location from the CSS811
  * @param  addr  ADDRESS.
  * @retval VALUE AT THE ADDRESS.
 */	 
uint8_t readRegister(uint8_t addr)
{
	uint8_t dt;

	hw_i2c_MemGet8(SENSORS_CCS818_I2C_INDEX, CCS811_ADDRD, addr,  &dt, 1 );
	while (hw_i2c_check_presence(SENSORS_CCS818_I2C_INDEX, CCS811_ADDWR));

	return dt;
}
		
		 
		 
uint32_t  restore_Baselines;
		 
/*
* @brief  	//restore the baseline value
			//Used for telling sensor what 'clean' air is
			//You must put the sensor in clean air and record this value
* @param  NONE.
* @retval NONE.
*/
void restore_Baseline()
{
	uint32_t  restore_Baseline = 0;


	restore_Baselines = restore_Baseline ;
	uint8_t res_bs[2];
	res_bs[0]=restore_Baseline>>8;
	res_bs[1]=restore_Baseline&0x000000FF;

	hw_i2c_MemSend8(SENSORS_CCS818_I2C_INDEX, CCS811_ADDWR, CSS811_BASELINE, res_bs, 2);

	while (hw_i2c_check_presence(SENSORS_CCS818_I2C_INDEX, CCS811_ADDWR));
}

 
/*
  * @brief  	//Returns the baseline value and saves into EEPROM
	            //Used for telling sensor what 'clean' air is
	            //You must put the sensor in clean air and record this value
  * @param  NONE.
  * @retval BASELINE VALUE.
 */
unsigned int getBaseline()
{
		 
	uint8_t ada[2];

	hw_i2c_MemGet8(SENSORS_CCS818_I2C_INDEX, CCS811_ADDRD, CSS811_BASELINE,  ada, 2 );
	while (hw_i2c_check_presence(SENSORS_CCS818_I2C_INDEX, CCS811_ADDWR));

	uint8_t baselineMSB = ada[0];
	uint8_t baselineLSB = ada[1];

	unsigned int baseline = ((unsigned int)baselineMSB << 8) | baselineLSB;

	return (baseline);
}



	//Write a value to a spot in the CCS811
/*
  * @brief  Write a value to a spot in the CCS811
  * @param  addr ADDRESS.
  * @param  val  VALUE.
  * @retval NONE.
 */
void writeRegister(uint8_t addr, uint8_t val)
{
		hw_i2c_MemSend8(SENSORS_CCS818_I2C_INDEX, CCS811_ADDWR, addr, &val, 1);
		while (hw_i2c_check_presence(SENSORS_CCS818_I2C_INDEX, CCS811_ADDWR));
}
	 
#endif

