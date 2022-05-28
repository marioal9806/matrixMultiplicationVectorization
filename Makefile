VECT_FLAGS=-O -ftree-vectorize -mavx2 -march=native -fopt-info-vec
DEBUG_FLAGS=-g -Wall

main: main.c
	gcc $(DEBUG_FLAGS) $(VECT_FLAGS) main.c -o main

# Runs the program to multiply 3x2 matrices
# 1) Sets the test/ directory
# 2) Creates the files with the appropriate number of items
# 3) Runs the executable (you must pass user input manually)
test3x2: main
	rm -rf ./test/
	mkdir ./test/
	head -n 6 sample_data/matrixA2500.txt > ./test/matrixA.txt
	head -n 6 sample_data/matrixB2500.txt > ./test/matrixB.txt
	./main

clean:
	rm -f ./main
	rm -rf ./test/
