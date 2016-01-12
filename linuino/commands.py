from arduino.bridgeclient import BridgeClient

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

        # key found, check if a parameter is needed
        cmdParams = valid_cmds[cmd[0]]
        cmdValue = None
        if cmdParams["parameter"] is not None:
            # parameter is needed, check type
            try:
                cmdValue = cmdParams["parameter"](cmd[1])
            except Exception:
                errorMsg = "Error: '{0}' is not a valid parameter for command " \
                    "'{1}'.".format(cmd[1], cmd[0])
                raise Exception(errorMsg)

        return (cmd[0], cmdValue)

    @staticmethod
    def request_cmd(cmd):
        """
        Requests the parsing of a command on the arduino side.

        The request is sent via the arduino-bridge-library.
        """
        # startByte = int("11", 16)
        # cmdByte = int()

        bridge = BridgeClient()

        cmd = cmd.split(" ")
        if len(cmd) == 1:
            # cmd without param was sent
            bridge.put(cmd[0], False)
        else:
            # cmd with param was sent
            bridge.put(cmd[0], cmd[1])
