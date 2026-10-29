import sys
import glob

for p in glob.glob('/gems/gems/openc3-cosmos-nos3-*/targets/NOVATEL_OEM615/scripts'):
    if p not in sys.path:
        sys.path.append(p)

try:
    from openc3.script import cmd, tlm, check
    from nos3.gps_lib import *
except ImportError:
    pass

def run_novatel_oem615_app_test():

    ##
    ## This script tests the standard cFS component application functionality.
    ## Currently this includes: 
    ##   Housekeeping, request telemetry to be published on the software bus
    ##   NOOP, no operation but confirm correct counters increment
    ##   Reset counters, increment as done in NOOP and confirm ability to clear repeatably
    ##   Invalid ground command, confirm bad lengths and codes are rejected
    ##

    # Get to known state
    safe_gps()

    ##
    ##   Housekeeping, request telemetry to be published on the software bus
    ##
    for n in range(NOVATEL_OEM615_TEST_LOOP_COUNT):
        get_gps_hk()


    ##
    ## NOOP, no operation but confirm correct counters increment
    ##
    for n in range(NOVATEL_OEM615_TEST_LOOP_COUNT):
        gps_cmd("NOVATEL_OEM615_DEBUG NOVATEL_OEM615_NOOP_CC")


    ##
    ## Reset counters, increment as done in NOOP and confirm ability to clear repeatably
    ##
    for n in range(NOVATEL_OEM615_TEST_LOOP_COUNT):
        gps_cmd("NOVATEL_OEM615_DEBUG NOVATEL_OEM615_NOOP_CC")
        cmd("NOVATEL_OEM615_DEBUG NOVATEL_OEM615_RST_COUNTERS_CC") # Note standard `cmd` as we can't reset counters and then confirm increment
        get_gps_hk()
        check("NOVATEL_OEM615_DEBUG NOVATEL_OEM615_HK_TLM CMD_COUNT == 0")
        check("NOVATEL_OEM615_DEBUG NOVATEL_OEM615_HK_TLM CMD_ERR_COUNT == 0")


    ##
    ##   Invalid ground command, confirm bad lengths and codes are rejected
    ##
    for n in range(NOVATEL_OEM615_TEST_LOOP_COUNT):
        # Bad length
        cmd_cnt = tlm("NOVATEL_OEM615_DEBUG NOVATEL_OEM615_HK_TLM CMD_COUNT")
        cmd_err_cnt = tlm("NOVATEL_OEM615_DEBUG NOVATEL_OEM615_HK_TLM CMD_ERR_COUNT")
        cmd(f"NOVATEL_OEM615_DEBUG NOVATEL_OEM615_NOOP_CC with CCSDS_LENGTH {n+2}") # Note +2 due to CCSDS already being +1
        get_gps_hk()
        check(f"NOVATEL_OEM615_DEBUG NOVATEL_OEM615_HK_TLM CMD_COUNT == {cmd_cnt}")
        check(f"NOVATEL_OEM615_DEBUG NOVATEL_OEM615_HK_TLM CMD_ERR_COUNT == {cmd_err_cnt+1}")

    for n in range(8, 8 + NOVATEL_OEM615_TEST_LOOP_COUNT):
        # Bad command codes
        cmd_cnt = tlm("NOVATEL_OEM615_DEBUG NOVATEL_OEM615_HK_TLM CMD_COUNT")
        cmd_err_cnt = tlm("NOVATEL_OEM615_DEBUG NOVATEL_OEM615_HK_TLM CMD_ERR_COUNT")
        cmd(f"NOVATEL_OEM615_DEBUG NOVATEL_OEM615_NOOP_CC with CCSDS_FC {n+1}")
        get_gps_hk()
        check(f"NOVATEL_OEM615_DEBUG NOVATEL_OEM615_HK_TLM CMD_COUNT == {cmd_cnt}")
        check(f"NOVATEL_OEM615_DEBUG NOVATEL_OEM615_HK_TLM CMD_ERR_COUNT == {cmd_err_cnt+1}")