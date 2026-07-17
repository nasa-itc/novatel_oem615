import sys
import glob
import time

for p in glob.glob('/gems/gems/openc3-cosmos-nos3-*/targets/NOVATEL_OEM615/scripts'):
    if p not in sys.path:
        sys.path.append(p)

try:
    from openc3.script import cmd, tlm, check, wait_check
    from nos3.gps_lib import *
except ImportError:
    pass

def run_novatel_oem615_ast_test():
    ##
    ## This script tests the cFS component in an automated scenario.
    ## Currently this includes: 
    ##   Hardware failure
    ##   Hardware status reporting fault
    ##


    ##
    ## Hardware failure
    ##
    for n in range(NOVATEL_OEM615_TEST_LOOP_COUNT):
        # Prepare
        gps_prepare_ast()

        # Disable sim and confirm device error counts increase
        dev_cmd_cnt = tlm("NOVATEL_OEM615_DEBUG NOVATEL_OEM615_HK_TLM DEVICE_COUNT")
        dev_cmd_err_cnt = tlm("NOVATEL_OEM615_DEBUG NOVATEL_OEM615_HK_TLM DEVICE_ERR_COUNT")
        
        gps_sim_disable()

        time.sleep(1)

        cmd("NOVATEL_OEM615_DEBUG NOVATEL_OEM615_REQ_HK")

        wait_check(f"NOVATEL_OEM615_DEBUG NOVATEL_OEM615_HK_TLM DEVICE_COUNT >= {dev_cmd_cnt}", 5)
        wait_check(f"NOVATEL_OEM615_DEBUG NOVATEL_OEM615_HK_TLM DEVICE_ERR_COUNT >= {dev_cmd_err_cnt}", 5)

        # Enable sim and confirm return to nominal operation
        gps_sim_enable()

        time.sleep(1)

        confirm_gps_data_loop()