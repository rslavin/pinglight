import serial.tools.list_ports
import serial
import sys
import time
from ping3 import ping
from collections import deque

SOLID_GREEN = '1'
SOLID_YELLOW = '2'
SOLID_RED = '3'
BLINK_RED = '4'
BLINK_YELLOW = '5'
BLINK_GREEN = '6'
BLINK_REDBLACK = '7'

MAX_GOOD_PING = 70  # maximum good ping
MIN_BAD_PING = 100  # minimum bad ping
PING_PACKET_SIZE = 32
PING_WINDOW = 3  # how many pings to record before calculating color
PING_TIMEOUT = 1  # how many seconds to wait for an individual ping before calling it a timeout
PING_DELAY = 2  # how many seconds between pings

HOST = "142.250.114.113"  # google.com


def get_arduino():
    if len(sys.argv) > 1:
        return serial.Serial(f"COM{sys.argv[1]}", 115200)

    ports = serial.tools.list_ports.comports()

    # find the first device with "Arduino" in the name
    for port in ports:
        if "Arduino" in port.description:
            return serial.Serial(port.device, 115200)

    return None


def determine_color(avg, timed_out):
    if avg == 0.0:
        return BLINK_REDBLACK
    if timed_out:
        if avg < MAX_GOOD_PING:
            return BLINK_GREEN
        elif avg < MIN_BAD_PING:
            return BLINK_YELLOW
        else:
            return BLINK_RED
    else:
        if avg < MAX_GOOD_PING:
            return SOLID_GREEN
        elif avg < MIN_BAD_PING:
            return SOLID_YELLOW
        else:
            return SOLID_RED


def main():
    ser = get_arduino()
    if ser is None:
        sys.exit("No Arduino found")

    pings = deque(maxlen=PING_WINDOW)

    i = 0
    timed_out = False

    while True:
        next_ping = ping(HOST, timeout=PING_TIMEOUT, size=PING_PACKET_SIZE)

        if next_ping is None:
            next_ping = 0.0
            timed_out = True
        else:
            next_ping *= 1000

        pings.append(next_ping)
        i += 1

        if i == PING_WINDOW:
            send_color(pings, ser, timed_out)
            timed_out = False
            i = 0

        time.sleep(PING_DELAY)


def send_color(pings, ser, timed_out):
    non_zero_pings = [p for p in pings if p != 0.0]
    avg = sum(non_zero_pings) / len(non_zero_pings) if non_zero_pings else 0.0
    color = determine_color(avg, timed_out)
    try:
        ser.write(color.encode())
    except serial.SerialException as e:
        sys.stderr.write(f"Serial Communication error: {e}")


if __name__ == "__main__":
    main()
