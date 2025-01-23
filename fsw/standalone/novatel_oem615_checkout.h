/*******************************************************************************
** File: novatel_oem615_checkout.h
**
** Purpose:
**   This is the header file for the NOVATEL_OEM615 checkout.
**
*******************************************************************************/
#ifndef _NOVATEL_OEM615_CHECKOUT_H_
#define _NOVATEL_OEM615_CHECKOUT_H_

/*
** Includes
*/
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>

#include "hwlib.h"
#include "device_cfg.h"
#include "novatel_oem615_device.h"
#include "../../../../../fsw/osal/src/os/inc/common_types.h"


#if TGTNAME == cpu1
    #include "nos_link.h"
#endif

/*
** Standard Defines
*/
#define PROMPT                  "novatel_oem615> "
#define MAX_INPUT_BUF         	512
#define MAX_INPUT_TOKENS      	64
#define MAX_INPUT_TOKEN_SIZE  	50
#define TELEM_BUF_LEN           8

/*
** Command Defines
*/
#define CMD_UNKNOWN        -1
#define CMD_HELP            0
#define CMD_EXIT            1
#define CMD_NOOP            2
#define CMD_HK              3
#define CMD_NOVATEL_OEM615  4
#define CMD_CFG             5
#define CMD_LOG             6
#define CMD_UNLOG           7
#define CMD_UNLOG_ALL       8

//#define NOVATEL_OEM615_MUTEX_NAME            "NOVATEL_OEM615_MUTEX"
#define NOVATEL_OEM615_HK_MUTEX_NAME         "NAV_HK_MUTEX"

/*
** Enabled and Disabled Definitions
*/
#define NOVATEL_OEM615_DEVICE_DISABLED       0
#define NOVATEL_OEM615_DEVICE_ENABLED        1

/*
** Prototypes
*/
void print_help(void);
int  get_command(const char* str);
int  main(int argc, char *argv[]);

/*
** Generic Prototypes
*/
int check_number_arguments(int actual, int expected);
void to_lower(char* str);
void NOVATEL_OEM615_IncrementDeviceCount(void);
void NOVATEL_OEM615_IncrementDeviceErrorCount(void);
int32 NOVATEL_OEM615_GetDeviceEnabledStatus(void);
int32 NOVATEL_OEM615_SafeRequestHK(NOVATEL_OEM615_Device_HK_tlm_t* data);


#endif /* _NOVATEL_OEM615_CHECKOUT_H_ */
