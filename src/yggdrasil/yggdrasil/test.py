import os
import copy
import sys
import signal
import unittest
import subprocess
from unittest.case import (
        _ExpectedFailure,
        _UnexpectedSuccess,
        SkipTest,
        )
import disk
import multiprocessing as mp

from z3 import And, Implies, ForAll, BoolRef, Not, Solver, unsat, unknown, Z3Exception

from diskspec import Machine
from util import prove, solve

import solver


def z3_option(**kwargs):
    def decorator(fn):
        if not hasattr(fn, '_z3_options'):
            fn._z3_options = {}
        fn._z3_options.update(kwargs)
        return fn
    return decorator


def fork_bool(test):
    def inner(self):
        pid = os.fork()
        if pid:
            os.waitpid(pid, 0)
            disk.assertion(self)
            return True
        else:
            disk.assertion(Not(self))
            return False
    return inner


class DiskTest(unittest.TestCase):
    def __init__(self, *args, **kwargs):
        super(DiskTest, self).__init__(*args, **kwargs)
        disk.native = False

    def run(self, result, *args, **kwargs):
        parentp, childp = mp.Pipe()

        pid = os.fork()

        if pid:
            try:
                os.waitpid(pid, 0)
            except Exception, e:
                self.skipTest(e)
                return
            else:
                if not parentp.poll(1):
                    self.skipTest('Child non-responding')
                    return

                # XXX: Hacky..
                # If the child forked while running tests,
                # we shoud gather all the sub-results
                # Copy the original one for-deduplication>
                presult = copy.deepcopy(result)

                # Wait at most 1 second *after* child has died
                # for result.
                while parentp.poll(0):
                    res = parentp.recv()
                    for k, v in res[0].items():
                        if isinstance(v, list):
                            v = map(list, v)
                            for v1 in v:
                                v1[0] = self

                            old = getattr(presult, k)
                            prev = getattr(result, k)
                            # Append the difference to the new result
                            setattr(result, k, prev + v[len(old):])
                        else:
                            setattr(result, k, v)

            finally:
                try:
                    # Kill all children
                    os.kill(-pid, signal.SIGTERM)
                except:
                    pass
        else:
            os.setpgrp()
            try:
                self._run(childp, result, *args, **kwargs)
            except Exception, e:
                print e
            os.kill(os.getpid(), signal.SIGTERM)

    def _run(self, q, result):
        presult = copy.deepcopy(result)
        super(DiskTest, self).run(result)
        if (len(presult.errors) < len(result.errors) or
                len(presult.failures) < len(result.failures)):
            fails = result.failures
            errors = result.errors
            result.failures = result.failures[len(presult.failures):]
            result.errors = result.errors[len(presult.errors):]
            result.printErrors()
            result.failures = fails
            result.errors = errors

        output = {}

        output['expectedFailures'] = map(lambda n: (str(n[0]), n[1]), result.expectedFailures)
        output['unexpectedSuccesses'] = map(lambda n: (str(n[0]), n[1]), result.unexpectedSuccesses)
        output['failures'] = map(lambda n: (str(n[0]), n[1]), result.failures)
        output['errors'] = map(lambda n: (str(n[0]), n[1]), result.errors)
        output['testsRun'] = result.testsRun

        q.send([output])

    def enable_symbolic_execution(self):
        setattr(BoolRef, "__nonzero__", fork_bool(self))

    def setUp(self):
        disk.native = False
        disk.assertion.assertions = []
        disk.debug.debugs = []
        self.enable_symbolic_execution()

    def prove(self, claim, *args, **kwargs):
        model = self._solve(Not(claim), *args, **kwargs)
        self.assertIsNone(model)

    def solve(self, *args, **kwargs):
        model = self._solve(*args, **kwargs)
        self.assertIsNone(model)

    def psolve(self, *args, **kwargs):
        model = self._solve(And(*disk.assertion.assertions), *args, **kwargs)
        if model:
            print ""
            for msg, vs in disk.debug.debugs:
                print msg,
                for v in vs:
                    print model.evaluate(v),
                print ""
            print ""

        self.assertIsNone(model)

    def pprove(self, claim, *args, **kwargs):
        model = self._solve(And(*disk.assertion.assertions), Not(claim), *args, **kwargs)
        if model:
            print ""
            for msg, vs in disk.debug.debugs:
                print msg,
                for v in vs:
                    print model.evaluate(v),
                print ""
            print ""

        self.assertIsNone(model)

    def show(self, *args, **kwargs):
        model = self._solve(*args, **kwargs)
        self.assertIsNotNone(model)
        return model

    def _solve(self, *args, **keywords):
        s = solver.Solver()

        s.set(**keywords)

        s.add(*args)

        if keywords.get('show', False):
            print(s)

        smt = os.getenv('SMT')
        if smt:
            PIPE = subprocess.PIPE
            args = [ "{}={}".format(a, str(b).lower()) for a, b in keywords.items() ]
            p = subprocess.Popen(smt.split() + args, stdin=PIPE, stdout=PIPE, stderr=PIPE)
            stdout, stderr = p.communicate(s.to_smt2())
            stdout = stdout.strip()
            if stdout == 'sat':
                return 'model'
            elif stdout == 'unsat':
                r = unsat
            else:
                print args
                print s.to_smt2()
                r = unknown
        else:
            r = s.check()
        if r == unsat:
            return None
        elif r == unknown:
            self.fail("Solver failed to solve")
        else:
            return s.model()


class RefinementMeta(type):
    def __new__(cls, name, parents, dct):
        for k, v in dct.items():
            if k.startswith('nop_'):
                fname = k[4:]
                dct['test_nop_%s' % k] = lambda self, fname=fname, fn=v: self._nop_fn(fname, fn, True)

            if k.startswith('match_'):
                fname = k[6:]
                if not getattr(v, 'nocrash', False):
                    dct['test_%s_crash' % k] = lambda self, fname=fname, fn=v: self._match_fn(fname, fn, True)
                dct['test_%s' % k] = lambda self, fname=fname, fn=v: self._match_fn(fname, fn, False)
                dct['test_%s_pre' % k] = lambda self, fname=fname, fn=v: self._check_pre(fname, fn)

        return super(RefinementMeta, cls).__new__(cls, name, parents, dct)


class RefinementTest(DiskTest):
    __metaclass__ = RefinementMeta
    DEBUG = False

    def setUp(self):
        super(RefinementTest, self).setUp()

    def pre(self, *args, **kwargs):
        return self.equivalence(*args, **kwargs)

    def post(self, *args, **kwargs):
        return self.equivalence(*args, **kwargs)

    def pre_post(self, *args, **kwargs):
        spec, impl, _, _ = yield self.pre(*args, **kwargs)
        kwargs['spec'] = spec
        kwargs['impl'] = impl
        yield self.post(*args, **kwargs)

    def _debug(self, *args, **kwargs):
        if self.DEBUG:
            print args, kwargs

    def assumption(self, name, spec_mach, impl_mach):
        return And(spec_mach.assumption,
                   impl_mach.assumption,
                   *disk.assertion.assertions)

    def _check_pre(self, fname, fn):
        self._debug('Running', fname, fn)

        for args in fn(self):
            self._debug('Iteration', args)

            disk.native = False
            self.enable_symbolic_execution()
            disk.assertion.assertions = []
            disk.debug.debugs = []

            spec_mach = Machine()
            impl_mach = Machine()

            spec = self.create_spec(spec_mach)
            self.assertIsNotNone(spec)

            impl = self.create_impl(impl_mach)
            self.assertIsNotNone(impl)

            pp = self.pre_post(spec=spec, impl=impl,
                    spec_mach=spec_mach, impl_mach=impl_mach,
                    fnargs=args, crash=False, fname=fname, pre=True)

            pre = pp.next()

            assumption = self.assumption(fname, spec_mach, impl_mach)

            self.show(assumption, pre)

    def _match_fn(self, fname, fn, crash):
        self._debug('Running', fname, fn)

        for args in fn(self):
            self._debug('Iteration', args)

            disk.native = False
            self.enable_symbolic_execution()
            disk.assertion.assertions = []
            disk.debug.debugs = []

            spec_mach = Machine()
            impl_mach = Machine()

            spec = self.create_spec(spec_mach)
            self.assertIsNotNone(spec)
            impl = self.create_impl(impl_mach)
            self.assertIsNotNone(impl)

            pp = self.pre_post(spec=spec, impl=impl,
                    spec_mach=spec_mach, impl_mach=impl_mach,
                    fnargs=args, crash=crash, fname=fname, pre=False)

            pre = pp.next()

            self._debug('Pre', pre)

            if hasattr(self, 'call_%s' % fname):
                rets = getattr(self, 'call_%s' % fname)(spec, impl, args)
                if rets is None:
                    rets = (None, None)
            else:
                if hasattr(self, 'call_%s_spec' % fname):
                    rets_spec = getattr(self, 'call_%s_spec' % fname)(spec, args)
                else:
                    # Guess something sensible..
                    if hasattr(spec, 'begin_tx'):
                        spec.begin_tx()
                    rets_spec = getattr(spec, fname)(*args)
                    if hasattr(spec, 'commit_tx'):
                        spec.commit_tx()

                if hasattr(self, 'call_%s_impl' % fname):
                    rets_impl = getattr(self, 'call_%s_impl' % fname)(impl, args)
                else:
                    # Guess something sensible..
                    if hasattr(impl, 'begin_tx'):
                        impl.begin_tx()
                    rets_impl = getattr(impl, fname)(*args)
                    if hasattr(impl, 'commit_tx'):
                        impl.commit_tx()

                rets = (rets_spec, rets_impl)

            if crash:
                if hasattr(self, 'crash_impl'):
                    impl = self.crash_impl(impl)
                else:
                    impl = impl.crash(Machine())
                if hasattr(self, 'crash_spec'):
                    spec = self.crash_spec(spec)
                else:
                    spec = spec.crash(Machine())

            post = pp.send((spec, impl, args, rets))

            assumption = self.assumption(fname, spec_mach, impl_mach)

            self._debug('Post', post)
            self._debug('Assumption', assumption)

            if self.DEBUG or getattr(fn, 'debug', False):
                self._debug('Precondition sat', self.show(pre))

            if crash:
                opt = {
                    'AUTO_CONFIG': False,
                    # 'MBQI': True
                }

                opt.update(getattr(fn, '_z3_options', {}))

                if spec_mach.control:
                    model = self._solve(assumption,
                        ForAll(spec_mach.control, Not(Implies(pre, post))), **opt)
                else:
                    model = self._solve(assumption, Not(Implies(pre, post)), **opt)
                if model:
                    print ""
                    for msg, vs in disk.debug.debugs:
                        print msg,
                        for v in vs:
                            print model.evaluate(v),
                        print ""
                    print ""

                    spec_mach.explain(model)
                    impl_mach.explain(model)
                self.assertIsNone(model)
            else:
                opt = {
                }

                opt.update(getattr(fn, '_z3_options', {}))

                self.psolve(And(*spec_mach.control), And(*impl_mach.control), Not(Implies(pre, post)), **opt)

    def _nop_fn(self, fname, fn, crash):
        self._debug('Running', fname, fn)

        for args in fn(self):
            self._debug('Iteration', args)

            disk.native = False
            self.enable_symbolic_execution()
            disk.assertion.assertions = []
            disk.debug.debugs = []

            spec_mach = Machine()
            impl_mach = Machine()

            spec = self.create_spec(spec_mach)
            self.assertIsNotNone(spec)
            impl = self.create_impl(impl_mach)
            self.assertIsNotNone(impl)

            pp = self.pre_post(spec=spec, impl=impl,
                    spec_mach=spec_mach, impl_mach=impl_mach,
                    fnargs=args, crash=crash, fname=fname, pre=False)

            pre = pp.next()

            self._debug('Pre', pre)

            if hasattr(self, 'call_%s' % fname):
                rets = getattr(self, 'call_%s' % fname)(None, impl, args)
                if rets is None:
                    rets = (None, None)
            else:
                if hasattr(self, 'call_%s_impl' % fname):
                    rets_impl = getattr(self, 'call_%s_impl' % fname)(impl, args)
                else:
                    # Guess something sensible..
                    if hasattr(impl, 'begin_tx'):
                        impl.begin_tx()
                    rets_impl = getattr(impl, fname)(*args)
                    if hasattr(impl, 'commit_tx'):
                        impl.commit_tx()

                rets = (None, rets_impl)

            if crash:
                if hasattr(self, 'crash_impl'):
                    impl = self.crash_impl(impl)
                else:
                    impl = impl.crash(Machine())

            post = pp.send((spec, impl, args, rets))

            assumption = self.assumption(fname, spec_mach, impl_mach)

            self._debug('Post', post)
            self._debug('Assumption', assumption)

            if self.DEBUG or getattr(fn, 'debug', False):
                self._debug('Precondition sat', self.show(pre))

            if crash:
                opt = {
                    'AUTO_CONFIG': False,
                }

                opt.update(getattr(fn, '_z3_options', {}))

                model = self._solve(assumption, Not(Implies(pre, post)), **opt)

                if model:
                    print ""
                    for msg, vs in disk.debug.debugs:
                        print msg,
                        for v in vs:
                            print model.evaluate(v),
                        print ""
                    print ""

                    spec_mach.explain(model)
                    impl_mach.explain(model)
                self.assertIsNone(model)
            else:
                opt = {
                }

                opt.update(getattr(fn, '_z3_options', {}))

                self.psolve(And(*spec_mach.control), And(*impl_mach.control), Not(Implies(pre, post)), **opt)



def main(*args, **kwargs):
    unittest.main(*args, **kwargs)
