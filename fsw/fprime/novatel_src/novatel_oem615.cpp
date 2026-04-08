// ======================================================================
// \title  novatel_oem615.cpp
// \author jstar
// \brief  cpp file for novatel_oem615 component implementation class
// ======================================================================

#include "novatel_src/novatel_oem615.hpp"
#include <Fw/Logger/Logger.hpp>
#include <Fw/Log/LogString.hpp>
// #include "FpConfig.hpp"
#include "Fw/FPrimeBasicTypes.hpp"



namespace Components {

  // ----------------------------------------------------------------------
  // Component construction and destruction
  // ----------------------------------------------------------------------

  novatel_oem615 ::
    novatel_oem615(const char* const compName) :
      novatel_oem615ComponentBase(compName)
  {
    /* Initialize HWLIB */
    //#ifdef _NOS_ENGINE_LINK_
    nos_init_link();
    //#endif

    /* Open device specific protocols */
    Novatel_oem615Uart.deviceString = NOVATEL_OEM615_CFG_STRING;
    Novatel_oem615Uart.handle = NOVATEL_OEM615_CFG_HANDLE;
    Novatel_oem615Uart.isOpen = PORT_CLOSED;
    Novatel_oem615Uart.baud = NOVATEL_OEM615_CFG_BAUDRATE_HZ;
    Novatel_oem615Uart.access_option = uart_access_flag_RDWR;

    // Novatel_oem615HK.DeviceCounter = 0;
    // Novatel_oem615HK.DeviceConfig = 0;
    // Novatel_oem615HK.DeviceStatus = 0;

    int status = uart_init_port(&Novatel_oem615Uart);
    if (status == OS_SUCCESS)
    {
      OS_printf("UART device %s configured with baudrate %d \n", Novatel_oem615Uart.deviceString, Novatel_oem615Uart.baud);
    }
    else
    {
      OS_printf("UART device %s failed to initialize! \n", Novatel_oem615Uart.deviceString);
    }
  }

  novatel_oem615 ::
    ~novatel_oem615()
  {
    // Close the device 
    uart_close_port(&Novatel_oem615Uart);

    nos_destroy_link();
  }

  // ----------------------------------------------------------------------
  // Handler implementations for commands
  // ----------------------------------------------------------------------
  
  void novatel_oem615 :: updateData_handler(const FwIndexType portNum, U32 context)
  {
    int32_t status = OS_SUCCESS;

    status = NOVATEL_OEM615_ChildProcessReadData(&Novatel_oem615Uart, &Novatel_oem615Data);
    if (status == OS_SUCCESS)
    {
      this->GPSout_out(0, 
                      Novatel_oem615Data.Weeks, 
                      Novatel_oem615Data.SecondsIntoWeek, 
                      Novatel_oem615Data.Fractions,
                      Novatel_oem615Data.ECEFX, 
                      Novatel_oem615Data.ECEFY, 
                      Novatel_oem615Data.ECEFZ, 
                      Novatel_oem615Data.VelX, 
                      Novatel_oem615Data.VelY, 
                      Novatel_oem615Data.VelZ, 
                      Novatel_oem615Data.lat, 
                      Novatel_oem615Data.lon, 
                      Novatel_oem615Data.alt);
    }
  }

  void novatel_oem615 :: updateTlm_handler(const FwIndexType portNum, U32 context)
  {
    this->tlmWrite_Weeks(Novatel_oem615Data.Weeks);
    this->tlmWrite_SecondsIntoWeek(Novatel_oem615Data.SecondsIntoWeek);
    this->tlmWrite_Fractions(Novatel_oem615Data.Fractions);
    this->tlmWrite_ECEFX(Novatel_oem615Data.ECEFX);
    this->tlmWrite_ECEFY(Novatel_oem615Data.ECEFY);
    this->tlmWrite_ECEFZ(Novatel_oem615Data.ECEFZ);
    this->tlmWrite_VelX(Novatel_oem615Data.VelX);
    this->tlmWrite_VelY(Novatel_oem615Data.VelY);
    this->tlmWrite_VelZ(Novatel_oem615Data.VelZ);
    this->tlmWrite_lat(Novatel_oem615Data.lat);
    this->tlmWrite_lon(Novatel_oem615Data.lon);
    this->tlmWrite_alt(Novatel_oem615Data.alt);
  }

  void novatel_oem615 :: REQUEST_DATA_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    
    int32_t status = OS_SUCCESS;
    status = NOVATEL_OEM615_ChildProcessReadData(&Novatel_oem615Uart, &Novatel_oem615Data);
    if (status == OS_SUCCESS)
      {
        Fw::LogStringArg log_msg("RequestData command success\n");
        this->log_ACTIVITY_HI_TELEM(log_msg);
        this->tlmWrite_Weeks(Novatel_oem615Data.Weeks);
        this->tlmWrite_SecondsIntoWeek(Novatel_oem615Data.SecondsIntoWeek);
        this->tlmWrite_Fractions(Novatel_oem615Data.Fractions);
        this->tlmWrite_ECEFX(Novatel_oem615Data.ECEFX);
        this->tlmWrite_ECEFY(Novatel_oem615Data.ECEFY);
        this->tlmWrite_ECEFZ(Novatel_oem615Data.ECEFZ);
        this->tlmWrite_VelX(Novatel_oem615Data.VelX);
        this->tlmWrite_VelY(Novatel_oem615Data.VelY);
        this->tlmWrite_VelZ(Novatel_oem615Data.VelZ);
        this->tlmWrite_lat(Novatel_oem615Data.lat);
        this->tlmWrite_lon(Novatel_oem615Data.lon);
        this->tlmWrite_alt(Novatel_oem615Data.alt);
      }
    else
      {
        Fw::LogStringArg log_msg("RequestData command failed!\n");
        this->log_ACTIVITY_HI_TELEM(log_msg);
      }
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  void novatel_oem615 :: SERIAL_CONFIG_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, const I8 log_type) {
    
    int status = OS_SUCCESS;
    status = NOVATEL_OEM615_CommandDevice(&Novatel_oem615Uart, 7, log_type, 0);
    if (status == OS_SUCCESS)
      {
        Fw::LogStringArg log_msg("Configuration command success");
        this->log_ACTIVITY_HI_TELEM(log_msg);
      }
      else
      {
        Fw::LogStringArg log_msg("Configuration command failed!\n");
        this->log_ACTIVITY_HI_TELEM(log_msg);
      }
 
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }


  void novatel_oem615 :: LOG_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, const I8 log_type2, const I8 period) { 

    int status = OS_SUCCESS;
    status = NOVATEL_OEM615_CommandDevice(&Novatel_oem615Uart, 4, log_type2, period);

    if (status == OS_SUCCESS)
      {
        Fw::LogStringArg log_msg("Configuration command success");
        this->log_ACTIVITY_HI_TELEM(log_msg);
      }
      else
      {
        Fw::LogStringArg log_msg("Configuration command failed!\n");
        this->log_ACTIVITY_HI_TELEM(log_msg);
      }
 
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  void novatel_oem615 :: UNLOG_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, const I8 log_type3) {
    
    int status = OS_SUCCESS;
    status = NOVATEL_OEM615_CommandDevice(&Novatel_oem615Uart, 5, log_type3, 0);

    if (status == OS_SUCCESS)
      {
        Fw::LogStringArg log_msg("Configuration command success");
        this->log_ACTIVITY_HI_TELEM(log_msg); 
      }
      else
      {
        Fw::LogStringArg log_msg("Configuration command failed!\n");
        this->log_ACTIVITY_HI_TELEM(log_msg);
      }
 
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

    void novatel_oem615 ::
    UNLOG_ALL_cmdHandler(
        FwOpcodeType opCode,
        U32 cmdSeq
    )
  {
    int32_t status = OS_SUCCESS;

    status =  NOVATEL_OEM615_CommandDevice(&Novatel_oem615Uart, 6, 0, 0);

    if (status == OS_SUCCESS)        
      {
        Fw::LogStringArg log_msg("UNLOG_ALL command success!\n");
        this->log_ACTIVITY_HI_TELEM(log_msg);
      }
    else
      {
        Fw::LogStringArg log_msg("UNLOG_ALL command failed!\n");
        this->log_ACTIVITY_HI_TELEM(log_msg);
      }


    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
 }
}
