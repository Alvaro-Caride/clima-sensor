# Clima Sensor

## Intention
Here we gather all the information avaliable on this project.

This project is intended to deploy a clima sensoring net in the Galicia Supercomputing Center CESGA. This starts with an ask to look for some reliable clima sensors to deploy and the now a days state is that we have our firts sensor integrated in the metrics database. As it have just happened the sensor is still in a testing face but we are whilling to add more sensors to the net iterating the process described below.

## Overview

The clima sensor project has 2 main separable parts. One is the from the perspective of the sensor, hardware, firmware and calibration. The other part is publushing sensor data into our database and being able to visualize it with our production tools. 

From the sensoring Device, hardware perspective, we have choosen a NodeMCU and a BME280. 
Our closest reference is this one from [Random Nerd Tutorials](https://randomnerdtutorials.com/esp8266-bme280-arduino-ide/).

We have modified the firmware porgram to export application/json to make it easier for us to integrate those readings with the database.

Also we have performed some calibration to the sensor and many tests are still ongoing.

For the publushing part we use a systemd timer that points to a python script that makes the publication. We are moving into this direction to try to unify the way we input data into our database. This timer is deployed in an administration virtual machine instanced with this purpose in mind, to unify there most of our metric exporters.

We have also design a enclosure and print it with our printer. this design it's also being refined but you can find some valid designs in [Enclosure](https://gitlab.com/cesga-sistemas/clima_sensor/-/tree/main/Enclosure).

## [Firmware](https://gitlab.com/cesga-sistemas/clima_sensor/-/blob/main/Firmware/Cesga-Prototype-Sensor-text-based.ino)

