import serial

# from arduino.bridgeclient import BridgeClient

# dictionary of all available requests and their settings
valid_cmds = {
    # external ardiono cmds
    "reset": {
        "byteId": 1,
        "parameter": None
    },
    "available_pids": {
        "byteId": 2,
        "parameter": int
    },
    "read_all": {
        "byteId": 3,
        "parameter": None
    },
    "read_pid": {
        "byteId": 4,
        "parameter": int
    },
    "read_car": {
        "byteId": 5,
        "parameter": None
    },
    "read_dtc": {
        "byteId": 6,
        "parameter": None
    },
    "delete_dtc": {
        "byteId": 7,
        "parameter": None
    },
    # internal linuino cmds
    "available_logs": {
        "byteId": -1,
        "parameter": None
    },
    "read_log": {
        "byteId": -1,
        "parameter": int
    },
    "delete_log": {
        "byteId": -1,
        "parameter": int
    },
    "help": {
        "byteId": -1,
        "parameter": None
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

        Returns true, if data request is valid.
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
        cmdValue = None

        # key found, check if a parameter is needed
        if cmdParams["parameter"] is not None:
            # parameter is needed, check type
            try:
                cmdValue = cmdParams["parameter"](cmd[1])
            except Exception:
                errorMsg = "Error: '{0}' is not a valid parameter for command " \
                    "'{1}'.".format(cmd[1], cmd[0])
                raise Exception(errorMsg)

        return (cmdKey, cmdValue)

    @staticmethod
    def request_cmd(key, value):
        """
        Requests the parsing of a command on the arduino side.

        The request is sent via serial-connection to arduino and consists
        of a custom message protocol.
        """
        print(key)
        print(value)

        # setup protocol bytes
        startByte = int("11")
        cmdByte = int(key)
        parameterByte = int(value)
        checkByte = cmdByte ^ parameterByte
        endByte = int("22")

        # setup message with bytes
        message = [
            startByte, startByte, startByte,
            cmdByte,
            parameterByte,
            checkByte,
            endByte
        ]
        package = "".join(chr(x) for x in message)

        # send package
        # @todo send package via serial-connection
