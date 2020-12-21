all: clean generate_relation generate_constraints 

generate_relation:
	g++ -std=c++17 generate_weak_linear_orders.cpp  -o generate_relation

generate_constraints:
	g++ -std=c++17 generate_constraints.cpp  -o generate_constraints


clean:
	$(RM) generate_constraints generate_relation
