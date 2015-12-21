import time
import datetime

from config import Config


class Logger(object):
    def __init__(self, _postfix):
        """
        Generates a new file-name for Logger and writes it as JSON-file to
        the user's file-system.
        """
        self.log_file_name = "{0}-{1}.json".format(self.make_stamp(), _postfix)
        self.log_path = Config.get_log_path()
        # create log-file
        try:
            self.log_file = open(self.log_path + self.log_file_name, "w")
        except IOError:
            # @todo error handling
            print("Path for logfile does not exist: {0}".format(self.log_path))

    def make_stamp(self):
        """
        Generates a logging stamp from current data and time.
        """
        timestamp = time.time()
        stamp = datetime.datetime \
            .fromtimestamp(timestamp) \
            .strftime(Config.get_log_format())
        return stamp

    def write_line(self, _line):
        """
        Writes a given string as a new line to the Logger's log.
        """
        self.log_file.write("{0}\n".format(_line))

    def update(self, json):
        """
        Called from the handed data-object.
        Is used to notify Logger about new data that can be fetched.

        Afterwards Logger requests anounced data from data-object and starts
        the writing process.
        """
        # new_data = self.data.get_new_data()
        self.write_line(json)
