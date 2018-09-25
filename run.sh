 #!/bin/bash

## Dataset of 9914 nodes ##
dataPath="./data/cs-stanford_data/"
nodeNum=9914

## Dataset of 100 nodes ##
#dataPath="./data/mathworks100_data/"
#nodeNum=100

## Number of threads for the parallel implemeentation
NUM_THREADS=4

echo "## Simple Iterative PageRank ##"
./pagerank_iter $dataPath $nodeNum
echo "## Gauss-Seidel Serial PageRank ##"
./pagerank_gs_omp $dataPath $nodeNum 1
echo "## Parallel Gauss-Seidel PageRank:" $NUM_THREADS "threads ##"
./pagerank_gs_omp $dataPath $nodeNum $NUM_THREADS
