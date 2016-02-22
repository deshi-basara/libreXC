import ConfigParser

# parse config.ini on runtime
config_file = ConfigParser.ConfigParser()
config_file.read("../config.ini")


class Config(object):
    """
    Implements a layer of static getter-methods for fetching config-values
    from config.ini.
    """

    @staticmethod
    def get_tty_port():
        return config_file.get("auth", "port")

    @staticmethod
    def get_user():
        return config_file.get("auth", "user")

    @staticmethod
    def get_password():
        return config_file.get("auth", "password")

    @staticmethod
    def get_socket_port():
        return config_file.get("websocket", "port")

    @staticmethod
    def get_socket_host():
        return config_file.get("websocket", "host")

    @staticmethod
    def get_socket_timeout():
        return float(config_file.get("websocket", "timeout"))

    @staticmethod
    def get_log_format():
        return config_file.get("logger", "format")

    @staticmethod
    def get_log_path():
        return config_file.get("logger", "path")
