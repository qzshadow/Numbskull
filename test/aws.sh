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

#mpirun -np 3 ./Numbskull BDC 1000 1 1 1
#echo ""
#mpirun -np 3 ./Numbskull BDC 1000 1 2 1
#echo ""
#mpirun -np 3 ./Numbskull BDC 1000 1 1 2
#echo ""
#mpirun -np 3 ./Numbskull BDC 1000 2 1 1

