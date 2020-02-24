from yggdrasil.util import BoolVal, If, And, Or


# A symbolic map that can be 'extracted' to a dict for execution


class SymbolicMap(object):
    def __init__(self):
        self._map = []

    def get(self, gkey, dresult):
        for key, value in self._map:
            try:
                cond = And(*[a == b for a, b in zip(gkey, key)])
            except:
                cond = gkey == key

            dresult = If(cond, value, dresult)
        return dresult

    def has_key(self, gkey):
        res = BoolVal(False)
        for key, _ in self._map:
            try:
                cond = And(*[a == b for a, b in zip(gkey, key)])
            except:
                cond = gkey == key

            res = Or(cond, res)

        return res

    def __setitem__(self, key, value):
        self._map.append((key, value))
