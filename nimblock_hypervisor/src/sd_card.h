#ifndef SD_CARD_H
#define SD_CARD_H

#include "ff.h"
#include "xil_printf.h"
#include "xstatus.h"
#include <stdio.h>
int SD_Init(FATFS* fatfs);
int SD_TransferPartial(const char *FileName, u64 DestinationAddress, u32 ByteLength);

#endif /* SD_CARD_H */
