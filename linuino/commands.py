from arduino.bridgeclient import BridgeClient

# list of all available requests
valid_cmds = [
    "available_pids",
    "available_logs",

    "read_all",
    "read_car",
    "read_dtc",

    "delete_dtc",
    "delete_log",

    "reset",
    "help"
]

# list of all available requests that accept additional parameters
valid_cmds_with_parameter = [
    "read_pid",
    "read_log"
]


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

        if len(cmd) == 1:
            # cmd without param was sent
            cmd_key = cmd[0]
            cmd_value = False
        else:
            # cmd with param was sent
            cmd_key = cmd[0]
            cmd_value = cmd[1]

        if cmd_value is False:
            # no value specified, check regular cmd-list
            return (cmd_key in valid_cmds)
        else:
            # value specified, check cmd-list with parameters
            return (cmd_key in valid_cmds_with_parameter)

    @staticmethod
    def request_cmd(cmd):
        bridge = BridgeClient()

        cmd = cmd.split(" ")
        if len(cmd) == 1:
            # cmd without param was sent
            bridge.put(cmd[0], False)
        else:
            # cmd with param was sent
            bridge.put(cmd[0], cmd[1])
