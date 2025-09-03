### RF433 Wireless Temperature and Humidity Monitoring

This repository contains the code for a simple wireless temperature and humidity monitoring system using Arduino. The project consists of two main parts: a **Transmitter (TX)** and a **Receiver (RX)**, communicating wirelessly via RF433 modules.


### Key Features
* **Wireless Data Transmission**: The system uses a pair of **RF433** modules for reliable, low-power communication over a short to medium range.
* **Temperature & Humidity Sensing**: An integrated **DHT11** sensor accurately measures the surrounding temperature and humidity.
* **Custom Communication Protocol**: The code includes a basic protocol with a **preamble**, **payload**, and a **CRC8 checksum** to ensure data integrity.
* **LCD Display**: Both the transmitter and receiver units use a 16x2 I2C LCD to display real-time sensor readings and system status.
* **Modular Design**: The code is separated into two distinct programs (`transmitter.ino` and `receiver.ino`), making it easy to set up and deploy each unit.


### How It Works
The **Transmitter (TX)** reads data from the DHT11 sensor, formats it into a string payload (e.g., "T=25.5;H=60.1"), calculates a checksum, and sends the complete data frame via the RF433 module. 

The **Receiver (RX)** continuously listens for incoming RF signals. Once a valid data frame is detected (preamble match), it reads the payload and checksum. The receiver then validates the data using the CRC8 algorithm. If the checksum is correct, the temperature and humidity values are parsed from the payload and displayed on the LCD.

This project is a great starting point for anyone interested in **IoT**, **wireless communication**, or **embedded systems**. It demonstrates fundamental concepts like data packet framing, checksum validation, and sensor integration.

Feel free to clone this repository, upload the code to your Arduino boards, and get started with your own wireless monitoring system!
