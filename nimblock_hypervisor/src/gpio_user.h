#ifndef GPIO_USER_H
#define GPIO_USER_H
#include "xgpiops.h"
#include <stdlib.h>
#include <stdio.h>
int init_gpio(XGpioPs_Config* GpioConfigPtr, XGpioPs* psGpioInstancePtr, int xStatus);
#endif /* GPIO_USER_H */
