# Library for NOVATEL_OEM615 Target
import sys
import glob

for p in glob.glob('/gems/gems/openc3-cosmos-nos3-*/targets/NOVATEL_OEM615/scripts'):
    if p not in sys.path:
        sys.path.append(p)

try:
    from openc3.script import cmd, tlm, check, wait_check, wait_check_packet, wait_check_tolerance
    import time
except ImportError:
    pass

#
# Definitions
#
NOVATEL_OEM615_CMD_SLEEP = 1.5
NOVATEL_OEM615_RESPONSE_TIMEOUT = 15
NOVATEL_OEM615_TEST_LOOP_COUNT = 1
NOVATEL_OEM615_DEVICE_LOOP_COUNT = 5
NOVATEL_OEM615_POSITION_DIFF = 25000
NOVATEL_OEM615_VELOCITY_DIFF = 100
NOVATEL_OEM615_HK_MAX_ATTEMPTS = 5
NOVATEL_OEM615_HK_RETRY_SLEEP = 3

#
# Functions
#
def get_gps_hk():
    for attempt in range(NOVATEL_OEM615_HK_MAX_ATTEMPTS):
        try:
            cmd("NOVATEL_OEM615_DEBUG NOVATEL_OEM615_REQ_HK")
            wait_check_packet("NOVATEL_OEM615_DEBUG", "NOVATEL_OEM615_HK_TLM", 1, NOVATEL_OEM615_RESPONSE_TIMEOUT)
            time.sleep(NOVATEL_OEM615_CMD_SLEEP)
            return  # success

        except Exception as e:
            if attempt == NOVATEL_OEM615_HK_MAX_ATTEMPTS - 1:
                raise RuntimeError(f"Failed to receive GPS HK telemetry after {NOVATEL_OEM615_HK_MAX_ATTEMPTS} attempts: {e}")
            else:
                print(f"REQ_HK attempt {attempt + 1} failed ({e}), retrying...")
                time.sleep(NOVATEL_OEM615_HK_RETRY_SLEEP)

def gps_cmd(command_string):
    count = tlm("NOVATEL_OEM615_DEBUG NOVATEL_OEM615_HK_TLM CMD_COUNT") + 1

    if (count == 256):
        count = 0

    cmd(command_string)
    get_gps_hk()
    current = tlm("NOVATEL_OEM615_DEBUG NOVATEL_OEM615_HK_TLM CMD_COUNT")
    if (current != count):
        # Try again
        cmd(command_string)
        get_gps_hk()
        current = tlm("NOVATEL_OEM615_DEBUG NOVATEL_OEM615_HK_TLM CMD_COUNT")
        if (current != count):
            # Third times the charm
            cmd(command_string)
            get_gps_hk()
            current = tlm("NOVATEL_OEM615_DEBUG NOVATEL_OEM615_HK_TLM CMD_COUNT")
            
    wait_check(f"NOVATEL_OEM615_DEBUG NOVATEL_OEM615_HK_TLM CMD_COUNT >= {count}", NOVATEL_OEM615_RESPONSE_TIMEOUT)

def enable_gps():
    # Send command
    gps_cmd("NOVATEL_OEM615_DEBUG NOVATEL_OEM615_ENABLE_CC")
    # Confirm
    wait_check("NOVATEL_OEM615_DEBUG NOVATEL_OEM615_HK_TLM DEVICE_ENABLED == 'ENABLED'", NOVATEL_OEM615_RESPONSE_TIMEOUT)

def disable_gps():
    # Send command
    gps_cmd("NOVATEL_OEM615_DEBUG NOVATEL_OEM615_DISABLE_CC")
    # Confirm
    wait_check("NOVATEL_OEM615_DEBUG NOVATEL_OEM615_HK_TLM DEVICE_ENABLED == 'DISABLED'", NOVATEL_OEM615_RESPONSE_TIMEOUT)

def safe_gps():
    time.sleep(2)
    get_gps_hk()
    state = tlm("NOVATEL_OEM615_DEBUG NOVATEL_OEM615_HK_TLM DEVICE_ENABLED")
    if (state != "DISABLED"):
        disable_gps()

def confirm_gps_data():
    dev_cmd_cnt = tlm("NOVATEL_OEM615_DEBUG NOVATEL_OEM615_HK_TLM DEVICE_COUNT")
    dev_cmd_err_cnt = tlm("NOVATEL_OEM615_DEBUG NOVATEL_OEM615_HK_TLM DEVICE_ERR_COUNT")
    
    wait_check_tolerance("NOVATEL_OEM615_DEBUG NOVATEL_OEM615_DATA_TLM ECEF_X", tlm("SIM_42_TRUTH SIM_42_TRUTH_DATA POSITION_W_0"), NOVATEL_OEM615_POSITION_DIFF, NOVATEL_OEM615_RESPONSE_TIMEOUT)
    wait_check_tolerance("NOVATEL_OEM615_DEBUG NOVATEL_OEM615_DATA_TLM ECEF_Y", tlm("SIM_42_TRUTH SIM_42_TRUTH_DATA POSITION_W_1"), NOVATEL_OEM615_POSITION_DIFF, NOVATEL_OEM615_RESPONSE_TIMEOUT)
    wait_check_tolerance("NOVATEL_OEM615_DEBUG NOVATEL_OEM615_DATA_TLM ECEF_Z", tlm("SIM_42_TRUTH SIM_42_TRUTH_DATA POSITION_W_2"), NOVATEL_OEM615_POSITION_DIFF, NOVATEL_OEM615_RESPONSE_TIMEOUT)

    wait_check_tolerance("NOVATEL_OEM615_DEBUG NOVATEL_OEM615_DATA_TLM VEL_X", tlm("SIM_42_TRUTH SIM_42_TRUTH_DATA VELOCITY_W_0"), NOVATEL_OEM615_VELOCITY_DIFF, NOVATEL_OEM615_RESPONSE_TIMEOUT)
    wait_check_tolerance("NOVATEL_OEM615_DEBUG NOVATEL_OEM615_DATA_TLM VEL_Y", tlm("SIM_42_TRUTH SIM_42_TRUTH_DATA VELOCITY_W_1"), NOVATEL_OEM615_VELOCITY_DIFF, NOVATEL_OEM615_RESPONSE_TIMEOUT)
    wait_check_tolerance("NOVATEL_OEM615_DEBUG NOVATEL_OEM615_DATA_TLM VEL_Z", tlm("SIM_42_TRUTH SIM_42_TRUTH_DATA VELOCITY_W_2"), NOVATEL_OEM615_VELOCITY_DIFF, NOVATEL_OEM615_RESPONSE_TIMEOUT)

    get_gps_hk()
    wait_check(f"NOVATEL_OEM615_DEBUG NOVATEL_OEM615_HK_TLM DEVICE_COUNT >= {dev_cmd_cnt}", NOVATEL_OEM615_RESPONSE_TIMEOUT)
    wait_check(f"NOVATEL_OEM615_DEBUG NOVATEL_OEM615_HK_TLM DEVICE_ERR_COUNT == {dev_cmd_err_cnt}", NOVATEL_OEM615_RESPONSE_TIMEOUT)

def confirm_gps_data_loop():
    for n in range(NOVATEL_OEM615_DEVICE_LOOP_COUNT):
        confirm_gps_data()

#
# Simulator Functions
#
def gps_prepare_ast():
    # Get to known state
    safe_gps()

    # Enable
    enable_gps()

    # Confirm data
    confirm_gps_data_loop()

def gps_sim_enable():
    cmd("SIM_CMDBUS_BRIDGE NOVATEL_OEM615_SIM_ENABLE")

def gps_sim_disable():
    cmd("SIM_CMDBUS_BRIDGE NOVATEL_OEM615_SIM_DISABLE")

def gps_sim_set_status(status):
    cmd(f"SIM_CMDBUS_BRIDGE NOVATEL_OEM615_SIM_SET_STATUS with STATUS {status}")