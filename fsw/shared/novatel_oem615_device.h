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
#include "device_cfg.h"
#include "hwlib.h"
#include "novatel_oem615_platform_cfg.h"


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

} __attribute__((packed)) NOVATEL_OEM615_Device_HK_tlm_t;
#define NOVATEL_OEM615_DEVICE_HK_LNGTH sizeof ( NOVATEL_OEM615_Device_HK_tlm_t )
#define NOVATEL_OEM615_DEVICE_HK_SIZE NOVATEL_OEM615_DEVICE_HK_LNGTH + NOVATEL_OEM615_DEVICE_HDR_TRL_LEN


/*
** NOVATEL_OEM615 device data telemetry definition
*/
typedef struct
{
    uint32_t  DeviceCounter;
    uint16_t  DeviceDataX;
    uint16_t  DeviceDataY;
    uint16_t  DeviceDataZ;

} __attribute__((packed)) NOVATEL_OEM615_Device_Data_tlm_t;
#define NOVATEL_OEM615_DEVICE_DATA_LNGTH sizeof ( NOVATEL_OEM615_Device_Data_tlm_t )
#define NOVATEL_OEM615_DEVICE_DATA_SIZE NOVATEL_OEM615_DEVICE_DATA_LNGTH + NOVATEL_OEM615_DEVICE_HDR_TRL_LEN


/*
** Prototypes
*/
int32_t NOVATEL_OEM615_ReadData(uart_info_t* device, uint8_t* read_data, uint8_t data_length);
int32_t NOVATEL_OEM615_CommandDevice(uart_info_t* device, uint8_t cmd, uint32_t payload);
int32_t NOVATEL_OEM615_RequestHK(uart_info_t* device, NOVATEL_OEM615_Device_HK_tlm_t* data);
int32_t NOVATEL_OEM615_RequestData(uart_info_t* device, NOVATEL_OEM615_Device_Data_tlm_t* data);


#endif /* _NOVATEL_OEM615_DEVICE_H_ */
