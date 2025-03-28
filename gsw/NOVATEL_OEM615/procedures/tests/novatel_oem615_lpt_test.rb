require 'cosmos'
require 'cosmos/script'
require "cfs_lib.rb"
#require 'math'

##
## NOOP
##
initial_command_count = tlm("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM CMD_COUNT")
initial_error_count = tlm("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM CMD_ERR_COUNT")
initial_device_error_count = tlm("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM DEVICE_ERR_COUNT")
cmd("NOVATEL_OEM615 NOVATEL_OEM615_NOOP_CC")
wait_check("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM CMD_COUNT > #{initial_command_count}", 30)
wait_check("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM CMD_ERR_COUNT == #{initial_error_count}", 30)
wait_check("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM DEVICE_ERR_COUNT == #{initial_device_error_count}", 30)

sleep(5)

##
## Successful Disable
##
initial_command_count = tlm("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM CMD_COUNT")
initial_error_count = tlm("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM CMD_ERR_COUNT")
initial_device_error_count = tlm("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM DEVICE_ERR_COUNT")
cmd("NOVATEL_OEM615 NOVATEL_OEM615_DISABLE_CC")
wait_check("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM CMD_COUNT > #{initial_command_count}", 30)
wait_check("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM CMD_ERR_COUNT == #{initial_error_count}", 30)
wait_check("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM DEVICE_ERR_COUNT == #{initial_device_error_count}", 30)
wait_check("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM DEVICE_ENABLED == 'DISABLED'", 30)

sleep(5)

##
## Failed Disable (doubled)
##
initial_command_count = tlm("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM CMD_COUNT")
initial_error_count = tlm("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM CMD_ERR_COUNT")
initial_device_error_count = tlm("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM DEVICE_ERR_COUNT")
cmd("NOVATEL_OEM615 NOVATEL_OEM615_DISABLE_CC")
wait_check("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM CMD_COUNT > #{initial_command_count}", 30)
wait_check("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM CMD_ERR_COUNT == #{initial_error_count}", 30)
wait_check("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM DEVICE_ERR_COUNT > #{initial_device_error_count}", 30)
wait_check("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM DEVICE_ENABLED == 'DISABLED'", 30)

sleep(5)

##
## HK without Device
##
initial_error_count = tlm("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM CMD_ERR_COUNT")
initial_device_error_count = tlm("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM DEVICE_ERR_COUNT")
cmd("NOVATEL_OEM615 NOVATEL_OEM615_REQ_HK")
wait_check("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM CMD_ERR_COUNT == #{initial_error_count}", 30)
wait_check("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM DEVICE_ERR_COUNT == #{initial_device_error_count}", 30)

sleep(5)

##
## Data without Device
##
initial_error_count = tlm("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM CMD_ERR_COUNT")
initial_device_error_count = tlm("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM DEVICE_ERR_COUNT")
cmd("NOVATEL_OEM615 NOVATEL_OEM615_REQ_DATA")
wait_check("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM CMD_ERR_COUNT == #{initial_error_count}", 30)
wait_check("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM DEVICE_ERR_COUNT == #{initial_device_error_count}", 30)

sleep(5)

##
## Successful Enable
##
initial_command_count = tlm("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM CMD_COUNT")
initial_error_count = tlm("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM CMD_ERR_COUNT")
initial_device_error_count = tlm("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM DEVICE_ERR_COUNT")
cmd("NOVATEL_OEM615 NOVATEL_OEM615_ENABLE_CC")
wait_check("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM CMD_COUNT > #{initial_command_count}", 30)
wait_check("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM CMD_ERR_COUNT == #{initial_error_count}", 30)
wait_check("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM DEVICE_ERR_COUNT == #{initial_device_error_count}", 30)
wait_check("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM DEVICE_ENABLED == 'ENABLED'", 30)

sleep(5)

##
## Failed Enable (doubled)
##
initial_command_count = tlm("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM CMD_COUNT")
initial_error_count = tlm("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM CMD_ERR_COUNT")
initial_device_error_count = tlm("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM DEVICE_ERR_COUNT")
cmd("NOVATEL_OEM615 NOVATEL_OEM615_ENABLE_CC")
wait_check("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM CMD_COUNT > #{initial_command_count}", 30)
wait_check("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM CMD_ERR_COUNT == #{initial_error_count}", 30)
wait_check("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM DEVICE_ERR_COUNT > #{initial_device_error_count}", 30)
wait_check("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM DEVICE_ENABLED == 'ENABLED'", 30)

sleep(5)

##
## Housekeeping w/ Device
##
initial_error_count = tlm("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM CMD_ERR_COUNT")
initial_device_error_count = tlm("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM DEVICE_ERR_COUNT")
cmd("NOVATEL_OEM615 NOVATEL_OEM615_REQ_HK")
wait_check("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM CMD_ERR_COUNT == #{initial_error_count}", 30)
wait_check("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM DEVICE_ERR_COUNT == #{initial_device_error_count}", 30)

sleep(5)

##
## Data w/ Device
##
initial_error_count = tlm("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM CMD_ERR_COUNT")
initial_device_error_count = tlm("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM DEVICE_ERR_COUNT")

cmd("NOVATEL_OEM615 NOVATEL_OEM615_REQ_DATA")

pos0 = tlm("SIM_42_TRUTH SIM_42_TRUTH_DATA POSITION_N_0")
pos1 = tlm("SIM_42_TRUTH SIM_42_TRUTH_DATA POSITION_N_1")
pos2 = tlm("SIM_42_TRUTH SIM_42_TRUTH_DATA POSITION_N_2")

vel0 = tlm("SIM_42_TRUTH SIM_42_TRUTH_DATA VELOCITY_N_0")
vel1 = tlm("SIM_42_TRUTH SIM_42_TRUTH_DATA VELOCITY_N_1")
vel2 = tlm("SIM_42_TRUTH SIM_42_TRUTH_DATA VELOCITY_N_2")

gps_posx = tlm("NOVATEL_OEM615 NOVATEL_OEM615_DATA_TLM ECEF_X")
gps_posy = tlm("NOVATEL_OEM615 NOVATEL_OEM615_DATA_TLM ECEF_y")
gps_posz = tlm("NOVATEL_OEM615 NOVATEL_OEM615_DATA_TLM ECEF_Z")

gps_velx = tlm("NOVATEL_OEM615 NOVATEL_OEM615_DATA_TLM VEL_X")
gps_vely = tlm("NOVATEL_OEM615 NOVATEL_OEM615_DATA_TLM VEL_Y")
gps_velz = tlm("NOVATEL_OEM615 NOVATEL_OEM615_DATA_TLM VEL_Z")

truth_42_pos0_diff = (pos0 - gps_posx).abs()
truth_42_pos1_diff = (pos1 - gps_posy).abs()
truth_42_pos2_diff = (pos2 - gps_posz).abs()

truth_42_vel0_diff = (vel0 - gps_velx).abs()
truth_42_vel1_diff = (vel1 - gps_vely).abs()
truth_42_vel2_diff = (vel2 - gps_velz).abs()

diff_margin = 2.5

wait_check("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM CMD_ERR_COUNT == #{initial_error_count}", 30)
wait_check("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM DEVICE_ERR_COUNT == #{initial_device_error_count}", 30)

wait_check_expression("truth_42_pos0_diff <= diff_margin # #{truth_42_pos0_diff} <= #{diff_margin}", 15)
wait_check_expression("truth_42_pos0_diff <= diff_margin # #{truth_42_pos1_diff} <= #{diff_margin}", 15)
wait_check_expression("truth_42_pos0_diff <= diff_margin # #{truth_42_pos2_diff} <= #{diff_margin}", 15)

wait_check_expression("truth_42_vel0_diff <= diff_margin # #{truth_42_vel0_diff} <= #{diff_margin}", 15)
wait_check_expression("truth_42_vel0_diff <= diff_margin # #{truth_42_vel1_diff} <= #{diff_margin}", 15)
wait_check_expression("truth_42_vel0_diff <= diff_margin # #{truth_42_vel2_diff} <= #{diff_margin}", 15)


sleep(5)

##
## Reset Counters
##
cmd("NOVATEL_OEM615 NOVATEL_OEM615_DISABLE_CC") # disable to be able to properly check that device count gets reset
cmd("NOVATEL_OEM615 NOVATEL_OEM615_RST_COUNTERS_CC")
wait_check("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM CMD_COUNT == 0", 30)
wait_check("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM CMD_ERR_COUNT == 0", 30)
wait_check("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM DEVICE_COUNT == 0", 30)
wait_check("NOVATEL_OEM615 NOVATEL_OEM615_HK_TLM DEVICE_ERR_COUNT == 0", 30)

##
## Reenable so test can be run multiple times
##
cmd("NOVATEL_OEM615 NOVATEL_OEM615_ENABLE_CC")