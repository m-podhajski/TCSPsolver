all: main.cpp 
	g++ -std=c++17 main.cpp  -o TCSPsolver

clean:
	$(RM) TCSPsolver
