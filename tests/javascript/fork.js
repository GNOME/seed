#!/usr/bin/env seed

testsuite = imports.testsuite
os = imports.os

a = os.fork()

testsuite.assert(a >= 0)
