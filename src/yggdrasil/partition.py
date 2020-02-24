from disk import *
import disk


class Partition(object):
    def __init__(self, disk, start, end):
        self._start = start
        self._end = end
        self._disk = disk

    def valid(self, bid):
        return ULT(bid, self._end - self._start)

    def read(self, bid):
        return self._disk.read(bid + self._start)

    def write(self, bid, block):
        disk.assertion(self.valid(bid))
        self._disk.write(bid + self._start, block)

    def flush(self):
        self._disk.flush()

    def crash(self, mach):
        return self.__class__(self._disk.crash(mach), self._start, self._end)
