###############################################################################
#
# The MIT License (MIT)
#
# Copyright (c) Tavendo GmbH
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
###############################################################################

from __future__ import absolute_import

from txaio._version import __version__
from txaio.interfaces import IFailedFuture, ILogger

version = __version__

# This is the API
# see tx.py for Twisted implementation
# see aio.py for asyncio/trollius implementation


class _Config:
    """
    This holds all valid configuration options, accessed as
    class-level variables. For example, if you were using asyncio:

    .. sourcecode:: python

        txaio.config.loop = asyncio.get_event_loop()

    ``loop`` is populated automatically (while importing one of the
    framework-specific libraries) but can be changed before any call
    into this library. Currently, it's only used by :meth:`call_later`
    If using asyncio, you must set this to an event-loop (by default,
    we use asyncio.get_event_loop). If using Twisted, set this to a
    reactor instance (by default we "from twisted.internet import
    reactor" on the first call to call_later)
    """
    #: the event-loop object to use
    loop = None


__all__ = (
    'using_twisted',            # True if we're using Twisted
    'using_asyncio',            # True if we're using asyncio
    'use_twisted',              # sets the library to use Twisted, or exception
    'use_asyncio',              # sets the library to use asyncio, or exception

    'config',                   # the config instance, access via attributes

    'create_future',   # create a Future (can be already resolved/errored)
    'as_future',       # call a method, and always return a Future
    'reject',          # errback a Future
    'resolve',         # callback a Future
    'add_callbacks',   # add callback and/or errback
    'gather',          # return a Future waiting for several other Futures
    'is_called',       # True if the Future has a result

    'failure_message',    # a printable error-message from a IFailedFuture
    'failure_traceback',  # returns a traceback instance from an IFailedFuture
    'failure_format_traceback',  # a string, the formatted traceback

    'make_logger',     # creates an object implementing ILogger
    'start_logging',   # initializes logging (may grab stdin at this point)

    'IFailedFuture',             # describes API for arg to errback()s
    'ILogger',                   # API for logging
)


def use_twisted():
    from txaio import tx
    _use_framework(tx)
    import txaio
    txaio.using_twisted = True
    txaio.using_asyncio = False


def use_asyncio():
    from txaio import aio
    _use_framework(aio)
    import txaio
    txaio.using_twisted = False
    txaio.using_asyncio = True


def _use_framework(module):
    """
    Internal helper, to set this modules methods to a specified
    framework helper-methods.
    """
    import txaio
    for method_name in __all__:
        if method_name in ['use_twisted', 'use_asyncio']:
            continue
        setattr(txaio, method_name,
                getattr(module, method_name))


try:
    from txaio.tx import *  # noqa
    using_twisted = True
except ImportError:
    try:
        from txaio.aio import *  # noqa
        using_asyncio = True
    except ImportError as e:  # pragma: no cover
        raise
        # pragma: no cover
        raise ImportError("Neither asyncio nor Twisted found.")
