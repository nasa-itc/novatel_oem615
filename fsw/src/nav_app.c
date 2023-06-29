/*******************************************************************************
** File: nav_app.c
**
** Purpose:
**   Navigation (NAV) App source
**
*******************************************************************************/


/************************************************************************
** Includes
*************************************************************************/
#include <string.h>
#include "nav_app.h"
#include "nav_perfids.h"
#include "nav_msgids.h"
#include "nav_events.h"
#include "nav_msg.h"
#include "nav_version.h"
#include "hwlib.h"


/************************************************************************
** Macro Definitions
*************************************************************************/


/************************************************************************
** NAV global data
*************************************************************************/
NAV_AppData_t     NAV_AppData;

/************************************************************************
** Forward declaration
*************************************************************************/
static void NAV_AppInit(void);
static void NAV_ProcessCommandPacket(uint8 CmdCode);
static void NAV_ReportHousekeeping(void);
static void NAV_ResetCounters(void);
static GPSSerialiation NAV_ParseOEM615Bestxyza(uint8_t *DataBuf, int32 DataLen);
static void NAV_ReadAvailableData(uint8 DataBuffer[], int32 *DataLen);

/************************************************************************
** NAV application entry point and main process loop
*************************************************************************/
void novatel_oem615_AppMain( void )
{
    int32 Status;
    uint8 CmdCode;
    NAV_AppData.RunStatus = CFE_ES_APP_RUN;

    CFE_ES_PerfLogEntry(NAV_APP_PERF_ID);

    NAV_AppInit();

    while(CFE_ES_RunLoop(&NAV_AppData.RunStatus) == TRUE)
    {
    	CFE_ES_PerfLogExit(NAV_APP_PERF_ID);

        Status = CFE_SB_RcvMsg(&NAV_AppData.MsgPtr, NAV_AppData.CmdPipe, CFE_SB_PEND_FOREVER);
        CFE_ES_PerfLogEntry(NAV_APP_PERF_ID);

        if(Status == CFE_SUCCESS)
        {
        	CFE_SB_MsgId_t id;
        	id = CFE_SB_GetMsgId(NAV_AppData.MsgPtr);
            CFE_EVS_SendEvent(NAV_CMD_ERR_EID, CFE_EVS_DEBUG, "NAV: command packet (id=0x%x)", id);
        	switch(id)
        	{
                /* Process Ground Commands: Noops, Req NAV Data, Etc */
                case NAV_GROUND_CMD_MID:
                    CmdCode = CFE_SB_GetCmdCode(NAV_AppData.MsgPtr);
                    CFE_EVS_SendEvent(NAV_CMD_ERR_EID, CFE_EVS_DEBUG, "NAV: ground command code (CmdCode=0x%x)", CmdCode);
                    NAV_ProcessCommandPacket(CmdCode);
                    break;

                /* Process cFE messages, such as those from SCH */
                case NAV_CMD_REQ_NAV_SCH_MID:
                    CmdCode = NAV_REQ_DATA_CC;  /* this message is from scheduler so manually set the command code to request data */
                    CFE_EVS_SendEvent(NAV_CMD_ERR_EID, CFE_EVS_DEBUG, "NAV: cFE command code (CmdCode=0x%x)", CmdCode);
                    NAV_ProcessCommandPacket(CmdCode);
                    break;

        		default:
                    CFE_EVS_SendEvent(NAV_CMD_ERR_EID, CFE_EVS_INFORMATION,"NAV: invalid command packet (id=0x%x)", id);
                	NAV_AppData.hk.cmd_error_count++;
                    break;
        	}
        }
        else
        {
        	OS_printf("NAV: CFE_SB_RcvMsg error!\n");
        }
    }

    CFE_ES_ExitApp(NAV_AppData.RunStatus);

} /* End of NAV_AppMain() */


static char deviceName[] = "/dev/tty1";
static uart_info_t NAV_UART = {.deviceString = &deviceName[0], .handle = 1, .isOpen = PORT_CLOSED, .baud = 115200};

/************************************************************************
** NAV application initialization
*************************************************************************/
static void NAV_AppInit(void)
{
	int32 status;

    status = uart_init_port(&NAV_UART);
    if(status != CFE_SUCCESS)
    {
    	OS_printf("NAV: UART port initialization error!\n");
    }    
    
    /* Register the app with Executive services */
    CFE_ES_RegisterApp() ;

    /* Register the events */
    CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);

    /* Create NAV command pipe */
    status = CFE_SB_CreatePipe(&NAV_AppData.CmdPipe, NAV_PIPE_DEPTH, NAV_PIPE_NAME);
    if(status != CFE_SUCCESS)
    {
    	OS_printf("NAV: CFE_SB_CreatePipe error!\n");
    }

    /* Subscribe to NAV ground commands */
    status = CFE_SB_Subscribe(NAV_GROUND_CMD_MID, NAV_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        OS_printf("NAV: CFE_SB_Subscribe error (id=%i)!\n", NAV_GROUND_CMD_MID);
    }

    /* Subscribe to NAV Req Data scheduler commands */
    status = CFE_SB_Subscribe(NAV_CMD_REQ_NAV_SCH_MID, NAV_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        OS_printf("NAV: CFE_SB_Subscribe error (id=%i)!\n", NAV_CMD_REQ_NAV_SCH_MID);
    }

    /* Initialize Counters */
    NAV_ResetCounters();

    /* init hk msg  - this message contains all telemetry */
    CFE_SB_InitMsg(&NAV_AppData.hk, NAV_SEND_HK_TLM, NAV_HK_TLM_LENGTH, TRUE);

    /* NAV initialization event */
    CFE_EVS_SendEvent(NAV_INIT_EID, CFE_EVS_INFORMATION,
                      "NAV Initialized. Version %d.%d.%d.%d",
                      NAV_MAJOR_VERSION,
                      NAV_MINOR_VERSION,
                      NAV_REVISION,
                      NAV_MISSION_REV);

} /* End of NAV_AppInit() */


/************************************************************************
** Process command pipe message
*************************************************************************/
static void NAV_ProcessCommandPacket(uint8 CmdCode)
{

    uint8_t DataBuffer[NAV_BUFFER_SIZE] = {0};
    int32 DataLen;
    GPSSerialiation GPSData;

	/* increase the command counter for successful command */
	NAV_AppData.hk.cmd_count++;

	switch(CmdCode)
	{
		/* NOOP Command 			*/
		case NAV_NOOP_CC:
			CFE_EVS_SendEvent(NAV_CMD_NOOP_EID, CFE_EVS_INFORMATION, "NAV NOOP command");
			break; 

		/* Request NAV data 		*/
		case NAV_REQ_DATA_CC:

            CFE_EVS_SendEvent(NAV_CMD_REQ_DATA_EID, CFE_EVS_DEBUG,"Request NAV GPS Data");

            /* Read the GPS data from the UART */
            NAV_ReadAvailableData(DataBuffer, &DataLen);
            GPSData = NAV_ParseOEM615Bestxyza(DataBuffer, DataLen);
            memcpy(&NAV_AppData.hk.gps_data, &GPSData, sizeof(GPSSerialiation));

            //CFE_EVS_SendEvent(NAV_CMD_REQ_DATA_EID, CFE_EVS_INFORMATION,
            //    "GPS Week = %lu, Seconds = %lu, GPS Fraction = %f, "
            //    "GPS ECEF_X = %f, ECEF_Y = %f, ECEF_Z = %f, "
            //    "GPS X Vel = %f, Y Vel = %f, Z Vel = %f",
            //    (unsigned long)GPSData.weeks, (unsigned long)GPSData.seconds_into_week, GPSData.fractions,
            //    GPSData.ECEF_X, GPSData.ECEF_Y, GPSData.ECEF_Z,
            //    GPSData.vel_x, GPSData.vel_y, GPSData.vel_z);

			/* publish the HK message which includes NAV data */
			NAV_ReportHousekeeping();
			break;

		/* Reset all app counters 	*/
		case NAV_RST_COUNTERS_CC:
			CFE_EVS_SendEvent(NAV_CMD_RST_COUNTERS_EID , CFE_EVS_INFORMATION, "NAV RST Counters command");
			NAV_ResetCounters();
			break;

		default:
			break;
	}

    return;

} /* End NAV_ProcessCommandPacket */


/************************************************************************
** Report Housekeeping Telemetry Data
*************************************************************************/
static void NAV_ReportHousekeeping(void)
{
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t*)&NAV_AppData.hk);
    CFE_SB_SendMsg((CFE_SB_Msg_t*)&NAV_AppData.hk);
} /* NAV_ReportingHousekeeping */


/************************************************************************
** Process command to reset counters
*************************************************************************/
static void NAV_ResetCounters(void)
{
	NAV_AppData.hk.cmd_count = 0;
	NAV_AppData.hk.cmd_error_count = 0;
} /* NAV_ResetCounters */

/************************************************************************
** Read data from the UART
*************************************************************************/
static void NAV_ReadAvailableData(uint8 DataBuffer[], int32 *DataLen)
{
    int32 i = 0;

    /* check how many bytes are waiting on the uart */
    *DataLen = uart_bytes_available(&NAV_UART);
    //OS_printf("NAV_ReadAvailableData(): gps bytes waiting: %d \n", DataLen);

    if (*DataLen > NAV_BUFFER_SIZE)
    {
        *DataLen = NAV_BUFFER_SIZE;
    }

    /* declare an out buffer to hold that data */
    if (*DataLen > 0)
    {
        /* grab the bytes */
        uart_read_port(&NAV_UART, DataBuffer, *DataLen);
    }
    else
    {
        /* OS_printf("GPS_ReadAvailableData(): gps uart data len is 0\n"); */
    }
}

/************************************************************************
** Parse NovAtel OEM615 BESTXYZA log data
*************************************************************************/
// Reference:  Section 1.1.1, p. 24, OEM6 Family Firmware Reference Manual, OM-20000129, Rev 8, January 2015 (file om-20000129.pdf)
// Reference:  Section 3.2.17, pp. 420-422, OEM6 Family Firmware Reference Manual, OM-20000129, Rev 8, January 2015 (file om-20000129.pdf)
static GPSSerialiation NAV_ParseOEM615Bestxyza(uint8 DataBuf[], int32 DataLen)
{
    GPSSerialiation GPSData;
    GPSData.weeks = 0;
    GPSData.seconds_into_week = 0;
    GPSData.fractions = 0.0;
    GPSData.ECEF_X = 0.0;
    GPSData.ECEF_Y = 0.0;
    GPSData.ECEF_Z = 0.0;
    GPSData.vel_x = 0.0;
    GPSData.vel_y = 0.0;
    GPSData.vel_z = 0.0;
    char *token;

    if (DataLen != 0) {
        token = strtok((char*)DataBuf, ",");
        if ((token != NULL) && (strncmp(token, "#BESTXYZA", 9) == 0)) {
            // Got a valid BESTXYZA log message - now the header
            token = strtok(NULL, ","); // Port
            token = strtok(NULL, ","); // Sequence #
            token = strtok(NULL, ","); // % Idle Time
            token = strtok(NULL, ","); // Time Status
            token = strtok(NULL, ","); // Week
            if (token != NULL) GPSData.weeks = atol(token);
            token = strtok(NULL, ","); // Seconds
            if (token != NULL) {
                GPSData.fractions = atof(token);
                GPSData.seconds_into_week = (uint32_t)GPSData.fractions;
                GPSData.fractions -= GPSData.seconds_into_week;
            }
            token = strtok(NULL, ","); // Receiver Status
            token = strtok(NULL, ","); // Reserved
            token = strtok(NULL, ";"); // Receiver s/w Version

            // Now the data
            token = strtok(NULL, ","); // P-sol status
            token = strtok(NULL, ","); // pos type
            token = strtok(NULL, ","); // P-X (m)
            if (token != NULL) GPSData.ECEF_X = atof(token);
            token = strtok(NULL, ","); // P-Y (m)
            if (token != NULL) GPSData.ECEF_Y = atof(token);
            token = strtok(NULL, ","); // P-Z (m)
            if (token != NULL) GPSData.ECEF_Z = atof(token);
            token = strtok(NULL, ","); // P-X sigma
            token = strtok(NULL, ","); // P-Y sigma
            token = strtok(NULL, ","); // P-Z sigma
            token = strtok(NULL, ","); // V-sol status
            token = strtok(NULL, ","); // vel type
            token = strtok(NULL, ","); // V-X (m/s
            if (token != NULL) GPSData.vel_x = atof(token);
            token = strtok(NULL, ","); // V-Y (m/s)
            if (token != NULL) GPSData.vel_y = atof(token);
            token = strtok(NULL, ","); // V-Z (m/s)
            if (token != NULL) GPSData.vel_z = atof(token);
        }
    }

    //GPSData.weeks uint32_t
    return GPSData;
} /* NAV_ParseOEM615Bestxyza */



