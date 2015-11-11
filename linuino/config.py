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
