#ifndef _NOVATEL_OEM615_CHECKOUT_DEVICE_CFG_H_
#define _NOVATEL_OEM615_CHECKOUT_DEVICE_CFG_H_

/*
** NOVATEL_OEM615 Checkout Configuration
*/
#define NOVATEL_OEM615_CFG
/* Note: NOS3 uart requires matching handle and bus number */
#define NOVATEL_OEM615_CFG_STRING           "/dev/usart_16"
#define NOVATEL_OEM615_CFG_HANDLE           16 
#define NOVATEL_OEM615_CFG_BAUDRATE_HZ      115200
#define NOVATEL_OEM615_CFG_MS_TIMEOUT       250
#define NOVATEL_OEM615_CFG_DEBUG
#define NOVATEL_OEM615_DEVICE_CFG_CMD          0x03


#endif /* _NOVATEL_OEM615_CHECKOUT_DEVICE_CFG_H_ */
