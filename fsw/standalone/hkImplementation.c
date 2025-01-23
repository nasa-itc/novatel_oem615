
                if (status == OS_SUCCESS) 
                {
                    uint8_t data_length = sizeof(read_data);
                    status = OS_ERROR;
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
                        bytes_available = uart_bytes_available(&Novatel_oem615Uart);
                        if (bytes_available > 0)
                        {
                            uint8_t* temp_read_data = (uint8_t*)calloc(bytes_available, sizeof(uint8_t));
                            /* Read all existing data on uart port */
                            bytes = uart_read_port(&Novatel_oem615Uart, temp_read_data, bytes_available);
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
                                for (int i=0;i<(bytes-data_length);i++)
                                {
                                    if ((temp_read_data[i]  == NOVATEL_OEM615_DEVICE_HDR_0)     && 
                                    (temp_read_data[i+1]  == NOVATEL_OEM615_DEVICE_HDR_1)     && 
                                    (temp_read_data[i+data_length-2] == NOVATEL_OEM615_DEVICE_TRAILER_0) && 
                                    (temp_read_data[i+data_length-1] == NOVATEL_OEM615_DEVICE_TRAILER_1) )
                                    {
                                        OS_printf("made it to evil for loop");
                                        for (int j=0;j<data_length;j++)
                                        {
                                            read_data[j] = read_data[i+j];
                                        }
                                        status = OS_SUCCESS;
                                        return status;
                                    }
                                    if(status != OS_SUCCESS)
                                    {
                                        free(temp_read_data);
                                        #ifdef NOVATEL_OEM615_CFG_DEBUG
                                            OS_printf("  NOVATEL_OEM615_ReadHK: No DEAD header + BEEF trailer found when reading uart port! \n");
                                        #endif
                                    }
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
                    if (status == OS_SUCCESS)
                    {
                        #ifdef NOVATEL_OEM615_CFG_DEBUG
                            OS_printf("  NOVATEL_OEM615_RequestHK = ");
                            for (uint32_t i = 0; i < sizeof(read_data); i++)
                            {
                                OS_printf("%02x", read_data[i]);
                            }
                            OS_printf("\n");
                        #endif

                        /* Verify data header and trailer */
                        if ((read_data[0]  == NOVATEL_OEM615_DEVICE_HDR_0)     && 
                            (read_data[1]  == NOVATEL_OEM615_DEVICE_HDR_1)     && 
                            (read_data[14] == NOVATEL_OEM615_DEVICE_TRAILER_0) && 
                            (read_data[15] == NOVATEL_OEM615_DEVICE_TRAILER_1) )
                        {
                            Novatel_oem615HK.DeviceCounter  = read_data[2] << 24;
                            Novatel_oem615HK.DeviceCounter |= read_data[3] << 16;
                            Novatel_oem615HK.DeviceCounter |= read_data[4] << 8;
                            Novatel_oem615HK.DeviceCounter |= read_data[5];

                            Novatel_oem615HK.DeviceConfig  = read_data[6] << 24;
                            Novatel_oem615HK.DeviceConfig |= read_data[7] << 16;
                            Novatel_oem615HK.DeviceConfig |= read_data[8] << 8;
                            Novatel_oem615HK.DeviceConfig |= read_data[9];

                            Novatel_oem615HK.DeviceStatus  = read_data[10] << 24;
                            Novatel_oem615HK.DeviceStatus |= read_data[11] << 16;
                            Novatel_oem615HK.DeviceStatus |= read_data[12] << 8;
                            Novatel_oem615HK.DeviceStatus |= read_data[13];

                            #ifdef NOVATEL_OEM615_CFG_DEBUG
                                OS_printf("  Header  = 0x%02x%02x  \n", read_data[0], read_data[1]);
                                OS_printf("  Counter = 0x%08x      \n", Novatel_oem615HK.DeviceCounter);
                                OS_printf("  Config  = 0x%08x      \n", Novatel_oem615HK.DeviceConfig);
                                OS_printf("  Status  = 0x%08x      \n", Novatel_oem615HK.DeviceStatus);
                                OS_printf("  Trailer = 0x%02x%02x  \n", read_data[14], read_data[15]);
                            #endif
                        }
                        else
                        {
                            #ifdef NOVATEL_OEM615_CFG_DEBUG
                                OS_printf("  NOVATEL_OEM615_RequestHK: NOVATEL_OEM615_ReadHK returned data with either invalid header [0x%02x%02x] or invalid trailer [0x%02x%02x]!\n", read_data[0], read_data[1], read_data[14], read_data[15]);
                            #endif 
                            status = OS_ERROR;
                        }
                    }
                }

