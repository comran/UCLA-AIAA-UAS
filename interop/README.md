# Interop_script
This script launches a MAVProxy instance to duplicate the stream being read from the plane. One copy of the stream is exported to another port for the ground station to listen to, and the other is read by the script to post telemetry to the server.
proxy_mavlink is used to read telemetry from that stream and post it to the server. It's a modified version of the code provided in the AUVSI SUAS repo (https://github.com/auvsi-suas/interop), which gets the telemetry data from different packet types that our plane uses. 

To use this program: 
-Connect your antenna to your computer. Make sure the plane is streaming telemetry that the antenna on your computer should be able to receive.
-Launch ./script.py BEFORE you open Qgroundcontrol
