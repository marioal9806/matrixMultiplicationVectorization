# Matrix Multiplication

This repo is a multiplication tool for floating point double precision matrices. It is implemented with the following methods:
1) Sequential solution
2) Autovectorized solution
3) OpenMP solution

Authored by:
- [Dafne Durón](https://github.com/DafneDC)
- [Mario Ortega](https://github.com/marioal9806)

## Build the project

The build tool used for the project is GNU Make. The target to build the executable for the project is called `main`, so to build the project just run `make main`. 
An example of the output of this command is shown below:

    $ make main
    gcc table.c -c -o table.o
    gcc matrix.c -c -o matrix.o
    gcc -g -Wall -O -ftree-vectorize -mavx2 -march=native -fopt-info-vec -fopenmp main.c table.o matrix.o -o main
    main.c:37:13: optimized: loop vectorized using 32 byte vectors

### Dependencies

There are two source file dependencies for the project, each automatically compiled and linked when building the project, along with its corresponding .c file:
- **matrix.h**: Utilities, defines and data structures developed to interact with the matrices.
- **table.h**: Open Source library for printing table-like output to STDOUT.

## Run the program

The output of the build process is an executable called `main`, which can be called as an executable script from the command line:

    ./main

When the execution starts, the program will ask for the size of the input matrices A and B:

    Matrix A
    Insert number of rows N: 3
    Insert number of columns M: 2

    Matrix B
    Insert number of rows N: 2
    Insert number of columns M: 3

If the multiplication can be performed (given the size of the matrices), the program proceeds to read both matrices from its corresponding text file.

> **NOTE**: The program expects two files to be present on the current working directory:
> - matrizA.txt
> - matrizB.txt
> 
> Each file must contain a list of double precision numbers separated by newlines. The number of elements must correspond to the matrix size you
> supplied as input in the previous step.

### Program Output

Once the multiplication is carried out, the result will be stored on an output file called `matrizC.txt`, located in the current working directory.

The execution time for each method is measured and reported with a table printed to stdout:

    +--------------------------------------------------------------------+
    | Time Metrics Table                                                 |
    +-------------+---------+----------------------+---------------------+
    | Corrida     | Serial  | Paralelo 1 (Autovec) | Paralelo 2 (OpenMP) |
    +-------------+---------+----------------------+---------------------+
    | 1           | 2465399 | 1094456              | 1101384             |
    | 2           | 2551383 | 1073303              | 1086928             |
    | 3           | 2464837 | 1082596              | 1086264             |
    | 4           | 2503218 | 1065483              | 1110980             |
    | 5           | 2332819 | 927890               | 1082523             |
    +-------------+---------+----------------------+---------------------+
    | Promedio    | 2463531 | 1048745              | 1093615             |
    +-------------+---------+----------------------+---------------------+
    | % vs Serial | -       | 234                  | 225                 |
    +-------------+---------+----------------------+---------------------+

## Development environment

This project was developed using the following tools and Operating System:
- Windows Subsystem for Linux (WSL) Version 2
- Ubuntu 20.04.4 LTS
- gcc (Ubuntu 9.4.0-1ubuntu1~20.04.1) 9.4.0
- GNU Make 4.2.1
- git version 2.25.1

## Testing

The validation tests were performed in a semi-automated manner by using makefile targets for each test of interest. The tests start with the `test` prefix on the target name
To run the test, simply type `make <name_of_the_test>`. Example:

    $ make test3x2 
    rm -rf ./matrizA.txt ./matrizB.txt
    head -n 6 sample_data/matrixA2500.txt > ./matrizA.txt
    head -n 6 sample_data/matrixB2500.txt > ./matrizB.txt
    echo "3\n2\n2\n3" | ./main
    ...
  
