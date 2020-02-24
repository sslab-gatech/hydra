import cython
import sys

from z3 import *
# from disk import *

from diskimpl cimport Concat32

from xv6inode cimport IndirectInodeDisk
from xv6inode import create_fuse_inode

def evalexp(v, *args, **kwargs):
    if callable(v):
        return evalexp(v(*args, **kwargs))
    elif hasattr(v, 'eval'):
        return evalexp(v.eval(*args, **kwargs))
    elif isinstance(v, bool):
        return v
    else:
        print v
        assert False


class Node(object):
    pass


class Lambda(Node):
    def __init__(self, fn):
        self._fn = fn

    def eval(self, *args, **kwargs):
        kwargs.pop('ranges', None)
        res = self._fn(*args, **kwargs)
        return evalexp(res)


class Quantifier(Node):
    def __init__(self, bindings, expression):
        if callable(expression):
            expression = Lambda(expression)

        self._bindings = bindings
        self._expression = expression

    def eval(self, *args, **kwargs):
        if len(self._bindings) == 1:
            return self.eval1(*args, **kwargs)
        else:
            assert(False)

    def eval1(self, *args, **kwargs):
        ranges = kwargs.get('ranges', {})

        v1 = self._bindings[0]
        r1 = ranges.get(v1, [2 ** v1.size()])
        if len(r1) == 1:
            r1 = [0, r1[0]]

        model = {}

        vv1 = r1[0]
        while vv1 < r1[1]:
            fargs = args + (vv1,)
            res = self.handle(fargs, evalexp(self._expression, *fargs, **kwargs))
            if res is not None:
                # if isinstance(res, dict):
                #     model.update(res)
                #     res = False
                if not res:
                    # model.update(kwargs)
                    # model[v1] = vv1
                    # return model
                    return res
                return res
            vv1 += 1

        return self.done()

    def z3(self, *args, **kwargs):
        fargs = args + tuple(self._bindings)
        return self._z3(self._bindings, self._expression.z3(*fargs, **kwargs))


class ForAll(Quantifier):
    def _z3(self, *args, **kwargs):
        return z3.ForAll(*args, **kwargs)

    def handle(self, arg, res):
        if not res:
            return False

    def done(self):
        return True


class Exists(Quantifier):
    def _z3(self, *args, **kwargs):
        return z3.Exists(*args, **kwargs)

    def handle(self, arg, res):
        if res:
            return True

    def done(self):
        return False


class Implies(Node):
    def __init__(self, A, B):
        self.A = A
        self.B = B

    def z3(self, *args, **kwargs):
        return z3.Implies(self.A, self.B)

    def eval(self, *args, **kwargs):
        # print "%s => %s" % (self.A, self.B)
        return not evalexp(self.A, *args, **kwargs) or evalexp(self.B, *args, **kwargs)


class And(Node):
    def __init__(self, *children):
        self._children = children

    def eval(self, *args, **kwargs):
        for i in self._children:
            if not evalexp(i, *args, **kwargs):
                return False
        return True


class Not(Node):
    def __init__(self, child):
        self._child = child

    def z3(self, disk):
        return z3.Not(*[ f(disk) for f in self._children ])

    def eval(self, *args, **kwargs):
        if isinstance(self._child, bool):
            return not self._child
        else:
            return not self._child.eval(*args, **kwargs)



class SAMap(object):
    def __init__(self):
        self._map = {}

    def __call__(self, arg):
        return self[arg]

    def __getitem__(self, arg):
        return SAEQ(self._map, arg)

    def __setitem__(self, arg, value):
        assert arg not in self._map or self._map[arg] == value
        self._map[arg] = value

    def __str__(self):
        return str(self._map)


class SAEQ(object):
    def __init__(self, map, arg):
        self._map = map
        self._arg = arg

    def __eq__(self, value):
        if self._arg in self._map:
            return self._map[self._arg] == value
        else:
            self._map[self._arg] = value
            return True


@cython.locals(impl=IndirectInodeDisk)
def run():
    predicate = (lambda impl, ino, off, pre_reverse_map=None:
            And(
                # Mapped blocks should be allocated
                Implies(impl.is_mapped(Concat32(ino, off)),
                    Not(impl.is_free(impl.mappingi(Concat32(ino, off))))),
                # Mapping should be injective
                Implies(impl.is_mapped(Concat32(ino, off)),
                    pre_reverse_map(impl.mappingi(Concat32(ino, off))) == Concat32(ino, off))))


    ino = BitVec('ino', 32)
    off = BitVec('off', 32)

    pre = ForAll([ino], ForAll([off], predicate))

    func = SAMap()
    impl = create_fuse_inode(sys.argv[1:])._idisk

    # Corrupt the image
    # impl.begin_tx()
    # impl._idisk.free(impl.mappingi(Concat32(2, 0)))
    # print impl.is_mapped(Concat32(2, 0))
    # impl.commit_tx()

    print pre.eval(impl, pre_reverse_map=func, ranges={ino: [1, 100], off: [0, 512]})
    print func
