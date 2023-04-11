#ifndef CONFIGURE_H
#define CONFIGURE_H
#include "nimblock_types.h"
#include <malloc.h>
#include "sd_card.h"
#include "gpio_user.h"

#include "xilfpga.h"
int reconfigure_slot(XFpga XFpgaInstance, XGpioPs psGpioInstancePtr, Task* task, Slot* slot);
void XAccel_Set_Val(XHls_ip *InstancePtr, u64 Data, int offset);
#endif /* CONFIGURE_H */
