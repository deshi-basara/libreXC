import sys
from time import sleep

try:
    import serial
except:
    print("Missing 'python-pyserial' library ... install before usage.")
    sys.exit(0)


# import serial port from config file
import reset_serial_config

sleeping_time = 1  # sec


def reset(_port, _time):
    """
    Forces a reset by openning and closing the handed serial-port with 1200bps.
    """
    print("Resetting port: {}".format(_port))
    serial_con = serial.Serial(_port, 57600)
    serial_con.close()
    serial_con.setBaudrate(1200)
    serial_con.open()
    serial_con.setRTS(True)
    serial_con.setDTR(False)
    serial_con.close()
    print("Sleeping for: {} sec".format(_time))
    sleep(sleeping_time)
    print("Reset finished!")


def main():
    reset(reset_serial_config.serial_port, sleeping_time)


if __name__ == "__main__":
    main()
