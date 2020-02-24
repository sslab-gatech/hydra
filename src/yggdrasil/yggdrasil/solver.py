import subprocess
import os

import util
import z3
import solver_utils as sutils


CURRENT = os.path.dirname(os.path.realpath(__file__))
Z3_SERVER_FILE = os.path.join(CURRENT, "server.py")


def to_smt2(*terms):
    s = z3.Solver()
    s.add(*terms)
    return s.to_smt2()


class ModelProxy(object):
    def __init__(self, model_str, solver):
        self._model = model_str
        self._solver = solver

    def __str__(self):
        return self._model

    def __repr__(self):
        return self._model

    def evaluate(self, term):
        term = term.sexpr()
        return self._solver._call('model_evaluate', term)

    def eval(self, term):
        return self.evaluate(term)


class Solver(object):
    def __init__(self):
        self._proc = subprocess.Popen(['python2', Z3_SERVER_FILE],
                stdout=subprocess.PIPE,
                stdin=subprocess.PIPE,
                # stderr=subprocess.PIPE
                )

    def _write(self, command):
        sutils.write_cmd(self._proc.stdin, command)

    def _call(self, name, *args, **kwargs):
        self._write({'name': name, 'args': args, 'kwargs': kwargs})
        res = self._read()
        if 'return' in res:
            return res['return']
        if 'exc' in res:
            raise RuntimeError(res['exc'])

    def _read(self, **kwargs):
        return sutils.read_cmd(self._proc.stdout)

    def add(self, *terms):
        term = to_smt2(*terms)
        return self._call('add', term)

    def set(self, *args, **kwargs):
        return self._call('set', *args, **kwargs)

    def check(self):
        vals = {'sat': z3.sat, 'unsat': z3.unsat, 'unknown': z3.unknown}
        return vals.get(self._call('check'))

    def model(self):
        return ModelProxy(self._call('model'), self)

    def push(self):
        return self._call('push')

    def pop(self):
        return self._call('pop')


if __name__ == '__main__':
    x = util.FreshBitVec('x', 32)
    y = util.FreshBitVec('y', 32)
    s = Solver()
    s.add(x == y)
    print s.check()
    model = s.model()
    print model
    print model.evaluate(x)
    # print s.model().evaluate(x)
