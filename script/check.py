#!/usr/bin/python
import sys

def main():
    in1 = open(sys.argv[1], 'r')
    in2 = open(sys.argv[2], 'r')

    line1 = in1.readlines()
    line2 = in2.readlines()

    if len(line1) != len(line2):
        sys.exit(1)


    n = len(line1)

    lose = False
    for i in range(0, n):
        nums1 = [float(x) for x in line1[i].rstrip().split(' ')]
        nums2 = [float(x) for x in line2[i].rstrip().split(' ')]

        if len(nums1) != len(nums2):
            sys.exit(2)

        for j in range(0, len(nums1)):
            if abs(nums1[j] - nums2[j]) > 0.05:
                print '%d, %d: %f != %f' % (i, j, nums1[j], nums2[j])
                lose = True
                
    in1.close()
    in2.close()

    if lose == True:
        sys.exit(3)

    sys.exit(0)


if __name__ == '__main__':
    main()
