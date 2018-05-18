#!/bin/sh
num_samples=100
total_var=4800
for gtype in BDC BFD AGC AED
do for worker_num in 2 4 6 8
    do
    factor_num_per_worker=$(expr ${total_var} / ${worker_num})
    mpirun -np ${worker_num} ../cmake-build-debug/Numbskull ${gtype} ${num_samples} 1 ${factor_num_per_worker} 1
    echo ""
    done
done


