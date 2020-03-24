#include <iostream>
#include "check_closure.cpp"



int main() {
    std::set<WeakLinearOrder> tuples;
    WeakLinearOrder order1(3, {2,0,1});
    WeakLinearOrder order2(3, {2,0,3});
    WeakLinearOrder order3(3, {2,0,-1});
    WeakLinearOrder order4(3, {2,1,0});
    WeakLinearOrder order5(3, {2,3,0});
    WeakLinearOrder order6(3, {2,-1,0});

    tuples.insert(order1);
    tuples.insert(order2);
    tuples.insert(order3);
    tuples.insert(order4);
    tuples.insert(order5);
    tuples.insert(order6);
    std::list<TemporalRelation> relations;
    TemporalRelation relation(3, tuples);
    relations.push_back(relation);
    TemporalConstraintLanguage temporalConstraintLanguage(relations);
    std::cout << LLClosed::check_closure(temporalConstraintLanguage) << std::endl;
    return 0;
}
