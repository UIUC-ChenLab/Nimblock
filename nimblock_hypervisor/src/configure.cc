#include "configure.h"


int reconfigure_slot(XFpga XFpgaInstance, XGpioPs psGpioInstancePtr, Task* task, Slot* slot) {
    // Allocate buffer (aligned to 8)
    // Seems like we can't use posix memalign or aligned alloc...
    char* p = (char*)memalign(8, PARTIAL_BITFILE_ALLOCATED_SPACE*sizeof(char));
    SD_TransferPartial(task->bin_name[slot->id], (u64) p, slot->bin_len);
    /* Start partial reconfiguration */
    /* Decouple */
    XGpioPs_WritePin(&psGpioInstancePtr, slot->decouple_gpio, 1);
    /* Load bitstream */
    int status = XFpga_PL_BitStream_Load(&XFpgaInstance, (UINTPTR)p, slot->bin_len, XFPGA_PARTIAL_EN);
    if(status != XST_SUCCESS) {
        printf("[ERROR] Configuring slot %d failed\n\r", slot->id);
        free(p);
        return 1;
    }
    // Resetting slot and turn decouple off
    XGpioPs_WritePin(&psGpioInstancePtr, slot->decouple_gpio, 0);
    XGpioPs_WritePin(&psGpioInstancePtr,  slot->reset_gpio, 0);
    usleep(100);
    XGpioPs_WritePin(&psGpioInstancePtr,  slot->reset_gpio, 1);
    free(p);
    return 0;
}

void XAccel_Set_Val(XHls_ip *InstancePtr, u64 Data, int offset) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
    XHls_ip_WriteReg(InstancePtr->Axilites_BaseAddress, offset, (u32)Data);
}