/*
 * RS232_cmd.h
 *
 *  Created on: Feb 9, 2018
 *      Author: Administrator
 */

#ifndef SRC_RS232_CMD_H_
#define SRC_RS232_CMD_H_


extern char receivedString[2];

void sendString (char* string);
char receiveChar (void);

#endif /* SRC_RS232_CMD_H_ */
