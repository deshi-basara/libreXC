# -*- test-case-name: twisted.python.test.test_dist3 -*-
# Copyright (c) Twisted Matrix Laboratories.
# See LICENSE for details.

"""
Support for installing Twisted on Python 3.

Only necessary while parts of Twisted are unported.

@var modules: A list of modules that have been ported,
    e.g. "twisted.python.versions"; a package name (e.g. "twisted.python")
    indicates the corresponding __init__.py file has been ported
    (e.g. "twisted/python/__init__.py"). To reduce merge conflicts, add new
    lines in alphabetical sort.

@var testModules: A list of test modules that have been ported, e.g
    "twisted.python.test.test_versions". To reduce merge conflicts, add new
    lines in alphabetical sort.

@var testDataFiles: A list of filenames that are data files used by tests.
    These are generally used by another Python process that the test case
    spawns.

@var almostModules: A list of any other modules which are needed by any of the
    modules in the other two lists, but which themselves have not actually
    been properly ported to Python 3.  These modules might work well enough to
    satisfy some of the requirements of the modules that depend on them, but
    cannot be considered generally usable otherwise.

@var modulesToInstall: A list of all modules that should be installed on
    Python 3.
"""

from __future__ import division

from os import path


modules = [
    "twisted",
    "twisted._version",
    "twisted.application",
    "twisted.application.app",
    "twisted.application.internet",
    "twisted.application.reactors",
    "twisted.application.service",
    "twisted.application.test",
    "twisted.copyright",
    "twisted.cred",
    "twisted.cred._digest",
    "twisted.cred.checkers",
    "twisted.cred.credentials",
    "twisted.cred.error",
    "twisted.cred.portal",
    "twisted.cred.test",
    "twisted.internet",
    "twisted.internet._baseprocess",
    "twisted.internet._glibbase",
    "twisted.internet._newtls",
    "twisted.internet._posixstdio",
    "twisted.internet._signals",
    "twisted.internet.abstract",
    "twisted.internet.address",
    "twisted.internet.base",
    "twisted.internet.default",
    "twisted.internet.defer",
    "twisted.internet.endpoints",
    "twisted.internet.epollreactor",
    "twisted.internet.error",
    "twisted.internet.fdesc",
    "twisted.internet.gireactor",
    "twisted.internet.gtk3reactor",
    "twisted.internet.interfaces",
    "twisted.internet.kqreactor",
    "twisted.internet.main",
    "twisted.internet.pollreactor",
    "twisted.internet.posixbase",
    "twisted.internet.process",
    "twisted.internet.protocol",
    "twisted.internet.reactor",
    "twisted.internet.selectreactor",
    "twisted.internet.ssl",
    "twisted.internet.stdio",
    "twisted.internet.task",
    "twisted.internet.tcp",
    "twisted.internet.test",
    "twisted.internet.test._posixifaces",
    "twisted.internet.test.connectionmixins",
    "twisted.internet.test.modulehelpers",
    "twisted.internet.test.reactormixins",
    "twisted.internet.threads",
    "twisted.internet.udp",
    "twisted.internet.unix",
    "twisted.internet.utils",
    "twisted.logger",
    "twisted.logger._buffer",
    "twisted.logger._file",
    "twisted.logger._filter",
    "twisted.logger._flatten",
    "twisted.logger._format",
    "twisted.logger._global",
    "twisted.logger._io",
    "twisted.logger._json",
    "twisted.logger._legacy",
    "twisted.logger._levels",
    "twisted.logger._logger",
    "twisted.logger._observer",
    "twisted.logger._stdlib",
    "twisted.logger._util",
    "twisted.logger.test",
    "twisted.names",
    "twisted.names._rfc1982",
    "twisted.names._version",
    "twisted.names.cache",
    "twisted.names.client",
    "twisted.names.common",
    "twisted.names.dns",
    "twisted.names.error",
    "twisted.names.hosts",
    "twisted.names.resolve",
    "twisted.names.test",
    "twisted.persisted",
    "twisted.persisted.aot",
    "twisted.persisted.crefutil",
    "twisted.persisted.sob",
    "twisted.persisted.styles",
    "twisted.plugin",
    "twisted.plugins",
    "twisted.plugins.twisted_trial",
    "twisted.positioning",
    "twisted.positioning._sentence",
    "twisted.positioning.base",
    "twisted.positioning.ipositioning",
    "twisted.positioning.nmea",
    "twisted.protocols",
    "twisted.protocols.basic",
    "twisted.protocols.policies",
    "twisted.protocols.test",
    "twisted.protocols.tls",
    "twisted.python",
    "twisted.python._tzhelper",
    "twisted.python.compat",
    "twisted.python.components",
    "twisted.python.constants",
    "twisted.python.context",
    "twisted.python.deprecate",
    "twisted.python.dist3",
    "twisted.python.failure",
    "twisted.python.filepath",
    "twisted.python.lockfile",
    "twisted.python.log",
    "twisted.python.modules",
    "twisted.python.monkey",
    "twisted.python.procutils",
    "twisted.python.randbytes",
    "twisted.python.reflect",
    "twisted.python.runtime",
    "twisted.python.sendmsg",
    "twisted.python.systemd",
    "twisted.python.test",
    "twisted.python.test.deprecatedattributes",
    "twisted.python.test.modules_helpers",
    "twisted.python.threadable",
    "twisted.python.threadpool",
    "twisted.python.urlpath",
    "twisted.python.usage",
    "twisted.python.util",
    "twisted.python.versions",
    "twisted.scripts",
    "twisted.scripts.trial",
    "twisted.test",
    "twisted.test.iosim",
    "twisted.test.proto_helpers",
    "twisted.test.ssl_helpers",
    "twisted._threads",
    "twisted._threads._convenience",
    "twisted._threads._ithreads",
    "twisted._threads._memory",
    "twisted._threads._pool",
    "twisted._threads._team",
    "twisted._threads._threadworker",
    "twisted.trial",
    "twisted.trial._asyncrunner",
    "twisted.trial._asynctest",
    "twisted.trial._synctest",
    "twisted.trial.itrial",
    "twisted.trial.reporter",
    "twisted.trial.runner",
    "twisted.trial.test",
    "twisted.trial.test.detests",
    "twisted.trial.test.erroneous",
    "twisted.trial.test.packages",
    "twisted.trial.test.skipping",
    "twisted.trial.test.suppression",
    "twisted.trial.test.suppression",
    "twisted.trial.unittest",
    "twisted.trial.util",
    "twisted.web",
    "twisted.web._auth",
    "twisted.web._auth.basic",
    "twisted.web._auth.digest",
    "twisted.web._auth.wrapper",
    "twisted.web._element",
    "twisted.web._flatten",
    "twisted.web._newclient",
    "twisted.web._responses",
    "twisted.web._stan",
    "twisted.web._version",
    "twisted.web.error",
    "twisted.web.guard",
    "twisted.web.http_headers",
    "twisted.web.proxy",
    "twisted.web.resource",
    "twisted.web.script",
    "twisted.web.static",
    "twisted.web.template",
    "twisted.web.test",
    "twisted.web.test.requesthelper",
    "twisted.web.util",
    "twisted.web.xmlrpc",
]



testModules = [
    "twisted.application.test.test_internet",
    "twisted.application.test.test_service",
    "twisted.cred.test.test_cramauth",
    "twisted.cred.test.test_cred",
    "twisted.cred.test.test_digestauth",
    "twisted.cred.test.test_simpleauth",
    "twisted.internet.test.test_abstract",
    "twisted.internet.test.test_address",
    "twisted.internet.test.test_base",
    "twisted.internet.test.test_core",
    "twisted.internet.test.test_default",
    "twisted.internet.test.test_endpoints",
    "twisted.internet.test.test_epollreactor",
    "twisted.internet.test.test_fdset",
    "twisted.internet.test.test_filedescriptor",
    "twisted.internet.test.test_gireactor",
    "twisted.internet.test.test_glibbase",
    "twisted.internet.test.test_inlinecb",
    "twisted.internet.test.test_kqueuereactor",
    "twisted.internet.test.test_main",
    "twisted.internet.test.test_newtls",
    "twisted.internet.test.test_posixbase",
    "twisted.internet.test.test_posixprocess",
    "twisted.internet.test.test_process",
    "twisted.internet.test.test_protocol",
    "twisted.internet.test.test_sigchld",
    "twisted.internet.test.test_stdio",
    "twisted.internet.test.test_tcp",
    "twisted.internet.test.test_threads",
    "twisted.internet.test.test_tls",
    "twisted.internet.test.test_udp",
    "twisted.internet.test.test_udp_internals",
    "twisted.internet.test.test_unix",
    "twisted.logger.test.test_buffer",
    "twisted.logger.test.test_file",
    "twisted.logger.test.test_filter",
    "twisted.logger.test.test_flatten",
    "twisted.logger.test.test_format",
    "twisted.logger.test.test_global",
    "twisted.logger.test.test_io",
    "twisted.logger.test.test_json",
    "twisted.logger.test.test_legacy",
    "twisted.logger.test.test_levels",
    "twisted.logger.test.test_logger",
    "twisted.logger.test.test_observer",
    "twisted.logger.test.test_stdlib",
    "twisted.logger.test.test_util",
    "twisted.names.test.test_cache",
    "twisted.names.test.test_client",
    "twisted.names.test.test_common",
    "twisted.names.test.test_dns",
    "twisted.names.test.test_hosts",
    "twisted.names.test.test_rfc1982",
    "twisted.names.test.test_util",
    "twisted.persisted.test.test_styles",
    "twisted.positioning.test.test_base",
    "twisted.positioning.test.test_nmea",
    "twisted.positioning.test.test_sentence",
    "twisted.protocols.test.test_basic",
    "twisted.protocols.test.test_tls",
    "twisted.python.test.test_components",
    "twisted.python.test.test_constants",
    "twisted.python.test.test_deprecate",
    "twisted.python.test.test_dist3",
    "twisted.python.test.test_runtime",
    "twisted.python.test.test_sendmsg",
    "twisted.python.test.test_systemd",
    "twisted.python.test.test_tzhelper",
    "twisted.python.test.test_urlpath",
    "twisted.python.test.test_util",
    "twisted.python.test.test_versions",
    "twisted.test.test_abstract",
    "twisted.test.test_application",
    "twisted.test.test_compat",
    "twisted.test.test_context",
    "twisted.test.test_cooperator",
    "twisted.test.test_defer",
    "twisted.test.test_defgen",
    "twisted.test.test_error",
    "twisted.test.test_factories",
    "twisted.test.testutils",
    "twisted.test.test_failure",
    "twisted.test.test_fdesc",
    "twisted.test.test_internet",
    "twisted.test.test_iosim",
    "twisted.test.test_iutils",
    "twisted.test.test_lockfile",
    "twisted.test.test_log",
    "twisted.test.test_loopback",
    "twisted.test.test_modules",
    "twisted.test.test_monkey",
    "twisted.test.test_paths",
    "twisted.test.test_plugin",
    "twisted.test.test_persisted",
    "twisted.test.test_policies",
    "twisted.test.test_process",
    "twisted.test.test_randbytes",
    "twisted.test.test_reflect",
    "twisted.test.test_setup",
    "twisted.test.test_sob",
    "twisted.test.test_ssl",
    "twisted.test.test_sslverify",
    "twisted.test.test_stdio",
    "twisted.test.test_task",
    "twisted.test.test_tcp",
    "twisted.test.test_tcp_internals",
    "twisted.test.test_threadable",
    "twisted.test.test_threadpool",
    "twisted.test.test_threads",
    "twisted.test.test_twisted",
    "twisted.test.test_twistd",
    "twisted.test.test_udp",
    "twisted.test.test_unix",
    "twisted.test.test_usage",
    "twisted._threads.test.test_convenience",
    "twisted._threads.test.test_memory",
    "twisted._threads.test.test_team",
    "twisted._threads.test.test_threadworker",
    "twisted.trial.test.test_assertions",
    "twisted.trial.test.test_asyncassertions",
    "twisted.trial.test.test_deferred",
    "twisted.trial.test.test_doctest",
    "twisted.trial.test.test_keyboard",
    "twisted.trial.test.test_loader",
    "twisted.trial.test.test_log",
    "twisted.trial.test.test_output",
    "twisted.trial.test.test_plugins",
    "twisted.trial.test.test_pyunitcompat",
    "twisted.trial.test.test_reporter",
    "twisted.trial.test.test_runner",
    "twisted.trial.test.test_script",
    "twisted.trial.test.test_suppression",
    "twisted.trial.test.test_testcase",
    "twisted.trial.test.test_tests",
    "twisted.trial.test.test_util",
    "twisted.trial.test.test_warning",
    "twisted.web.test._util",
    "twisted.web.test.test_error",
    # The downloadPage tests weren't ported:
    "twisted.web.test.test_http",
    "twisted.web.test.test_flatten",
    "twisted.web.test.test_httpauth",
    "twisted.web.test.test_http_headers",
    "twisted.web.test.test_newclient",
    "twisted.web.test.test_proxy",
    "twisted.web.test.test_resource",
    "twisted.web.test.test_script",
    "twisted.web.test.test_stan",
    "twisted.web.test.test_static",
    "twisted.web.test.test_template",
    "twisted.web.test.test_util",
    "twisted.web.test.test_web",
    "twisted.web.test.test_web__responses",
    "twisted.web.test.test_webclient",
    "twisted.web.test.test_xmlrpc",
]


testDataFiles = [
    "twisted.internet.test.process_cli",
    "twisted.internet.test.process_helper",
    "twisted.positioning.test.receiver",
    "twisted.python.test.pullpipe",
    "twisted.python.test.pullpipe",
    "twisted.test._preamble",
    "twisted.test.plugin_basic",
    "twisted.test.plugin_extra1",
    "twisted.test.plugin_extra2",
    "twisted.test.process_cmdline",
    "twisted.test.process_echoer",
    "twisted.test.process_fds",
    "twisted.test.process_linger",
    "twisted.test.process_reader",
    "twisted.test.process_signal",
    "twisted.test.process_stdinreader",
    "twisted.test.process_tester",
    "twisted.test.process_tty",
    "twisted.test.process_twisted",
    "twisted.test.stdio_test_consumer",
    "twisted.test.stdio_test_halfclose",
    "twisted.test.stdio_test_hostpeer",
    "twisted.test.stdio_test_lastwrite",
    "twisted.test.stdio_test_loseconn",
    "twisted.test.stdio_test_producer",
    "twisted.test.stdio_test_write",
    "twisted.test.stdio_test_writeseq",
    "twisted.trial.test.mockdoctest",
    "twisted.trial.test.moduleself",
    "twisted.trial.test.moduletest",
    "twisted.trial.test.novars",
    "twisted.trial.test.ordertests",
    "twisted.trial.test.packages",
    "twisted.trial.test.sample",
    "twisted.trial.test.scripttest",
]


almostModules = [
    # Required for Trial
    "twisted.python.logfile",
    # Missing test coverage, see #6156:
    "twisted.internet._sslverify",
    # twisted.names.client semi-depends on twisted.names.root, but only on
    # Windows really:
    "twisted.names.root",
    # Echo is ported for twisted.application tests:
    "twisted.protocols.wire",
    # Required by twisted.test.test_twistd, which tests twisted.application:
    "twisted.scripts.twistd",
    "twisted.scripts._twistd_unix",
    # Required by twisted.test.test_twistd
    # https://twistedmatrix.com/trac/ticket/7958
    "twisted.python.fakepwd",
    # Missing test coverage:
    "twisted.protocols.loopback",
    # Minimally used by setup3.py:
    "twisted.python.dist",
    # twisted.python.filepath depends on twisted.python.win32, but on Linux it
    # only really needs to import:
    "twisted.python.win32",
    "twisted.test.reflect_helper_IE",
    "twisted.test.reflect_helper_VE",
    "twisted.test.reflect_helper_ZDE",
    # Agent code and downloadPage aren't ported, test coverage isn't complete:
    "twisted.web.client",
    # Required by twisted.web.server, no actual code here:
    "twisted.web.iweb",
    # Required by twisted.web.server for an error handling case:
    "twisted.web.html",
    # This module has a lot of missing test coverage.  What tests it has pass,
    # but it needs a lot more.  It was ported only enough to make the client
    # work.
    "twisted.web.http",
    # GzipEncoder and allowed methods functionality not ported, no doubt
    # missing lots of test coverage:
    "twisted.web.server",
]


def _processDataFileList(dataFiles):
    """
    Turn a list of file names into a format that distutils likes.

    For example:

        ["foo/bar.py", "baz/spam.py"]

    ...is transformed into...

        [("foo", ["foo/bar.py"]), ("baz", "baz/spam.py")]
    """
    files = {}

    for file in dataFiles:
        pathFragments = file.split(".")
        targetDir = path.sep.join(pathFragments[:-1])

        if not files.get(targetDir):
            files[targetDir] = []
        files[targetDir].append(path.sep.join(pathFragments) + ".py")

    return list(files.items())


modulesToInstall = modules + testModules + almostModules
