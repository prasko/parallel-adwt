#!/bin/bash

for testcase in data/*
do
    echo $testcase

    file=`echo $testcase | cut -d / -f 2`

    cpuname=tmp/${file}_cpu
    cudaname=tmp/${file}_cuda

    time ./adwt_cpu < $testcase > $cpuname
    time ./adwt_cuda < $testcase > $cudaname

    continue

    ./check.py $cpuname $cudaname

    if [ $? -ne 0 ]; then 
	echo "Nije prosao"
    else
	echo "Prosao"
    fi

    echo '---'
done
