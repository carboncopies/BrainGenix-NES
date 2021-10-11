import sys
import os.path as op

sys.path.append(op.join(sys.path[0], '../artifacts/py'))
sys.path.append(op.join(sys.path[0], '../artifacts/bin'))

import pycpptest

def main():
    s = "Hello";
    ss = pycpptest.Foo(s)
    print(ss)

if __name__ == '__main__':
    main()

