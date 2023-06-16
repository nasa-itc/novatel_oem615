/*******************************************************************************
** File: novatel_oem615_device.h
**
** Purpose:
**   This is the header file for the NOVATEL_OEM615 device.
**
*******************************************************************************/
#ifndef _NOVATEL_OEM615_DEVICE_H_
#define _NOVATEL_OEM615_DEVICE_H_

/*
** Required header files.
*/
#include "hwlib.h"
#include "novatel_oem615_platform_cfg.h"
//#include "novatel_oem615_child.h"

/*
** Type definitions
** TODO: Make specific to your application
*/
#define NOVATEL_OEM615_DEVICE_HDR              0xDEAD
#define NOVATEL_OEM615_DEVICE_HDR_0            0xDE
#define NOVATEL_OEM615_DEVICE_HDR_1            0xAD

#define NOVATEL_OEM615_DEVICE_NOOP_CMD         0x00
#define NOVATEL_OEM615_DEVICE_REQ_HK_CMD       0x01
#define NOVATEL_OEM615_DEVICE_REQ_DATA_CMD     0x02
#define NOVATEL_OEM615_DEVICE_CFG_CMD          0x03

#define NOVATEL_OEM615_DEVICE_TRAILER          0xBEEF
#define NOVATEL_OEM615_DEVICE_TRAILER_0        0xBE
#define NOVATEL_OEM615_DEVICE_TRAILER_1        0xEF

#define NOVATEL_OEM615_DEVICE_HDR_TRL_LEN      4
#define NOVATEL_OEM615_DEVICE_CMD_SIZE         9

/*
** NOVATEL_OEM615 device housekeeping telemetry definition
*/
typedef struct
{
    uint32_t  DeviceCounter;
    uint32_t  DeviceConfig;
    uint32_t  DeviceStatus;

} OS_PACK NOVATEL_OEM615_Device_HK_tlm_t;
#define NOVATEL_OEM615_DEVICE_HK_LNGTH sizeof ( NOVATEL_OEM615_Device_HK_tlm_t )
#define NOVATEL_OEM615_DEVICE_HK_SIZE NOVATEL_OEM615_DEVICE_HK_LNGTH + NOVATEL_OEM615_DEVICE_HDR_TRL_LEN


/*
** NOVATEL_OEM615 device data telemetry definition
*/
typedef struct
{
    uint16_t Weeks;
    uint32_t SecondsIntoWeek;
    double Fractions;
    double ECEFX;
    double ECEFY;
    double ECEFZ;
    double VelX;
    double VelY;
    double VelZ;

} OS_PACK NOVATEL_OEM615_Device_Data_tlm_t;
#define NOVATEL_OEM615_DEVICE_DATA_LNGTH sizeof ( NOVATEL_OEM615_Device_Data_tlm_t )
#define NOVATEL_OEM615_DEVICE_DATA_SIZE NOVATEL_OEM615_DEVICE_DATA_LNGTH + NOVATEL_OEM615_DEVICE_HDR_TRL_LEN


/*
** Prototypes
*/
int32_t NOVATEL_OEM615_ReadData(int32_t handle, uint8_t* read_data, uint8_t data_length);
int32_t NOVATEL_OEM615_ReadHK(int32_t handle, uint8_t* read_data, uint8_t data_length);
//int32_t NOVATEL_OEM615_CommandDevice(int32_t handle, uint8_t cmd, uint32_t payload);
int32_t NOVATEL_OEM615_RequestHK(int32_t handle, NOVATEL_OEM615_Device_HK_tlm_t* data);
//int32_t NOVATEL_OEM615_RequestData(int32_t handle, NOVATEL_OEM615_Device_Data_tlm_t* data);
int32_t NOVATEL_OEM615_RequestDataChild(int32_t handle, NOVATEL_OEM615_Device_Data_tlm_t* data);
void NOVATEL_OEM615_ParseBestXYZA(NOVATEL_OEM615_Device_Data_tlm_t* device_data_struct, uint8_t data_buf[], int32_t data_len);

#endif /* _NOVATEL_OEM615_DEVICE_H_ */
