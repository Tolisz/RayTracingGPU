prog: main.o
	g++ -o prog main.o -lOpenCL

main.o: main.cpp kelner.cl
	g++ -c main.cpp -o main.o

clean:
	rm main.o prog