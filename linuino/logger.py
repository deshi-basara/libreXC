import time
import datetime
import os

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
        except IOError as e:
            # @todo error handling
            print("Path for logfile does not exist: {0}".format(e.strerror))

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

    @staticmethod
    def get_log_list():
        """
        Fetches all available log files from the logging-path and returns
        them with a unique id.
        """
        id = 1
        result = ""
        for file in os.listdir(Config.get_log_path()):
            entry = "[{0}] - {1}\n".format(id, file)
            result += entry
            id += 1

        return result

    @staticmethod
    def get_log(id):
        """
        Fetches the content of a log-file identified by id and returns it.
        """
        # get file from id
        files = os.listdir(Config.get_log_path())
        requested_file = (files[id])

        # read file content and return it
        file_content = open(Config.get_log_path() + requested_file, "r").read()
        return file_content

    @staticmethod
    def remove_log(id):
        """
        Removes a log-file identified by id from the logging-path.
        """
        # get file from id
        files = os.listdir(Config.get_log_path())
        remove_file = (files[id])

        # remove file
        os.remove(Config.get_log_path() + remove_file)
        return
