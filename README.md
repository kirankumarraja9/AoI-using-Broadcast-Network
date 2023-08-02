# AoI-using-Broadcast-Network
Created By P. Kiran Kumar Raja on 30 December 2022.

This file contains instructions on how to set up your HOP Network and execute the files:

Configuring Your Broadcast Network
==================================
To create your Broadcast Network, You need to use one ESP8266 Board as a Transmitter and more than one ESP8266 Board as Receivers. In this, we have transmitted the messages from one Board to the other by using the MAC Addresses of the individual boards.
We have used Arduino IDE for the configuration of the boards.

System Model
==================================
We generate a sequence of messages and transmit them from one or more transmitters and receive them at the receiver( Base Station). In the
system model, we generate packets from various nodes Node 1 to Node N, and transmit them to the Base station. Each packet contains the time
stamp of the generation time u(t) when the packet was formed and is transmitted to the Base station along with the sensor output at the generated time u(t)

In the Base Station, the receiver records the time stamp t of the reception of information, and using the time stamp of the information generated the base station u(t) we calculate the Age of Information(AoI) of each message
 
