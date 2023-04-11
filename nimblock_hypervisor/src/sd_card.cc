#include "sd_card.h"

int SD_Init(FATFS* fatfs) {
	FRESULT rc;

	rc = f_mount(fatfs, "", 0);
	if (rc) {
		printf(" ERROR : f_mount returned %d\r\n", rc);
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int SD_TransferPartial(const char *FileName, u64 DestinationAddress, u32 ByteLength)
{
  FIL fil;
  FRESULT rc;
  UINT br;

  rc = f_open(&fil, FileName, FA_READ);
  if (rc) {
    printf(" ERROR : f_open returned %d\r\n", rc);
    return XST_FAILURE;
  }

  rc = f_lseek(&fil, 0);
  if (rc) {
    printf(" ERROR : f_lseek returned %d\r\n", rc);
    return XST_FAILURE;
  }

  rc = f_read(&fil, (void*) DestinationAddress, ByteLength, &br);
  if (rc) {
    printf(" ERROR : f_read returned %d\r\n", rc);
    return XST_FAILURE;
  }

  rc = f_close(&fil);
  if (rc) {
    printf(" ERROR : f_close returned %d\r\n", rc);
    return XST_FAILURE;
  }

  return XST_SUCCESS;
}

