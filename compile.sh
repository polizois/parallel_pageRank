 #!/bin/bash

 gcc pagerank_iter.c -o pagerank_iter -O3 -lm
 gcc pagerank_gs_omp.c -o pagerank_gs_omp -O3 -lm -fopenmp
