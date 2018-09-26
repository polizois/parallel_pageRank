# parallel_pageRank
Faculty of Electrical and Computer Engineering AUTH
4th assignment on Parallel and Distributed Systems (7th semester)

Two **versions** of the PageRank program provided:
* Serial version implementing the iterative method (*pagerank_iter.c*)
* Parallel version using OpenMP and implementing the Gauss-Seidel method (*pagerank_gs_omp.c*)

In order to **compile** the programs run the following commands
* For pagerank_iter.c run      : *gcc pagerank_iter.c -o pagerank_iter -O3 -lm*
* For pagerank_gs_omp.c run    : *gcc pagerank_gs_omp.c -o pagerank_gs_omp -O3 -lm -fopenmp*
* (or you can run the provided __*compile.sh*__ to compile both)

The programs accept the following arguments:
1. *(both versions)*: __The path that contains the data on which the algorithm will be applied.__ The data needed are:
    * *sparseMatrix.bin*: A binary file containing an adjacency matrix of size (NxN) in sparse representation.So the binary file actually contains an integer array of 2 columns and rows as many as the non-zero elements of the full adjacency matrix.The first column contains the row-number of the non-zero element on the full adjacency matrix and the second column contains the column-number of the element.
The matrix represents a directed graph with N nodes and edges defined by the elements A[i][j] (directed edge from node i to node j).
    * *labels.txt*: A text file containing a 1d array of size (N) and type (string).The
array contains the label of each node.
    * *matResults.bin*: A binary file containing a 1d array of size (N) and type (double).The
array contains the PageRanks that have been calculated on matlab for
the same dataset.It is used to verify that the results from this
algorithm are correct.
2. *(both versions)*: __The number of the nodes (N) of the given dataset.__
3. *(parallel version only)*: __The number of threads to be used for the parallelism.__

In order to **execute** the programs you can run the provided __*run.sh*__.  
You can also **edit** the __*run.sh*__ in order to change the dataset(and it's size) to be used and the number of threads to be created.

Both of the programs calculate the PageRank vector of the nodes and print:
  * The time taken for the algorithm to converge.
  * The convergence error.
  * The iterations taken for the algorithm to converge.
  * The divergence of the calculated Pagerank vector from the matlab one.
  
  
  
