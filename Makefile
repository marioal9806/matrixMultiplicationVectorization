VECT_FLAGS=-O -ftree-vectorize -mavx2 -march=native -fopt-info-vec
OMP_FLAGS=-fopenmp
DEBUG_FLAGS=-g -Wall

all: main

main: main.c table.o matrix.o
	gcc $(DEBUG_FLAGS) $(VECT_FLAGS) $(OMP_FLAGS) main.c table.o matrix.o -o main

table.o: table.c table.h
	gcc table.c -c -o table.o

matrix.o: matrix.c matrix.h
	gcc matrix.c -c -o matrix.o

# Runs the program to multiply 3x2 matrices
# 1) Sets the test/ directory
# 2) Creates the files with the appropriate number of items
# 3) Runs the executable with input coming from stdin
test3x2: main
	rm -rf ./matrizA.txt ./matrizB.txt
	head -n 6 sample_data/matrixA2500.txt > ./matrizA.txt
	head -n 6 sample_data/matrixB2500.txt > ./matrizB.txt
	echo "3\n2\n2\n3" | ./main

test5x5: main
	rm -rf ./matrizA.txt ./matrizB.txt
	head -n 25 sample_data/matrixA2500.txt > ./matrizA.txt
	head -n 25 sample_data/matrixB2500.txt > ./matrizB.txt
	echo "5\n5\n5\n5" | ./main

test50x50: main
	rm -rf ./matrizA.txt ./matrizB.txt
	head -n 2500 sample_data/matrixA2500.txt > ./matrizA.txt
	head -n 2500 sample_data/matrixB2500.txt > ./matrizB.txt
	echo "50\n50\n50\n50" | ./main

test1024x1024: main
	rm -rf ./matrizA.txt ./matrizB.txt
	head -n 1048576 sample_data/matrixA1048576.txt > ./matrizA.txt
	head -n 1048576 sample_data/matrixB1048576.txt > ./matrizB.txt
	echo "1024\n1024\n1024\n1024" | ./main

# The file will have less items than the expected given the matrix dimensions
# When run, input 50 as the matrix size for both matrix A and B
test-smaller-than-expected: main
	rm -rf ./matrizA.txt ./matrizB.txt
	head -n 2000 sample_data/matrixA2500.txt > ./matrizA.txt
	head -n 2000 sample_data/matrixB2500.txt > ./matrizB.txt
	echo "50\n50\n50\n50" | ./main

# The file will have more items than the expected given the matrix dimensions
# When run, input any number less than 50 as the matrix size for both matrix A and B
test-larger-than-expected: main
	rm -rf ./matrizA.txt ./matrizB.txt
	head -n 2500 sample_data/matrixA2500.txt > ./matrizA.txt
	head -n 2500 sample_data/matrixB2500.txt > ./matrizB.txt
	echo "25\n25\n25\n25" | ./main

# Input a number larger than what malloc can return to you
# Tested value for the dims was 1000000000
test-too-large-for-malloc:
	rm -rf ./matrizA.txt ./matrizB.txt
	head -n 2500 sample_data/matrixA2500.txt > ./matrizA.txt
	head -n 2500 sample_data/matrixB2500.txt > ./matrizB.txt
	echo "1000000000\n1000000000\n1000000000\n1000000000" | ./main

clean:
	rm -f ./main
	rm -f ./table.o
	rm -f ./matrix.o
	rm -rf ./matrizA.txt ./matrizB.txt
