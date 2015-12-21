# list of all available requests
valid_cmds = [
    "available_pids",
    "available_logs",

    "read_all",
    "read_pid",
    "read_car",
    "read_dtc",
    "read_log",

    "delete_dtc",
    "delete_log",

    "reset",
    "help"
]


class Commands(object):
    """
    Manages the validation and parsing of sent commands by users, for
    example data-requests to the WebSocket.
    """

    @staticmethod
    def validate_cmd(headers):
        """
        Validates if sent data requests are valid by checking them
        against our valid_cmds-list.

        Returns true, if data request is valid.
        """
        header_cmd_key = headers["cmd"]
        header_cmd_value = headers["value"]

        if header_cmd_key in valid_cmds:
            return True
        else:
            return False
