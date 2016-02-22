import serial

from config import Config
from logger import Logger

# dictionary of all available requests and their settings
valid_cmds = {
    # external ardiono cmds
    "reset": {
        "byteId": 1,
        "parameter": None,
        "desc": "Resets the OBD2UART connection."
    },
    "available_pids": {
        "byteId": 2,
        "parameter": None,
        "desc": "Lists all available OBD-pids."
    },
    "read_all": {
        "byteId": 3,
        "parameter": None,
        "desc": "Returns values from each available OBD-pids."
    },
    "read_pid": {
        "byteId": 4,
        "parameter": int,
        "desc": "Returns the value of a handed OBD-pid (e.g. 'read_pid 100')."
    },
    "read_car": {
        "byteId": 5,
        "parameter": None,
        "desc": "Returns all available car values."
    },
    "read_dtc": {
        "byteId": 6,
        "parameter": None,
        "desc": "Returns all buffered error codes."
    },
    "delete_dtc": {
        "byteId": 7,
        "parameter": None,
        "desc": "Resets all buffered error codes."
    },
    # internal linuino cmds
    "available_logs": {
        "byteId": -1,
        "parameter": None,
        "desc": "Returns a list of all available log-files with their IDs."
    },
    "read_log": {
        "byteId": -1,
        "parameter": int,
        "desc": "Returns the content of a log-file identified by its ID " +
        "(e.g. 'read_log 1')"
    },
    "delete_log": {
        "byteId": -1,
        "parameter": int,
        "desc": "Deletes a log-file identified by its ID " +
        "(e.g. 'delete_log 1')"
    },
    "help": {
        "byteId": -1,
        "parameter": None,
        "desc": "Shows this help."
    }
}


class Commands(object):
    """
    Manages the validation and parsing of sent commands by users, for
    example data-requests to the WebSocket.
    """

    @staticmethod
    def validate_cmd(cmd):
        """
        Validates if sent data requests are valid by checking them
        against our valid_cmds-list.

        Returns
            cmdKey: Integer identifier of cmd
            cmdValue: cmd parameter, if no parameter is needed 'None'
            cmdExtern: True, command should be sent to arduino
        """
        cmd = cmd.split(" ")

        # did the user send a valid cmd?
        if cmd[0] not in valid_cmds.keys():
            errorMsg = "Error: '{0}' is not a valid command, enter 'help' for " \
                "all available commands.".format(cmd[0])
            raise Exception(errorMsg)

        # extract byteId and parameter-settings
        cmdParams = valid_cmds[cmd[0]]
        cmdKey = cmdParams["byteId"]
        cmdValue = -1

        # key found, check if a parameter is needed
        if cmdParams["parameter"] is not None:
            # parameter is needed, check type
            try:
                cmdValue = cmdParams["parameter"](cmd[1])
            except Exception:
                errorMsg = "Error: '{0}' is not a valid parameter for command " \
                    "'{1}'.".format(cmd[1], cmd[0])
                raise Exception(errorMsg)

        # is the cmd for arduino or should it be executed on linuino (for
        # example when a log is returned)
        cmdArduino = True
        if(cmdKey == -1):
            # internal cmd was found
            cmdKey = cmd[0]
            cmdArduino = False

        return (cmdKey, cmdValue, cmdArduino)

    @staticmethod
    def execute_cmd(key, value):
        """
        Executes a handed command on the linuino side.
        """
        if key == "available_logs":
            logs = Logger.get_log_list()

            result = "All available logs with their ids:\n\n" + logs
            return result
        elif key == "read_log":
            log = Logger.get_log(value)

            result = "\n" + log
            return result
        elif key == "delete_log":
            Logger.remove_log(value)

            result = "Logfile was removed successfully."
        elif key == "help":
            result = "All available commands:\n\n"

            for cmd in valid_cmds:
                help_line = "{command} - {description}\n".format(
                    command=cmd,
                    description=valid_cmds[cmd]["desc"]
                )
                result += help_line

            return result

    @staticmethod
    def request_cmd(byteId, value):
        """
        Requests the parsing of a command on the arduino side.

        The request is sent via serial-connection to arduino with a
        custom message protocol.
        """
        # setup protocol bytes
        start_byte = 0x11
        cmd_byte = byteId
        if value == -1:
            parameter_byte = 255  # 255 = no parameter
        else:
            parameter_byte = value
        check_byte = cmd_byte ^ parameter_byte
        end_byte = 0x22

        # setup message with bytes
        message = [
            start_byte, start_byte, start_byte,
            cmd_byte,
            parameter_byte,
            check_byte,
            end_byte, end_byte, end_byte
        ]

        # send package
        try:
            tty = serial.Serial(port=Config.get_tty_port(), baudrate=57600)
            tty.write(message)
            print("Request Command wrote to Serial: {0}".format(message))
        except Exception as e:
            print("Request Command serial Exception: {0}".format(e))
