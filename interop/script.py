#!/usr/bin/python
import threading
import concurrent
from interop import AsyncClient
import signal
import requests

import os
import time

#hostname="10.10.130.10"
#portnum="80"
#username="losangeles"
#password="9886843481"
hostname="localhost"
portnum="8000"
username="testuser"
password="testpass"

mavlink_stream = "udp:127.0.0.1:14551"
qgroundcontrol_stream = "udp:192.168.1.105:14552"

usbport_id = "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AL02UQVX-if00-port0"

url = "http://" + hostname + ":" + portnum

# Launch mavproxy in a new process
mp_pid = os.fork()
if(mp_pid == 0):
    args = ["sudo", "python", "/usr/local/lib/python2.7/dist-packages/MAVProxy/mavproxy.py", \
            "--source-system=221", \
            "--daemon", \
            "--out=" + mavlink_stream, \
            "--out=" + qgroundcontrol_stream, \
            "--master=" + usbport_id]
    os.execvp("sudo", args)

# Launch interop_cli.py in a new process
ic_pid = os.fork()
if(ic_pid == 0):
    args = ["interop_cli.py", \
            "--url", url, \
            "--username", username, \
            "--password", password,  \
            "mavlink", \
            "--device", mavlink_stream]
    os.execv("interop_cli.py", args)
    exit(0);

# Make sure the execed processes gets exited on ^c
def signal_handler(signal, frame):
    os.kill(mp_pid, signal)
    os.kill(ic_pid, signal)
    exit(0)
    signal.signal(signal.SIGINT, signal_handler)

#client_instance = AsyncClient(url, username, password)

while(True):
    obstacles=client_instance.get_obstacles()
    for type in obstacles.result(timeout=1):
        for obstacle in type:
            print obstacle
        time.sleep(1)

# Wait forever.
while True:
    time.sleep(1)

