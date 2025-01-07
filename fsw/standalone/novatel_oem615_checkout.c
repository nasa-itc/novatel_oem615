/*******************************************************************************
** File: novatel_oem615_checkout.c
**
** Purpose:
**   This checkout can be run without cFS and is used to quickly develop and 
**   test functions required for a specific component.
**
*******************************************************************************/

/*
** Include Files
*/
#include "novatel_oem615_checkout.h"

/*
** Global Variables
*/
uart_info_t Novatel_oem615Uart;             /* Hardware protocol definition */
NOVATEL_OEM615_Device_HK_tlm_t Novatel_oem615HK;    /* NOVATEL_OEM615 Housekeeping Telemetry Packet */
NOVATEL_OEM615_Device_Data_tlm_t Novatel_oem615Data; 

/*
** Operational data  - not reported in housekeeping
*/
CFE_MSG_Message_t * MsgPtr;                 /* Pointer to msg received on software bus */
CFE_SB_PipeId_t CmdPipe;                    /* Pipe Id for HK command pipe */
uint32 RunStatus;                           /* App run status for controlling the application state */
uint32 HkDataMutex;                         /* Locks all data */

/*
** Device data 
*/
uint32 DeviceID;		                    /* Device ID provided by CFS on initialization */
NOVATEL_OEM615_Device_tlm_t Novatel_oem615Device;      /* Device specific data packet */

/*
** Component Functions
*/
void print_help(void) 
{
    printf(PROMPT "command [args]\n"
        "---------------------------------------------------------------------\n"
        "help                               - Display help                    \n"
        "exit                               - Exit app                        \n"
        "noop                               - No operation command to device  \n"
        "n                                  - ^                               \n"
        "hk                                 - Request device housekeeping     \n"
        "h                                  - ^                               \n"
        "novatel_oem615                     - Request novatel_oem615 data     \n"
        "s                                  - ^                               \n"
        "cfg #                              - Send configuration #            \n"
        "c #                                - ^                               \n"
        "\n"
    );
}


int get_command(const char* str)
{
    int status = CMD_UNKNOWN;
    char lcmd[MAX_INPUT_TOKEN_SIZE];
    strncpy(lcmd, str, MAX_INPUT_TOKEN_SIZE);

    /* Convert command to lower case */
    to_lower(lcmd);

    if(strcmp(lcmd, "help") == 0) 
    {
        status = CMD_HELP;
    }
    else if(strcmp(lcmd, "exit") == 0) 
    {
        status = CMD_EXIT;
    }
    else if(strcmp(lcmd, "noop") == 0) 
    {
        status = CMD_NOOP;
    }
    else if(strcmp(lcmd, "n") == 0) 
    {
        status = CMD_NOOP;
    }
    else if(strcmp(lcmd, "hk") == 0) 
    {
        status = CMD_HK;
    }
    else if(strcmp(lcmd, "h") == 0) 
    {
        status = CMD_HK;
    }
    else if(strcmp(lcmd, "novatel_oem615") == 0) 
    {
        status = CMD_NOVATEL_OEM615;
    }
    else if(strcmp(lcmd, "s") == 0) 
    {
        status = CMD_NOVATEL_OEM615;
    }
    else if(strcmp(lcmd, "cfg") == 0) 
    {
        status = CMD_CFG;
    }
    else if(strcmp(lcmd, "c") == 0) 
    {
        status = CMD_CFG;
    }
    return status;
}


int process_command(int cc, int num_tokens, char tokens[MAX_INPUT_TOKENS][MAX_INPUT_TOKEN_SIZE])
{
    int32_t status = OS_SUCCESS;
    int32_t exit_status = OS_SUCCESS;
    uint32_t config;

    /* Process command */
    switch(cc) 
    {	
        case CMD_HELP:
            print_help();
            break;
        
        case CMD_EXIT:
            exit_status = OS_ERROR;
            break;

        case CMD_NOOP:
            if (check_number_arguments(num_tokens, 0) == OS_SUCCESS)
            {
                status = NOVATEL_OEM615_CommandDevice(&Novatel_oem615Uart, NOVATEL_OEM615_DEVICE_CFG_CMD, 0);
                if (status == OS_SUCCESS)
                {
                    OS_printf("NOOP command success\n");
                }
                else
                {
                    OS_printf("NOOP command failed!\n");
                }
            }
            break;

        case CMD_HK:
            if (check_number_arguments(num_tokens, 0) == OS_SUCCESS)
            {
                status = NOVATEL_OEM615_RequestHK(&Novatel_oem615Uart, &Novatel_oem615HK);
                if (status == OS_SUCCESS)
                {
                    OS_printf("NovatelHk.DeviceCounter = %d \n", Novatel_oem615HK.DeviceCounter);
                    OS_printf("NovatelHK.DviceConfig = %d \n", Novatel_oem615HK.DeviceConfig);
                    OS_printf("NovatelHK.DeviceStatus = %d \n", Novatel_oem615HK.DeviceStatus);
                }
            }
            break;

        case CMD_NOVATEL_OEM615:
            if (check_number_arguments(num_tokens, 0) == OS_SUCCESS)
            {
                status = NOVATEL_OEM615_RequestData(&Novatel_oem615Uart, &Novatel_oem615Data);
                if (status == OS_SUCCESS)
                {
                    OS_printf("NOVATEL_OEM615_RequestData command success\n");
                    // TODO - Fix print statements, error says Novatel_oem615Data has no DeviceCounter attribute but its declaration is similar to HK so not sure why
                    // OS_printf("NovatelData.DeviceCounter = %d \n", Novatel_oem615Data.DeviceDataZ);
                    // OS_printf("NovatelData.DeviceDataX = %d \n", Novatel_oem615Data.DeviceDataX);
                    // OS_printf("NovatelData.DeviceDataY = %d \n", Novatel_oem615Data.DeviceDataY);
                    // OS_printf("NovatelData.DeviceDataZ = %d \n", Novatel_oem615Data.DeviceDataZ);
                }
                else
                {
                    OS_printf("NOVATEL_OEM615_RequestData command failed!\n");
                }
            }
            break;

        case CMD_CFG:
            if (check_number_arguments(num_tokens, 1) == OS_SUCCESS)
            {
                config = atoi(tokens[0]);
                status = NOVATEL_OEM615_CommandDevice(&Novatel_oem615Uart, NOVATEL_OEM615_DEVICE_CFG_CMD, config);
                if (status == OS_SUCCESS)
                {
                    OS_printf("Configuration command success with value %u\n", config);
                }
                else
                {
                    OS_printf("Configuration command failed!\n");
                }
            }
            break;
        
        default: 
            OS_printf("Invalid command format, type 'help' for more info\n");
            break;
    }
    return exit_status;
}


int main(int argc, char *argv[]) 
{
    int status = OS_SUCCESS;
    char input_buf[MAX_INPUT_BUF];
    char input_tokens[MAX_INPUT_TOKENS][MAX_INPUT_TOKEN_SIZE];
    int num_input_tokens;
    int cmd;    
    char* token_ptr;
    uint8_t run_status = CFE_ES_RunStatus_APP_RUN;

    /* Initialize HWLIB */
    #ifdef _NOS_ENGINE_LINK_
        nos_init_link();
    #endif

    /* Register the events */
    CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY);

    /* Create the Software Bus command pipe */
    status = CFE_SB_CreatePipe(&CmdPipe, NOVATEL_OEM615_PIPE_DEPTH, "NAV_CMD_PIPE")
    {
        CFE_EVS_SendEvent(NOVATEL_OEM615_PIPE_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Creating SB Pipe,RC=0x%08X",(unsigned int) status);
       return status;
    }

    /*
    ** Subscribe to ground commands
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(NOVATEL_OEM615_CMD_MID), CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(NOVATEL_OEM615_SUB_CMD_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Subscribing to HK Gnd Cmds, MID=0x%04X, RC=0x%08X",
            NOVATEL_OEM615_CMD_MID, (unsigned int) status);
        return status;
    }

    /*
    ** Subscribe to housekeeping (hk) message requests
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(NOVATEL_OEM615_REQ_HK_MID), CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(NOVATEL_OEM615_SUB_REQ_HK_ERR_EID, CFE_EVS_EventType_ERROR,
            "Error Subscribing to HK Request, MID=0x%04X, RC=0x%08X",
            NOVATEL_OEM615_REQ_HK_MID, (unsigned int) status);
        return status;
    }

    /* 
    ** Initialize the published HK message - this HK message will contain the 
    ** telemetry that has been defined in the NOVATEL_OEM615_HkTelemetryPkt for this app.
    */
    CFE_MSG_Init(CFE_MSG_PTR(Novatel_oem615HK.TlmHeader),
                   CFE_SB_ValueToMsgId(NOVATEL_OEM615_HK_TLM_MID),
                   NOVATEL_OEM615_HK_TLM_LNGTH);

    /*
    ** Initialize the device packet message
    ** This packet is specific to your application
    */
    CFE_MSG_Init(CFE_MSG_PTR(Novatel_oem615Device.TlmHeader),
                   CFE_SB_ValueToMsgId(NOVATEL_OEM615_DEVICE_TLM_MID),
                   NOVATEL_OEM615_DEVICE_TLM_LNGTH);


    /* Open device specific protocols */
    Novatel_oem615Uart.deviceString = NOVATEL_OEM615_CFG_STRING;
    Novatel_oem615Uart.handle = NOVATEL_OEM615_CFG_HANDLE;
    Novatel_oem615Uart.isOpen = PORT_CLOSED;
    Novatel_oem615Uart.baud = NOVATEL_OEM615_CFG_BAUDRATE_HZ;
    Novatel_oem615Uart.access_option = uart_access_flag_RDWR;

    /*
    ** Initialize application data
    ** Note that counters are excluded as they were reset in the previous code block
    */
    Novatel_oem615HK.DeviceEnabled = NOVATEL_OEM615_DEVICE_DISABLED;
    Novatel_oem615HK.DeviceHK.DeviceCounter = 0;
    Novatel_oem615HK.DeviceHK.DeviceConfig = 0;
    Novatel_oem615HK.DeviceHK.DeviceStatus = 0;


    /* Create hk data mutex */
    status = OS_MutSemCreate(&HkDataMutex, NOVATEL_OEM615_HK_MUTEX_NAME, 0);
    if (status != OS_SUCCESS)
    {
        CFE_EVS_SendEvent(NOVATEL_OEM615_CREATE_MUTEX_ERR_EID, CFE_EVS_EventType_ERROR, "NOVATEL_OEM615: Create hk mutex error %d", status);
        return status;
    }


    status = uart_init_port(&Novatel_oem615Uart);
    if (status == OS_SUCCESS)
    {
        printf("UART device %s configured with baudrate %d \n", Novatel_oem615Uart.deviceString, Novatel_oem615Uart.baud);
    }
    else
    {
        printf("UART device %s failed to initialize! \n", Novatel_oem615Uart.deviceString);
        run_status = OS_ERROR;
    }

    /* 
    ** Create device task
    */
    status = CFE_ES_CreateChildTask(&DeviceID,
                                    NOVATEL_OEM615_CHILD_TASK_NAME,
                                    NOVATEL_OEM615_ChildTask, 0,
                                    NOVATEL_OEM615_CHILD_TASK_STACK_SIZE,
                                    NOVATEL_OEM615_CHILD_TASK_PRIORITY, 0);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(NOVATEL_OEM615_CHILDTASKINIT_ERR_EID, CFE_EVS_EventType_ERROR, "NOVATEL_OEM615: Create device task error %d", status);
        return status;
    }

    /* 
    ** Always reset all counters during application initialization 
    */
    NOVATEL_OEM615_ResetCounters();

    /* 
     ** Send an information event that the app has initialized. 
     ** This is useful for debugging the loading of individual applications.
     */
    status = CFE_EVS_SendEvent(NOVATEL_OEM615_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION,
               "NOVATEL_OEM615 App Initialized. Version %d.%d.%d.%d",
                NOVATEL_OEM615_MAJOR_VERSION,
                NOVATEL_OEM615_MINOR_VERSION, 
                NOVATEL_OEM615_REVISION, 
                NOVATEL_OEM615_MISSION_REV);	
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("NOVATEL_OEM615: Error sending initialization event: 0x%08X\n", (unsigned int) status);
    }

    /* Main loop */
    print_help();
    while(run_status == OS_SUCCESS) 
    {
        num_input_tokens = -1;
        cmd = CMD_UNKNOWN;

        /* Read user input */
        printf(PROMPT);
        fgets(input_buf, MAX_INPUT_BUF, stdin);

        /* Tokenize line buffer */
        token_ptr = strtok(input_buf, " \t\n");
        while((num_input_tokens < MAX_INPUT_TOKENS) && (token_ptr != NULL)) 
        {
            if(num_input_tokens == -1) 
            {
                /* First token is command */
                cmd = get_command(token_ptr);
            }
            else 
            {
                strncpy(input_tokens[num_input_tokens], token_ptr, MAX_INPUT_TOKEN_SIZE);
            }
            token_ptr = strtok(NULL, " \t\n");
            num_input_tokens++;
        }

        /* Process command if valid */
        if(num_input_tokens >= 0)
        {
            /* Process command */
            run_status = process_command(cmd, num_input_tokens, input_tokens);
        }
    }

    // Close the device 
    uart_close_port(&Novatel_oem615Uart);

    #ifdef _NOS_ENGINE_LINK_
        nos_destroy_link();
    #endif

    OS_printf("Cleanly exiting novatel_oem615 application...\n\n"); 
    return 1;
}


/*
** Generic Functions
*/
int check_number_arguments(int actual, int expected)
{
    int status = OS_SUCCESS;
    if (actual != expected)
    {
        status = OS_ERROR;
        OS_printf("Invalid command format, type 'help' for more info\n");
    }
    return status;
}

void to_lower(char* str)
{
    char* ptr = str;
    while(*ptr)
    {
        *ptr = tolower((unsigned char) *ptr);
        ptr++;
    }
    return;
}

