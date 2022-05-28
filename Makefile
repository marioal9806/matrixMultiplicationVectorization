VECT_FLAGS=-O -ftree-vectorize -mavx2 -march=native -fopt-info-vec
DEBUG_FLAGS=-g -Wall

main: main.c
	gcc $(DEBUG_FLAGS) $(VECT_FLAGS) main.c -o main

test3x2: main
	rm -rf ./test/
	mkdir ./test/
	head -n 6 sample_data/matrixA2500.txt > ./test/matrixA.txt
	head -n 6 sample_data/matrixB2500.txt > ./test/matrixB.txt
	./main

clean:
	rm -f ./main
	rm -rf ./test/
