
![TrashWatch Report](https://github.com/bounIoT/TrashWatch/blob/master/Figures/TrashWatch-One-Pager-Report.png?raw=true "TrashWatch Report")


<h2>IoT Architecture</h2>

![IoT Architecture](https://github.com/bounIoT/TrashWatch/blob/master/Figures/iot-architecture.png "IoT Architecture")

<h2>Components</h2>

| Component                   | Name                               | Action                                                   |
|-----------------------------|------------------------------------|----------------------------------------------------------|
| Ultrasonic Sensor           | HC-SR04                            | Senses the distance                                      |
| Embedded HW Platform        | Arduino YUN                        | Does the computation via built-in microprocessor         |
| Wireless/Wired Connectivity | Arduino YUN Shield(Atheros AR9331) | Connects the device to WiFi                              |
| Embedded App SW             | Arduino IDE                        | Compiles the code                                        |
| Cloud Platform              | Blynk Server                       | Receives data from Arduino YUN and sends it to Blynk App |
| Client SW                   | Blynk App                          | Visualization of data, interaction with interface        |
| LED                         |                                    | Physically shows that trash bin is full                  |
| Metal Holder                |                                    | Structure                                                |

<h2>Circuit Diagram</h2>

![Circuit Diagram](https://github.com/bounIoT/TrashWatch/blob/master/Figures/circuit-diagram.png "Circuit Diagram")
