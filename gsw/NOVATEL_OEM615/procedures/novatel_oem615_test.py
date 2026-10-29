import sys
import glob

for p in glob.glob('/gems/gems/openc3-cosmos-nos3-*/targets/NOVATEL_OEM615/scripts'):
    if p not in sys.path:
        sys.path.append(p)

from openc3.script.suite import Suite, Group

try:
    from nos3.gps_lib import *
    from nos3.novatel_oem615_app_test import run_novatel_oem615_app_test
    from nos3.novatel_oem615_device_test import run_novatel_oem615_device_test
    from nos3.novatel_oem615_ast_test import run_novatel_oem615_ast_test
except ImportError:
    pass

class NOVATEL_OEM615_Functional_Test(Group):
    def setup(self):
        safe_gps()

    def script_application(self):
        run_novatel_oem615_app_test()

    def script_device(self):
        run_novatel_oem615_device_test()

    def teardown(self):
        safe_gps()

class NOVATEL_OEM615_Automated_Scenario_Test(Group):
    def setup(self):
        safe_gps()

    def script_AST(self):
        run_novatel_oem615_ast_test()

    def teardown(self):
        safe_gps()

class Novatel_oem615_Test(Suite):
    def __init__(self):
        super().__init__()
        self.add_group(NOVATEL_OEM615_Functional_Test)
        self.add_group(NOVATEL_OEM615_Automated_Scenario_Test)

    def setup(self):
        safe_gps()
  
    def teardown(self):
        safe_gps()