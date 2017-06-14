import serial
import socket
from pynmea import nmea

ser = serial.Serial('/dev/ttyUSB0', 4800, timeout=1)
latitude = ''
longitude = ''
def readgps(latitude,longitude):
    gpgga = nmea.GPGGA()
    """Read the GPG LINE using the NMEA standard"""
    while True:
        line = ser.readline()
        print line[0:6]
        if line[0:6].find('$GPGGA') == -1:
            continue
        gpgga.parse(line)
        print "Antenna altitude : " + str(gpgga.antenna_altitude)
        print line
        if "GPGGA" in line:
            latitude = line[18:26] #Yes it is positional info for lattitude
            longitude = line[31:39] #do it again
            return(latitude,longitude)
    print "Finished"

def main():
    readgps(latitude, longitude)

if __name__ == "__main__":
    main()
