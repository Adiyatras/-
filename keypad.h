/*
 * keypad.h
 *
 *  Created on: 2013-3-19
 *      Author: think
 */

#ifndef KEYPAD_H_
#define KEYPAD_H_

void Check_Key(void);
void Key_Event(void);

extern unsigned char key_Pressed;
extern unsigned char key_val;
extern unsigned char key_flag;


#endif /* KEYPAD_H_ */
