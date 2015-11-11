class Security(object):
    """
    Manages all security related tasks, like checking authentication
    credentials.
    """

    @staticmethod
    def auth_status(headers, valid_user, valid_password):
        """
        Validates if sent authentication headers match the authentication
        headers from config.ini.

        Returns true, if authentication headers are correct.
        """
        # are all needed authentication headers available?
        if "user" not in headers or "password" not in headers:
            return False

        # fetch headers and validate
        header_user = headers["user"]
        header_password = headers["password"]
        if header_user != valid_user:
            return False
        elif header_password != valid_password:
            # @todo hash password
            return False

        return True
