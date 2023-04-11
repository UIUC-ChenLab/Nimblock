#include "gpio_user.h"

int init_gpio(XGpioPs_Config* GpioConfigPtr, XGpioPs* psGpioInstancePtr, int xStatus) {
	GpioConfigPtr = XGpioPs_LookupConfig(XPAR_PSU_APM_0_DEVICE_ID);

	if(GpioConfigPtr == NULL) {
		printf("Cannot acquire GPIO config Ptr \n\r");
		return XST_FAILURE;
	}

	xStatus = XGpioPs_CfgInitialize(psGpioInstancePtr,
		  GpioConfigPtr,
		  GpioConfigPtr->BaseAddr);

	if(XST_SUCCESS != xStatus) {
		printf("PS GPIO INIT FAILED \n\r");
		return XST_FAILURE;
	}
	else {
		printf("PS GPIO INIT SUCCEED \n\r");
		return XST_SUCCESS;
	}

}
