/*******************************************************************************
** File: novatel_oem615_child.c
**
** Purpose:
**   This file contains the source code for the NOVATEL_OEM615 child task.
**
*******************************************************************************/

/*
** Include Files
*/
#include "novatel_oem615_child.h"

/* 
** Run child task until stopped
*/
void NOVATEL_OEM615_ChildTask(void)
{
    int32  result;
    uint32 read_timeout = NOVATEL_OEM615_READ_TIMEOUT;

    result = CFE_ES_RegisterChildTask();
    if (result != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(NOVATEL_OEM615_CHILDTASKINIT_ERR_EID, CFE_EVS_ERROR, "NOVATEL_OEM615: reg child task error %d", result);
        CFE_ES_ExitChildTask();
        return;
    }
    else
    {
        CFE_EVS_SendEvent(NOVATEL_OEM615_CHILDTASKINIT_INF_EID, CFE_EVS_INFORMATION, "NOVATEL_OEM615: child task started, runStatus = %d", NOVATEL_OEM615_AppData.RunStatus);
    }

    while (NOVATEL_OEM615_GetRunStatus() == CFE_ES_APP_RUN)
    {
        NOVATEL_OEM615_ProcessData();
    }

    /* This call allows cFE to clean-up system resources */
    CFE_EVS_SendEvent(NOVATEL_OEM615_CHILDTASKEXIT_INF_EID, CFE_EVS_INFORMATION,
        "NOVATEL_OEM615 child task exit complete");
    CFE_ES_ExitChildTask();
}

void NOVATEL_OEM615_ProcessData(void)
{
    const uint32 sleeptime = NOVATEL_OEM615_READ_TIMEOUT;
    int32 status = CFE_SUCCESS;
    if (NOVATEL_OEM615_GetDeviceEnabledStatus() == NOVATEL_OEM615_DEVICE_ENABLED)
    {
        status = NOVATEL_OEM615_SafeRequestDataChild(NOVATEL_OEM615_AppData.Novatel_oem615Uart.handle, (NOVATEL_OEM615_Device_Data_tlm_t*) &NOVATEL_OEM615_AppData.DevicePkt.Novatel_oem615);
        if (status != CFE_SUCCESS)
        {
            #ifdef NOVATEL_OEM615_CFG_DEBUG
                CFE_EVS_SendEvent(NOVATEL_OEM615_UART_READ_ERR_EID, CFE_EVS_ERROR, 
                    "NOVATEL_OEM615: Device read error.  NOVATEL_OEM615_SafeRequestDataChild returned %d.", status);
            #endif
        }
    }
    OS_TaskDelay(sleeptime);
}