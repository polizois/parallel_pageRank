/*
    Author: Polizois Siois 8535
*/
/*
    Faculty of Electrical and Computer Engineering AUTH
    4th assignment on Parallel and Distributed Systems (7th semester)
*/
/*
		Serial implementation of the PageRank algorithm.
    In order to find the pagerank of each node it uses
    the iterative method described here: https://en.wikipedia.org/wiki/PageRank#Iterative

    The algorithm accepts as input:
    - The path that contains the data on which the algorithm will be applied.
      The data needed are:
      - A binary file containing a 2d adjacency matrix of size (NxN) and type
        (double).The matrix represents a directed graph with N nodes and edges
        represented by the elements A[i][j] (directed edge from node i to node j).
      - A text file containing a 1d array of size (N) and type (string).The
        array contains the label of each node.
      - A binary file containing a 1d array of size (N) and type (double).The
        array contains the PageRanks that have been calculated on matlab for
        the same dataset.It is used to verify that the results from this
        algorithm are correct.
    - The number of the nodes (N).

    The algorithm calculates the PageRank vector of the nodes and prints:
    - The time taken for the algorithm to converge.
    - The convergence error.
    - The iterations taken for the algorithm to converge.
    - The divergence of the calculated Pagerank vector from the matlab one.

    References:
    https://en.wikipedia.org/wiki/PageRank
*/

// NOTE COMPILE
// gcc pagerank_iter.c -o pagerank_iter -O3 -lm

// NOTE
// Accessing matrixes line-by-line is WAY FASTER than column-by-column.
// In the data I use, an (i,j) cell of the adjacency indicates an outbound
// link from node-i to node-j.
// So in order to implement the algorithm, i would normally have to
// access the matrix column-by-column in each iteration.
// In order to make the implementation faster i have to take the transpose
// of the matrix and access it line-by-line.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>


char *dataPath;
int nodeNum;

#define labelSize 200
#define d 0.85         // dumping factor
#define err 0.0001     // maximum convergence error
#define maxIters 200   // maximum number of iterations
#define matrixFileName "sparseMatrix.bin"
#define labelsFileName "labels.txt"
#define resultsFileName "matResults.bin"

// Time variables
struct timeval startwtime, endwtime;

// Fuction declarations
double timer(char *command);
double** alloc_matrix_double(int rows, int cols);
char** alloc_matrix_char(int rows, int cols);
int fileLoad_bin(char *filePath, int rows, int cols, double **dest);
int sparseFileLoad_bin(char *filePath, double **dest);
int fileLoad_txt(char *filePath, int rows, size_t cols, char **dest);
void fillArray(double *arr, int size, double value);
void countOutLinks(double *arr, double **matrix, int size);
void transpose(double **matrix, int size);

double quad_err(double *a, double *b, int size);

double checkResults(char *matResults, double *pr);


// argv[1] = dataPath
// argv[2] = nodeNum
int main(int argc, char *argv[])
{
  dataPath = argv[1]; nodeNum = atoi(argv[2]);

  // Variable Declaration
  FILE *matrixFile;
  FILE *labelsFile;
  double **matrix; // (nodeNum)x(nodeNum)
  char **labels;   // (nodeNum)x(200)
  int i, j;
  char matrixFilePath[100]; strcpy(matrixFilePath, dataPath); strcat(matrixFilePath, matrixFileName);
  char labelsFilePath[100]; strcpy(labelsFilePath, dataPath); strcat(labelsFilePath, labelsFileName);
  char resultsFilePath[100]; strcpy(resultsFilePath, dataPath); strcat(resultsFilePath, resultsFileName);


  // Memory Allocation
	matrix = alloc_matrix_double(nodeNum, nodeNum);     // Matrix
	labels = alloc_matrix_char(nodeNum, labelSize);     // Labels
  double *pr = malloc(nodeNum * sizeof(*pr));         // Latest PageRank Array
  double *pr_old = malloc(nodeNum * sizeof(*pr_old)); // Previous iteration PageRank Array
  double *outL = malloc(nodeNum * sizeof(*outL));     // Outbound Links Array
  int *indexes = malloc(nodeNum * sizeof(*indexes));  // Outbound Links Array

  // File Reading
  sparseFileLoad_bin(matrixFilePath, matrix);               // Matrix
  fileLoad_txt(labelsFilePath, nodeNum, labelSize, labels); // Labels

  // Array Initialization
  fillArray(pr, nodeNum, 1/(double)nodeNum); // initialize with 1/N
  fillArray(outL, nodeNum, 0);               // initialize with 0
  countOutLinks(outL, matrix, nodeNum);      // initialize with the outbound links of each node

  // Get the Transpose of the Matrix
  transpose(matrix, nodeNum);

  // Iterative Implementation of PageRank
  double sum;
  int iters = 0;
  timer("tic"); // Starting timer
  do
  {
    iters++;

    for(i=0;i<nodeNum;i++) pr_old[i] = pr[i]; // Update the pr_old array with the new pageranks

    for(i=0;i<nodeNum;i++) // rows
    {
      sum = 0;
      for(j=0;j<nodeNum;j++) // cols
        if(matrix[i][j]) sum += pr_old[j]/outL[j];

      // Update the PageRank of node 'i'
      pr[i] = (1-d)/nodeNum + d*sum;
    }

  // Check for convergence
  }while(quad_err(pr, pr_old, nodeNum) > err && iters < maxIters);

  // Check for convergence
  printf("Convergence time: %f sec\n", timer("toc"));  // Stop timer and print the convergence time
  printf("Error: %f\n", quad_err(pr, pr_old, nodeNum));// Print the error of the convergence
  printf("Number of iterations: %d\n", iters);         // Print the number of iterations until convergence
  printf("Divergence from matlab results: %f\n", checkResults(resultsFilePath, pr)); // Check Results
  printf("\n");

  // Print results
  // for(i=0;i<25;i++) printf("%d: pr = %f :: outLinks = %d :: %s", i+1, pr[i], (int)outL[i], labels[i]);

  // Free Allocated space
  free(matrix[0]); free(matrix);
  free(labels[0]); free(labels);
  free(pr);
  free(pr_old);
  free(outL);
  free(indexes);

  return 0;
}

// Loads data from bin file
int fileLoad_bin(char *filePath, int rows, int cols, double **dest)
{
  FILE *file;
  file=fopen(filePath,"rb");
  int temp[2];

  int i;

  for (i=0; i < rows; i++)
	{
		// Loading a row of the matrix
		if (!fread(&dest[i][0], sizeof(double), cols, file))
		{
      printf("Unable to read Matrix from file!");
      return 1;
    }
	}

  fclose(file);
  return 0;
}

// Loads data from bin file(sparse representation) to a full matrix
int sparseFileLoad_bin(char *filePath, double **dest)
{
  FILE *file;
  file=fopen(filePath,"rb");
  int temp[2];

  while(fread(&temp[0], sizeof(int), 2, file))
	{
		dest[temp[0]-1][temp[1]-1] = 1;
	}

  fclose(file);
  return 0;
}

// Loads data from txt file
int fileLoad_txt(char *filePath, int rows, size_t cols, char **dest)
{
  FILE *file;
  file=fopen(filePath,"r");

  int i;

  for (i=0; i < rows; i++)
	{
    // Loading a line of text (label)
    if(getline(&dest[i], &cols, file) == -1)
    {
      printf("Unable to read Labels from file!");
      return 1;
    }
	}

  fclose(file);
  return 0;
}

// Allocates memory for a 2d array of doubles
double** alloc_matrix_double(int rows, int cols)
{
	int i;

  double **matrix= malloc(rows * sizeof(*matrix));
  if(!matrix)
  {
  printf("Out of memory\n");
  exit(-1);
  }

  for(i=0;i<rows;i++)
  {
    matrix[i] = malloc(cols * sizeof(**matrix));
    if(!matrix[i])
    {
    printf("Out of memory\n");
    exit(-1);
    }
  }

	return matrix;
}

// Allocates memory for a 2d array of chars
char** alloc_matrix_char(int rows, int cols)
{
	int i;

  char **matrix= malloc(rows * sizeof(*matrix));
  if(!matrix)
  {
  printf("Out of memory\n");
  exit(-1);
  }

  for(i=0;i<rows;i++)
  {
    matrix[i] = malloc(cols * sizeof(**matrix));
    if(!matrix[i])
    {
    printf("Out of memory\n");
    exit(-1);
    }
  }

	return matrix;
}

// Fills a given 1d array (arr) with a given value
void fillArray(double *arr, int size, double value)
{
  for(int i=0;i<size;i++) arr[i] = value;
}

// Counts the outbound links of each node of a given adjacency matrix(2d)
// and stores them in arr(1d)
void countOutLinks(double *arr, double **matrix, int size)
{
  int i, j;
  for(i=0;i<nodeNum;i++)
  {
    for(j=0;j<nodeNum;j++)
      if(matrix[i][j]) arr[i]++;

    if(!arr[i])
    {
      for(j=0;j<nodeNum;j++) matrix[i][j] = 1;
      arr[i] = nodeNum;
    }
  }
}

// Calculates  the norm of a 1d array of doubles
double quad_err(double *a, double *b, int size)
{
  double sum = 0;

  for(int i=0;i<size;i++) sum += (a[i]-b[i])*(a[i]-b[i]);

  return sqrt(sum);
}

// use timer("tic") to start the timer and get the curent time
// use timer("toc") to get the time between this toc and the last tic
double timer(char *command)
{
  double gap = -1;

  if(command == "tic")
  {
    gettimeofday( &startwtime, NULL ); // Start timer
    endwtime.tv_sec = 0;
    endwtime.tv_usec = 0;
    gap = (double)( startwtime.tv_usec / 1.0e6 + startwtime.tv_sec );
  }
  else if(command == "toc")
  {
    gettimeofday( &endwtime, NULL );   // Stop timer
    gap = (double)( ( endwtime.tv_usec - startwtime.tv_usec ) / 1.0e6 + endwtime.tv_sec - startwtime.tv_sec );
  }

  return gap;
}

// Turns a 2d (double) matrix of (size)x(size) size into its Transpose
void transpose(double **matrix, int size)
{
  double temp;

  for(int i=1;i<size;i++)
  {
    for(int j=0;j<i;j++)
    {
      temp = matrix[i][j];
      matrix[i][j] = matrix[j][i];
      matrix[j][i] = temp;
    }
  }
}

// Calculates and returns the distance between the calculated pagerank vector and
// the one calculated by matlab
double checkResults(char *matResults, double *pr)
{
  double *prMat = malloc(nodeNum * sizeof(*prMat));
  fileLoad_bin(matResults, 1, nodeNum, &prMat);

  return quad_err(prMat, pr, nodeNum);
}
