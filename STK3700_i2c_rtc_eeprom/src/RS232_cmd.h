/*
 * RS232_cmd.h
 *
 *  Created on: Feb 9, 2018
 *      Author: Administrator
 */

#ifndef SRC_RS232_CMD_H_
#define SRC_RS232_CMD_H_


#define INPUT_SIZE 20
extern char currentString[];

void waitForInput(void);
void receiveChar (void);
void sendString (char* string);

#endif /* SRC_RS232_CMD_H_ */
