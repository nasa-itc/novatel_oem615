/*******************************************************************************
** File: novatel_oem615_device.c
**
** Purpose:
**   This file contains the source code for the NOVATEL_OEM615 device.
**
*******************************************************************************/

/*
** Include Files
*/
#include "novatel_oem615_device.h"

/* 
** Read data from device
*/
int32_t NOVATEL_OEM615_ReadData(int32_t handle, uint8_t* read_data, uint8_t data_length)
{
    int32_t status = OS_SUCCESS;
    int32_t bytes = 0;
    int32_t bytes_available = 0;
    char *token;

    if (data_length < 9)
    {
        #ifdef NOVATEL_OEM615_CFG_DEBUG
            OS_printf("  NOVATEL_OEM615_ReadData: Structure data length provided below expected data header token size! \n");
        #endif
        status = OS_ERROR;
    }
    else
    {
        /* check how many bytes are waiting on the uart */
        bytes_available = uart_bytes_available(handle);
        if (bytes_available > 0)
        {
            uint8_t* temp_read_data = (uint8_t*)calloc(bytes_available, sizeof(uint8_t));
            /* Read all existing data on uart port */
            bytes = uart_read_port(handle, temp_read_data, bytes_available);
            if (bytes != bytes_available)
            {
                #ifdef NOVATEL_OEM615_CFG_DEBUG
                    OS_printf("  NOVATEL_OEM615_ReadData: Bytes read != to requested! \n");
                #endif
                status = OS_ERROR;
                free(temp_read_data);
            }
            else
            {
                /* search uart data for token signifying start of bestxyza gps data packet */
                token = strtok((char*)temp_read_data, ",");
                if ((token != NULL) && (strncmp(token, "#BESTXYZA", 9) == 0)) 
                {
                    OS_printf(" DATA TOKEN FOUND = %s\n", token);
                    for (int i=0;i<data_length;i++)
                    {
                        read_data[i] = *token;
                        token++;
                    }
                }
                else
                {
                    #ifdef NOVATEL_OEM615_CFG_DEBUG
                        OS_printf("  NOVATEL_OEM615_ReadData: No #BESTXYZA token found when reading uart port! \n");
                    #endif
                    status = OS_ERROR;
                    free(temp_read_data);
                }
            }
        }
        else
        {
            #ifdef NOVATEL_OEM615_CFG_DEBUG
                OS_printf("  NOVATEL_OEM615_ReadData: No data available when attempting to read from uart port! \n");
            #endif
            status = OS_ERROR;
        }
    }
    return status;
}

/* 
** Read HK from device
*/
int32_t NOVATEL_OEM615_ReadHK(int32_t handle, uint8_t* read_data, uint8_t data_length)
{
    int32_t status = OS_ERROR;
    int32_t bytes = 0;
    int32_t bytes_available = 0;

    if (data_length < 4)
    {
        #ifdef NOVATEL_OEM615_CFG_DEBUG
            OS_printf("  NOVATEL_OEM615_ReadHK: Structure data length provided below expected HK header + trailer size! \n");
        #endif
    }
    else
    {
        /* check how many bytes are waiting on the uart */
        bytes_available = uart_bytes_available(handle);
        if (bytes_available > 0)
        {
            uint8_t* temp_read_data = (uint8_t*)calloc(bytes_available, sizeof(uint8_t));
            /* Read all existing data on uart port */
            bytes = uart_read_port(handle, temp_read_data, bytes_available);
            if (bytes != bytes_available)
            {
                free(temp_read_data);
                #ifdef NOVATEL_OEM615_CFG_DEBUG
                    OS_printf("  NOVATEL_OEM615_ReadHK: Bytes read != to requested! \n");
                #endif
            }
            else
            {
                /* search uart data for header+trailer signifying start of HK gps packet */
                int j = 0;
                for (int i=0;i<(bytes-data_length);i++)
                {
                    if ((temp_read_data[i]  == NOVATEL_OEM615_DEVICE_HDR_0)     && 
                    (temp_read_data[i+1]  == NOVATEL_OEM615_DEVICE_HDR_1)     && 
                    (temp_read_data[i+data_length-2] == NOVATEL_OEM615_DEVICE_TRAILER_0) && 
                    (temp_read_data[i+data_length-1] == NOVATEL_OEM615_DEVICE_TRAILER_1) )
                    {
                        OS_printf(" HK HEADER + TRAILER FOUND \n");
                        for (int j=0;j<data_length;j++)
                        {
                            read_data[j] = read_data[i+j];
                        }
                        status = OS_SUCCESS;
                        //break;
                        return status;
                    }
                }
                if (status != OS_SUCCESS)
                {
                    free(temp_read_data);
                    #ifdef NOVATEL_OEM615_CFG_DEBUG
                        OS_printf("  NOVATEL_OEM615_ReadHK: No DEAD header + BEEF trailer found when reading uart port! \n");
                    #endif
                }
            }
        }
        else
        {
            #ifdef NOVATEL_OEM615_CFG_DEBUG
                OS_printf("  NOVATEL_OEM615_ReadHK: No data available when attempting to read from uart port! \n");
            #endif
        }
    }
    return status;
}

/*
** Request housekeeping command
*/
int32_t NOVATEL_OEM615_RequestHK(int32_t handle, NOVATEL_OEM615_Device_HK_tlm_t* data)
{
    int32_t status = OS_SUCCESS;
    uint8_t read_data[NOVATEL_OEM615_DEVICE_HK_SIZE] = {0};

    /* Command device to send HK */
    //status = NOVATEL_OEM615_CommandDevice(handle, NOVATEL_OEM615_DEVICE_REQ_HK_CMD, 0);
    //if (status == OS_SUCCESS)
    //{
        /* Read HK data */
        status = NOVATEL_OEM615_ReadHK(handle, read_data, sizeof(read_data));
        if (status == OS_SUCCESS)
        {
            //#ifdef NOVATEL_OEM615_CFG_DEBUG
                OS_printf("  NOVATEL_OEM615_RequestHK = ");
                for (uint32_t i = 0; i < sizeof(read_data); i++)
                {
                    OS_printf("%02x", read_data[i]);
                }
                OS_printf("\n");
            //#endif

            /* Verify data header and trailer */
            if ((read_data[0]  == NOVATEL_OEM615_DEVICE_HDR_0)     && 
                (read_data[1]  == NOVATEL_OEM615_DEVICE_HDR_1)     && 
                (read_data[14] == NOVATEL_OEM615_DEVICE_TRAILER_0) && 
                (read_data[15] == NOVATEL_OEM615_DEVICE_TRAILER_1) )
            {
                data->DeviceCounter  = read_data[2] << 24;
                data->DeviceCounter |= read_data[3] << 16;
                data->DeviceCounter |= read_data[4] << 8;
                data->DeviceCounter |= read_data[5];

                data->DeviceConfig  = read_data[6] << 24;
                data->DeviceConfig |= read_data[7] << 16;
                data->DeviceConfig |= read_data[8] << 8;
                data->DeviceConfig |= read_data[9];

                data->DeviceStatus  = read_data[10] << 24;
                data->DeviceStatus |= read_data[11] << 16;
                data->DeviceStatus |= read_data[12] << 8;
                data->DeviceStatus |= read_data[13];

                //#ifdef NOVATEL_OEM615_CFG_DEBUG
                    OS_printf("  Header  = 0x%02x%02x  \n", read_data[0], read_data[1]);
                    OS_printf("  Counter = 0x%08x      \n", data->DeviceCounter);
                    OS_printf("  Config  = 0x%08x      \n", data->DeviceConfig);
                    OS_printf("  Status  = 0x%08x      \n", data->DeviceStatus);
                    OS_printf("  Trailer = 0x%02x%02x  \n", read_data[14], read_data[15]);
                //#endif
            }
            else
            {
                #ifdef NOVATEL_OEM615_CFG_DEBUG
                    OS_printf("  NOVATEL_OEM615_RequestHK: NOVATEL_OEM615_ReadData returned data with either invalid header [0x%02x%02x] or invalid trailer [0x%02x%02x]!\n", read_data[0], read_data[1], read_data[14], read_data[15]);
                #endif 
                status = OS_ERROR;
            }
        }
        else
        {
            #ifdef NOVATEL_OEM615_CFG_DEBUG
                OS_printf("  NOVATEL_OEM615_RequestHK: Invalid data read from NOVATEL_OEM615_ReadHK!\n");
            #endif 
            status = OS_ERROR;
        } /* NOVATEL_OEM615_ReadData */
    /*
    }
    else
    {
        #ifdef NOVATEL_OEM615_CFG_DEBUG
            OS_printf("  NOVATEL_OEM615_RequestHK: NOVATEL_OEM615_CommandDevice reported error %d \n", status);
        #endif 
    }
    */
    return status;
}

/*
** Request data command for child task
*/
int32_t NOVATEL_OEM615_RequestDataChild(int32_t handle, NOVATEL_OEM615_Device_Data_tlm_t* data)
{
    int32_t status = OS_SUCCESS;
    uint8_t read_data[NOVATEL_OEM615_DEVICE_DATA_SIZE] = {0};

    /* Read HK data */
    status = NOVATEL_OEM615_ReadData(handle, read_data, sizeof(read_data));
    if (status == OS_SUCCESS)
    {
        
        //#ifdef NOVATEL_OEM615_CFG_DEBUG
            OS_printf("  NOVATEL_OEM615_RequestDataChild = ");
            for (uint32_t i = 0; i < sizeof(read_data); i++)
            {
                OS_printf("%02x", read_data[i]);
            }
            OS_printf("\n");
        //#endif
        
        NOVATEL_OEM615_ParseBestXYZA(data, read_data, sizeof(read_data));

    } 
    else
    {
        #ifdef NOVATEL_OEM615_CFG_DEBUG
            OS_printf("  NOVATEL_OEM615_RequestDataChild: Invalid data read from NOVATEL_OEM615_ReadData!\n");
        #endif 
        status = OS_ERROR;
    } /* NOVATEL_OEM615_ReadData */
    return status;
}

/************************************************************************
** Parse NovAtel OEM615 BESTXYZA log data
*************************************************************************/
// Reference:  Section 1.1.1, p. 24, OEM6 Family Firmware Reference Manual, OM-20000129, Rev 8, January 2015 (file om-20000129.pdf)
// Reference:  Section 3.2.17, pp. 420-422, OEM6 Family Firmware Reference Manual, OM-20000129, Rev 8, January 2015 (file om-20000129.pdf)
void NOVATEL_OEM615_ParseBestXYZA(NOVATEL_OEM615_Device_Data_tlm_t* device_data_struct, uint8_t data_buf[], int32_t data_len)
{
    device_data_struct->Weeks = 0;
    device_data_struct->SecondsIntoWeek = 0;
    device_data_struct->Fractions = 0.0;
    device_data_struct->ECEFX = 0.0;
    device_data_struct->ECEFY = 0.0;
    device_data_struct->ECEFZ = 0.0;
    device_data_struct->VelX = 0.0;
    device_data_struct->VelY = 0.0;
    device_data_struct->VelZ = 0.0;
    char *token;
    char *saveptr;

    if (data_len != 0) {
        token = strtok_r((char*)data_buf, ", ", &saveptr);
        OS_printf(" DELETE: NOVATEL_OEM615_ParseBextXYZA token = %s\n", token);
        if ((token != NULL) && (strncmp(token, "#BESTXYZA", 9) == 0)) {
            OS_printf("  NOVATEL_OEM615_ParseBestXYZA = ");
            for (int32_t i = 0; i < data_len; i++)
            {
                OS_printf("%02x", data_buf[i]);
            }
            OS_printf("\n");
            // Got a valid BESTXYZA log message - now the header
            token = strtok_r(NULL, ",", &saveptr); // Port
            OS_printf(" DELETE2: NOVATEL_OEM615_ParseBextXYZA token = %s\n", token);
            token = strtok_r(NULL, ",", &saveptr); // Sequence #
            OS_printf(" DELETE3: NOVATEL_OEM615_ParseBextXYZA token = %s\n", token);
            token = strtok_r(NULL, ",", &saveptr); // % Idle Time
            OS_printf(" DELETE4: NOVATEL_OEM615_ParseBextXYZA token = %s\n", token);
            token = strtok_r(NULL, ",", &saveptr); // Time Status
            OS_printf(" DELETE5: NOVATEL_OEM615_ParseBextXYZA token = %s\n", token);
            token = strtok_r(NULL, ",", &saveptr); // Week
            OS_printf(" DELETE6: NOVATEL_OEM615_ParseBextXYZA token = %s\n", token);
            if (token != NULL) device_data_struct->Weeks = atol(token);
            token = strtok_r(NULL, ",", &saveptr); // Seconds
            if (token != NULL) {
                device_data_struct->Fractions = atof(token);
                device_data_struct->SecondsIntoWeek = (uint32_t)device_data_struct->Fractions;
                device_data_struct->Fractions -= device_data_struct->SecondsIntoWeek;
            }
            token = strtok_r(NULL, ",", &saveptr); // Receiver Status
            token = strtok_r(NULL, ",", &saveptr); // Reserved
            token = strtok_r(NULL, ";", &saveptr); // Receiver s/w Version

            // Now the data
            token = strtok_r(NULL, ",", &saveptr); // P-sol status
            token = strtok_r(NULL, ",", &saveptr); // pos type
            token = strtok_r(NULL, ",", &saveptr); // P-X (m)
            if (token != NULL) device_data_struct->ECEFX = atof(token);
            token = strtok_r(NULL, ",", &saveptr); // P-Y (m)
            if (token != NULL) device_data_struct->ECEFY = atof(token);
            token = strtok_r(NULL, ",", &saveptr); // P-Z (m)
            if (token != NULL) device_data_struct->ECEFZ = atof(token);
            token = strtok_r(NULL, ",", &saveptr); // P-X sigma
            token = strtok_r(NULL, ",", &saveptr); // P-Y sigma
            token = strtok_r(NULL, ",", &saveptr); // P-Z sigma
            token = strtok_r(NULL, ",", &saveptr); // V-sol status
            token = strtok_r(NULL, ",", &saveptr); // vel type
            token = strtok_r(NULL, ",", &saveptr); // V-X (m/s
            if (token != NULL) device_data_struct->VelX = atof(token);
            token = strtok_r(NULL, ",", &saveptr); // V-Y (m/s)
            if (token != NULL) device_data_struct->VelY = atof(token);
            token = strtok_r(NULL, ",", &saveptr); // V-Z (m/s)
            if (token != NULL) device_data_struct->VelZ = atof(token);
        }
    }

    //#ifdef NOVATEL_OEM615_CFG_DEBUG
        OS_printf("  Weeks = %d\n", device_data_struct->Weeks);
        OS_printf("  SecondsIntoWeek = %d\n", device_data_struct->SecondsIntoWeek);
        OS_printf("  Fractions = %d\n", device_data_struct->Fractions);
        OS_printf("  ECEFX = %d\n", device_data_struct->ECEFX);
        OS_printf("  ECEFY = %d\n", device_data_struct->ECEFY);
        OS_printf("  ECEFZ = %d\n", device_data_struct->ECEFZ);
        OS_printf("  VelX = %d\n", device_data_struct->VelX);
        OS_printf("  VelY = %d\n", device_data_struct->VelY);
        OS_printf("  VelZ = %d\n", device_data_struct->VelZ);
    //#endif

} /* NAV_ParseOEM615Bestxyza */

/*
** Request data command
*/
/*
int32_t NOVATEL_OEM615_RequestData(int32_t handle, NOVATEL_OEM615_Device_Data_tlm_t* data)
{
    int32_t status = OS_SUCCESS;
    uint8_t read_data[NOVATEL_OEM615_DEVICE_DATA_SIZE] = {0};

    // Command device to send data 
    status = NOVATEL_OEM615_CommandDevice(handle, NOVATEL_OEM615_DEVICE_REQ_DATA_CMD, 0);
    if (status == OS_SUCCESS)
    {
        // Read HK data 
        status = NOVATEL_OEM615_ReadData(handle, read_data, sizeof(read_data));
        if (status == OS_SUCCESS)
        {
            #ifdef NOVATEL_OEM615_CFG_DEBUG
                OS_printf("  NOVATEL_OEM615_RequestData = ");
                for (uint32_t i = 0; i < sizeof(read_data); i++)
                {
                    OS_printf("%02x", read_data[i]);
                }
                OS_printf("\n");
            #endif

            NOVATEL_OEM615_ParseBestXYZA(data, read_data, NOVATEL_OEM615_DEVICE_DATA_SIZE);

        } 
        else
        {
            #ifdef NOVATEL_OEM615_CFG_DEBUG
                OS_printf("  NOVATEL_OEM615_RequestData: Invalid data read from NOVATEL_OEM615_ReadData!\n");
            #endif 
            status = OS_ERROR;
        } // NOVATEL_OEM615_ReadData 
    }
    else
    {
        #ifdef NOVATEL_OEM615_CFG_DEBUG
            OS_printf("  NOVATEL_OEM615_RequestData: NOVATEL_OEM615_CommandDevice reported error %d \n", status);
        #endif 
    }
    return status;
}
*/
/* 
** Command to device
** Note that confirming the echoed response is specific to this implementation
*/
/*
int32_t NOVATEL_OEM615_CommandDevice(int32_t handle, uint8_t cmd_code, uint32_t payload)
{
    int32_t status = OS_SUCCESS;
    int32_t bytes = 0;
    uint8_t write_data[NOVATEL_OEM615_DEVICE_CMD_SIZE] = {0};
    uint8_t read_data[NOVATEL_OEM615_DEVICE_DATA_SIZE] = {0};

    payload = CFE_MAKE_BIG32(payload);

    // Prepare command
    write_data[0] = NOVATEL_OEM615_DEVICE_HDR_0;
    write_data[1] = NOVATEL_OEM615_DEVICE_HDR_1;
    write_data[2] = cmd_code;
    write_data[3] = payload >> 24;
    write_data[4] = payload >> 16;
    write_data[5] = payload >> 8;
    write_data[6] = payload;
    write_data[7] = NOVATEL_OEM615_DEVICE_TRAILER_0;
    write_data[8] = NOVATEL_OEM615_DEVICE_TRAILER_1;

    // Flush any prior data 
    status = uart_flush(handle);
    if (status == UART_SUCCESS)
    {
        // Write data 
        bytes = uart_write_port(handle, write_data, NOVATEL_OEM615_DEVICE_CMD_SIZE);
        #ifdef NOVATEL_OEM615_CFG_DEBUG
            OS_printf("  NOVATEL_OEM615_CommandDevice[%d] = ", bytes);
            for (uint32_t i = 0; i < NOVATEL_OEM615_DEVICE_CMD_SIZE; i++)
            {
                OS_printf("%02x", write_data[i]);
            }
            OS_printf("\n");
        #endif
        if (bytes == NOVATEL_OEM615_DEVICE_CMD_SIZE)
        {
            status = NOVATEL_OEM615_ReadData(handle, read_data, NOVATEL_OEM615_DEVICE_CMD_SIZE);
            if (status == OS_SUCCESS)
            {
                // Confirm echoed response 
                bytes = 0;
                //OS_printf("DELETE: WE ARE ECHOING RESPONSE\n");
                while ((bytes < (int32_t) NOVATEL_OEM615_DEVICE_CMD_SIZE) && (status == OS_SUCCESS))
                {
                    //OS_printf("DELETE: DURING ECHO read_data[%d] = %d; write_data[%d] = %d\n", bytes, read_data[bytes], bytes, write_data[bytes]);
                    if (read_data[bytes] != write_data[bytes])
                    {
                        status = OS_ERROR;
                    }
                    bytes++;
                }
                //OS_printf("DELETE: AFTER ECHOING, OUR STATUS IS %d\n", status);
            } // NOVATEL_OEM615_ReadData 
            else
            {
                #ifdef NOVATEL_OEM615_CFG_DEBUG
                    OS_printf("NOVATEL_OEM615_CommandDevice - NOVATEL_OEM615_ReadData returned %d \n", status);
                #endif
            }
        } 
        else
        {
            #ifdef NOVATEL_OEM615_CFG_DEBUG
                OS_printf("NOVATEL_OEM615_CommandDevice - uart_write_port returned %d, expected %d \n", bytes, NOVATEL_OEM615_DEVICE_CMD_SIZE);
            #endif
        } // uart_write 
    } // uart_flush
    return status;
}
*/