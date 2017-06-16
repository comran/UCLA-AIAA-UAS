# Module to receive MAVLink packets and forward telemetry via interoperability.
# Packet details at http://mavlink.org/messages/common#GLOBAL_POSITION_INT.

import logging
import sys
import time

from pymavlink import mavutil

from interop import Telemetry

logger = logging.getLogger(__name__)

PRINT_PERIOD = 5.0


def mavlink_latlon(degrees):
    """Converts a MAVLink packet lat/lon degree format to decimal degrees."""
    return float(degrees) / 1e7


def mavlink_alt(dist):
    """Converts a MAVLink packet millimeter format to decimal feet."""
    return dist * 0.00328084


def mavlink_heading(heading):
    """Converts a MAVLink packet heading format to decimal degrees."""
    return heading / 100.0


def proxy_mavlink(device, client):
    """Receives packets over the device and forwards telemetry via the client.

    Args:
        device: A pymavlink device name to forward.
        client: Interop Client with which to send telemetry packets.
    """
    # Create the MAVLink connection.
    print(device)
    mav = mavutil.mavlink_connection(device, source_system = 253,baud=57600, autoreconnect=True)

    # Track rates.
    sent_since_print = 0
    last_print = time.time()

    # Continuously forward packets.
    while True:
        # Get packets
        start = time.time()
        alt_and_hdg = mav.recv_match(type= 'VFR_HUD',#'GLOBAL_POSITION_INT',
                             blocking=True,
                             timeout=10.0)
        #msg = mav.recv_msg()
        if alt_and_hdg is None:
            logger.critical(
                'Did not receive MAVLink packet for over 10 seconds.')
            sys.exit(-1)
       	#received_VFR = time.time()
        long_and_lat = mav.recv_match(type= 'GPS_RAW_INT',#'GLOBAL_POSITION_INT',
                             blocking=True,
                             timeout=10.0)

        if long_and_lat is None:
            logger.critical(
                'Did not receive MAVLink packet for over 10 seconds.')
            sys.exit(-1)
        #received_GPS = time.time()
        # Convert to telemetry.
        
        telemetry = Telemetry(latitude=mavlink_latlon(long_and_lat.lat),
                              longitude=mavlink_latlon(long_and_lat.lon),
                              altitude_msl=mavlink_alt(alt_and_hdg.alt*1000),#mavlink_alt(msg.alt),
                              uas_heading=alt_and_hdg.heading)#mavlink_heading(alt_and_hdg.heading))
        
        print(telemetry)
        # Forward telemetry.
        try:
            client.post_telemetry(telemetry)
        except:
            logger.exception('Failed to post telemetry to interop.')
            sys.exit(-1)

        #posted_telemetry = time.time()
        # Track telemetry rates.
        sent_since_print += 1
        now = time.time()
        since_print = now - last_print
        if since_print > PRINT_PERIOD:
            logger.info('Telemetry rate: %f', sent_since_print / since_print)
            sent_since_print = 0
            last_print = now

        #print("took %s to get VFR packet, %s to get GPS packet, and %s to post telemetry.\n" % (str(received_VFR-start), str(received_GPS-received_VFR), str(posted_telemetry-received_GPS)))

        
