#!/usr/bin/env bash
# mpirun -np 3 Numbskull graph_type num_samples num_var_on_master num_fac_on_worker num_var_per_fac
mpirun -np 3 ../cmake-build-debug/Numbskull BDC 1000 1 1 1
echo ""
mpirun -np 3 ../cmake-build-debug/Numbskull BDC 1000 1 2 1
echo ""
mpirun -np 3 ../cmake-build-debug/Numbskull BDC 1000 1 1 2
echo ""
mpirun -np 3 ../cmake-build-debug/Numbskull BDC 1000 2 1 1
