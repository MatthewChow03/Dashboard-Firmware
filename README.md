# UBC Solar Race Car Firmware
This repository contains personal firmware iterations to the [3rd edition firmware](https://github.com/UBC-Solar/firmware_v3) of UBC Solar's "Brightside" race car.

## Boards Overview

As a member of the software team at UBC Solar, I program the control boards using C in the STM32 IDE environment. Firmware is tested using tools like debuggers in addition to physical tools such as digital logic analyzers. There are 4 main boards that the software team works on.

### Main Control Board (MCB)

The MCB is in charge of the encoder (pedal) input, regenerative braking inputs, and cruise control inputs. Its job is to package the input information for the motor controller by sending CAN messages over the car's CAN bus.

### Driver Information Display (DID)

The DID parses the messages on the CAN bus and displays the relevant information to the driver via the dashboard LCD.

### Array Monitor Board (AMB)

The function of the AMB is to measure various values from the solar arrays including voltage, current, and temperature and output it onto the CAN bus. This is vital information for the telemetry  and information systems on the car.

### Telemetry Board (TEL)

The telemetry system is composed of a cellular network that uses HTTP requests to interact with a web server in addition to a radio system that communicates with a local PC (for example, a laptop in a team vehicle that trails the race car). The TEL transmits data from the CAN bus over the cellular/radio systems. It also transmits GPS data for location and inertia measurement unit data (IMU) for instantaneous acceleration and 3 dimensional momentum.

## Dashboard in Depth (coming soon...)

### High Level Algorithm

### Physical Setup

### Improvements from Firmware V2

### Iterations

#### Polling

#### RTOS

#### Interrupts

### Future
