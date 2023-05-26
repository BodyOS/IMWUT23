/*
 * sensorDataRead.h
 *
 *  Created on: Nov 2, 2022
 *      Author: erenyildiz
 */

#ifndef HARDWARESIM_SENSORDATAREAD_H_
#define HARDWARESIM_SENSORDATAREAD_H_

extern uint16_t cur_data_num;
extern uint32_t missingData;
extern uint32_t missingDataArr[10];
extern int miscount;
void SensorUpdate();

#endif /* HARDWARESIM_SENSORDATAREAD_H_ */
