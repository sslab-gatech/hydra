import inspect
import compiler
import astor
import ast
from astor.codegen import to_source

import lfs


def foo(disk):
    print 'test'
    disk.write()
    disk.flush()
    disk.write()
    disk.flush()
    disk.write()
    disk.flush()

class RemoveFlush(ast.NodeTransformer):
    def __init__(self, root, *args, **kwargs):
        self.__root = root
        self.__objs = []
        super(RemoveFlush, self).__init__(*args, **kwargs)

    def get_all(self):
        return self.__objs

    def visit_Call(self, node):
        # from ipdb import set_trace; set_trace()
        # print to_source(node, add_line_information=False)
        if not hasattr(node, 'func'):
            return node

        if not hasattr(node.func, 'attr'):
            return node

        if not node.func.attr == 'flush':
            return node

        l = {}
        g = {}

        s = to_source(self.__root, add_line_information=False)
        eval(compile(ast.parse(s), '<string>', 'exec'), l, g)

        g['__source'] = s
        self.__objs.append(g)

def remove_flush_opt(obj):
    code = inspect.getsource(obj)
    cnode = ast.parse(code)
    rf = RemoveFlush(cnode)
    cnode = rf.visit(cnode)

    s = to_source(cnode, add_line_information=False)
    l = {}
    g = {}
    eval(compile(ast.parse(s), '<string>', 'exec'), l, g)
    g['__source'] = s
    
    objs = rf.get_all()

    objs.append(g)

    return objs

print remove_flush_opt(lfs.LFS)
for k in remove_flush_opt(lfs.LFS):
    print k['__source']
    print ""
