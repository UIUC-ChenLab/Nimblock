/*
 * my_io.c
 *
 *  Created on: Sep 22, 2020
 *      Author: Mang Yu
 */
//#include "ff.h"
#include "my_io.h"
#include "xil_printf.h"
#include <stdio.h>

char my_getc(FIL *fp){
	char c;
	UINT n;
	f_read (fp, &c, 1, &n);
	return c;
}

void my_ungetc(char c, FIL *fp){
	FSIZE_t fptr = fp->fptr;
	f_lseek(fp, fptr-1);
}

int read_int(FIL *fp){
	char buf[MAX_DIGITS] = {0};
	int ret;
	for(int i = 0;i < MAX_DIGITS;i ++){
		char c = my_getc(fp);
		if ((c <= '9') && (c >= '0')){
			buf[i] = c;
		} else {
			buf[i] = '\0';
			my_ungetc(c, fp);
			break;
		}
	}
	sscanf(buf, "%d", &ret);
	return ret;
}
