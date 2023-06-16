# NovAtel OEM 615 - cFS Application

This repository contains the NovAtel OEM 615 cFS application.
This includes flight software (FSW), ground software (GSW), simulation, and support directories.

## Documentation
Currently support exists for the following versions:
* [OEM615 Dual-Frequency GNSS Receiver](https://www.novatel.com/products/gnss-receivers/oem-receiver-boards/oem6-receivers/oem615/)

## Overview
This novatel_oem615 component is a USART device that accepts multiple commands including requests for telemetry. 
The available FSW is for use in the core Flight System (cFS) while the GSw supports COSMOS.
A NOS3 simulation is available which includes a 42 data provider.

# Device Communications
The protocol, commands, and responses of the component are captured below.

## Protocol
The protocol in use is Universal Synchronous/Asynchronous Receiver/Transmitter (USART). The novatel_oem615 operates on the usart_1 bus on the spacecraft under handle/node-port 1.

## Commands
There commands for the novatel_oem615 include LOG, LOGALL, UNLOG, and SERIALCONFIG, as well as the generic commands expected of all NOS3 components (NOOP, RESET_COUNTERS, ENABLE, DISABLE, CONFIG).

## Response
Response formats are as follows:
* TODO

# Configuration
The various configuration parameters available for each portion of the component are captured below.

## FSW
Refer to the file [fsw/platform_inc/novatel_oem615_platform_cfg.h](fsw/platform_inc/novatel_oem615_platform_cfg.h) for the default
configuration settings, as well as a summary on overriding parameters in mission-specific repositories.

## Simulation
The default configuration returns data that is X * 0.001, Y * 0.002, and Z * 0.003 the request count after conversions:
```
<simulator>
    <name>novatel_oem615_sim</name>
    <active>true</active>
    <library>libnovatel_oem615_sim.so</library>
    <hardware-model>
        <type>NOVATEL_OEM615</type>
        <connections>
            <connection><type>command</type>
                <bus-name>command</bus-name>
                <node-name>novatel_oem615-sim-command-node</node-name>
            </connection>
            <connection><type>usart</type>
                <bus-name>usart_1</bus-name>
                <node-port>1</node-port>
            </connection>
        </connections>
        <data-provider>
            <type>NOVATEL_OEM615_PROVIDER</type>
        </data-provider>
    </hardware-model>
</simulator>
```

## 42
Optionally the 42 data provider can be configured in the `nos3-simulator.xml`:
```
        <data-provider>
            <type>NOVATEL_OEM615_42_PROVIDER</type>
            <hostname>localhost</hostname>
            <port>4245</port>
            <max-connection-attempts>5</max-connection-attempts>
            <retry-wait-seconds>5</retry-wait-seconds>
            <spacecraft>0</spacecraft>
        </data-provider>
```

## Releases
We use [SemVer](http://semver.org/) for versioning. For the versions available, see the tags on this repository.
* v1.0.0 - X/Y/Z 
  - Updated to be a component repository including FSW, GSW, Sim, and Standalone checkout
* v0.1.0 - 10/9/2021 
  - Initial release with version tagging

## Legal 
If this project interests you or if you have any questions, please feel free to contact any developer.

### License
This project is licensed under the NOSA (NASA Open Source Agreement) License. 

### Acknowledgments
* Special thanks to all the developers involved!
