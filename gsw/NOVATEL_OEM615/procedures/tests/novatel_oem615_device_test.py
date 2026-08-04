import sys
import glob
import time

for p in glob.glob('/gems/gems/openc3-cosmos-nos3-*/targets/NOVATEL_OEM615/scripts'):
    if p not in sys.path:
        sys.path.append(p)

try:
    from openc3.script import cmd, tlm, wait_check
    from nos3.gps_lib import *
except ImportError:
    pass

def run_novatel_oem615_device_test():
    ##
    ## This script tests the cFS component device functionality.
    ## Currently this includes: 
    ##   Enable / disable, control hardware communications
    ##


    ##
    ## Enable / disable, control hardware communications
    ##
    for n in range(NOVATEL_OEM615_TEST_LOOP_COUNT):
        # Get to known state
        safe_gps()

        # Manually command to disable when already disabled
        cmd_cnt = tlm("NOVATEL_OEM615_DEBUG NOVATEL_OEM615_HK_TLM CMD_COUNT")
        cmd_err_cnt = tlm("NOVATEL_OEM615_DEBUG NOVATEL_OEM615_HK_TLM CMD_ERR_COUNT")
        cmd("NOVATEL_OEM615_DEBUG NOVATEL_OEM615_DISABLE_CC")
        get_gps_hk()
        wait_check(f"NOVATEL_OEM615_DEBUG NOVATEL_OEM615_HK_TLM CMD_COUNT == {cmd_cnt}", 5)
        wait_check(f"NOVATEL_OEM615_DEBUG NOVATEL_OEM615_HK_TLM CMD_ERR_COUNT == {cmd_err_cnt+1}", 5)

        # Enable
        enable_gps()

        time.sleep(1)

        get_gps_hk()

        # Confirm device counters increment without errors
        confirm_gps_data_loop()

        # Manually command to enable when already enabled
        cmd_cnt = tlm("NOVATEL_OEM615_DEBUG NOVATEL_OEM615_HK_TLM CMD_COUNT")
        cmd_err_cnt = tlm("NOVATEL_OEM615_DEBUG NOVATEL_OEM615_HK_TLM CMD_ERR_COUNT")
        cmd("NOVATEL_OEM615_DEBUG NOVATEL_OEM615_ENABLE_CC")
        get_gps_hk()
        wait_check(f"NOVATEL_OEM615_DEBUG NOVATEL_OEM615_HK_TLM CMD_COUNT == {cmd_cnt}", 5)
        wait_check(f"NOVATEL_OEM615_DEBUG NOVATEL_OEM615_HK_TLM CMD_ERR_COUNT == {cmd_err_cnt+1}", 5)

        # Reconfirm data remains as expected
        confirm_gps_data_loop()

        # Disable
        disable_gps()