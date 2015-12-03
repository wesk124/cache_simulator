all:
	g++ -std=c++11 main.cpp cache.cpp -o simulator
clean:
	rm simulator
