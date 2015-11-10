import ConfigParser


class Config(object):
    config_path = "../config.ini"

    def __init__(self):
        self.config = ConfigParser.ConfigParser()
        self.config.read(config_path)

    def get_user(self):
        key = "user"
        return self.config.get("auth", key)
