#!/usr/bin/python
#
# Copyright: (c) 2011 Matija Osrecki <matija.osrecki@fer.hr>
#

import sys
import random

def main():
    n = int(sys.argv[1])
    m = int(sys.argv[2])

    random.seed()

    print n

    for i in range(0, n):
        print random.uniform(0, m),

    print


if __name__ == '__main__':
    main()
