import math
import json

LEN_LEN = 8

def write_cmd(stream, command):
    payload = json.dumps(command)
    assert math.log(len(payload), 10) < LEN_LEN, "payload length = {} to large".format(len(payload))
    stream.write(str(len(payload)).rjust(LEN_LEN, '0'))
    stream.write(payload)
    stream.flush()


def read(stream, count):
    v = stream.read(count)
    return v


def read_cmd(stream):
    cmdlen = read(stream, LEN_LEN)
    if not cmdlen:
        return None
    data = read(stream, int(cmdlen))
    try:
        return json.loads(data)
    except Exception, e:
        print data
        raise e

