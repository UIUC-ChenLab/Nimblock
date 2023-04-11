/*
 * my_io.h
 *
 *  Created on: Sep 22, 2020
 *      Author: Mang Yu
 */

#ifndef SRC_MY_IO_H_
#define SRC_MY_IO_H_

#include "ff.h"
#define MAX_DIGITS 32

char my_getc(FIL *fp);
void my_ungetc(char c, FIL *fp);
int read_int(FIL *fp);

#endif /* SRC_MY_IO_H_ */
