/*******************************************************************************
** File:
**   novatel_oem615_msg.h
**
** Purpose:
**  Define NOVATEL_OEM615 application commands and telemetry messages
**
*******************************************************************************/
#ifndef _NOVATEL_OEM615_MSG_H_
#define _NOVATEL_OEM615_MSG_H_

#include "cfe.h"
#include "novatel_oem615_device.h"


/*
** Ground Command Codes
** TODO: Add additional commands required by the specific component
*/
#define NOVATEL_OEM615_NOOP_CC                 0
#define NOVATEL_OEM615_RESET_COUNTERS_CC       1
#define NOVATEL_OEM615_ENABLE_CC               2
#define NOVATEL_OEM615_DISABLE_CC              3
#define NOVATEL_OEM615_CONFIG_CC               4


/* 
** Telemetry Request Command Codes
** TODO: Add additional commands required by the specific component
*/
#define NOVATEL_OEM615_REQ_HK_TLM              0
#define NOVATEL_OEM615_REQ_DATA_TLM            1

/*
** Generic "no arguments" command type definition
*/
typedef struct
{
    /* Every command requires a header used to identify it */
    uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];

} NOVATEL_OEM615_NoArgs_cmd_t;


/*
** NOVATEL_OEM615 write configuration command
*/
typedef struct
{
    uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint32   DeviceCfg;

} NOVATEL_OEM615_Config_cmd_t;


/*
** NOVATEL_OEM615 device telemetry definition
*/
typedef struct 
{
    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE];
    NOVATEL_OEM615_Device_Data_tlm_t Novatel_oem615;

} OS_PACK NOVATEL_OEM615_Device_tlm_t;
#define NOVATEL_OEM615_DEVICE_TLM_LNGTH sizeof ( NOVATEL_OEM615_Device_tlm_t )


/*
** NOVATEL_OEM615 housekeeping type definition
*/
typedef struct 
{
    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint8   CommandErrorCount;
    uint8   CommandCount;
    uint8   DeviceErrorCount;
    uint8   DeviceCount;
  
    /*
    ** TODO: Edit and add specific telemetry values to this struct
    */
    uint8   DeviceEnabled;
    NOVATEL_OEM615_Device_HK_tlm_t DeviceHK;

} OS_PACK NOVATEL_OEM615_Hk_tlm_t;
#define NOVATEL_OEM615_HK_TLM_LNGTH sizeof ( NOVATEL_OEM615_Hk_tlm_t )

#endif /* _NOVATEL_OEM615_MSG_H_ */
