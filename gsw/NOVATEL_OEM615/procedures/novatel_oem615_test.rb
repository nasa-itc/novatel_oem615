require 'cosmos'
require 'cosmos/script'
require 'mission_lib.rb'

class GPS_LPT < Cosmos::Test
  def setup

  end

  def test_lpt
    start("tests/novatel_oem615_lpt_test.rb")
  end

  def teardown

  end
end

class GPS_CPT < Cosmos::Test
  def setup
      
  end

  def test_cpt
    start("tests/novatel_oem615_cpt_test.rb")
  end

  def teardown

  end
end

class Novatel_oem615_Test < Cosmos::TestSuite
  def initialize
      super()
      add_test('GPS_CPT')
      add_test('GPS_LPT')
  end

  def setup
  end
  
  def teardown
  end
end