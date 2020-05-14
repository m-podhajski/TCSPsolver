#include <iostream>
#include "ll_closed.cpp"
#include "min_closed.cpp"
#include "mi_closed.cpp"
#include "mx_closed.cpp"



/*
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
    std::vector<TemporalRelation> relations;
    TemporalRelation relation(3, tuples);
    relations.push_back(relation);
    TemporalConstraintLanguage temporalConstraintLanguage(relations);
    std::cout << LLClosed::check_closure(temporalConstraintLanguage) << std::endl;
 */

/*std::set<WeakLinearOrder> tuples;
  WeakLinearOrder order1(4, {0,0,1,1});
  WeakLinearOrder order2(4, {1,1,0,0});
  WeakLinearOrder order3(4, {0,0,1,2});
  WeakLinearOrder order4(4, {2,1,0,0});


  tuples.insert(order1);
  tuples.insert(order2);
  tuples.insert(order3);
  tuples.insert(order3);


  std::vector<TemporalRelation> relations;
  TemporalRelation relation(3, tuples);
  relations.push_back(relation);
  TemporalConstraintLanguage temporalConstraintLanguage(relations);
  TemporalConstraint constraint1(temporalConstraintLanguage, 0, {0,1,2,3});
  TemporalConstraint constraint2(temporalConstraintLanguage, 0, {0,1,3,4});
  //TemporalConstraint constraint3(temporalConstraintLanguage, 0, {0,1,3,4});





  std::vector<TemporalConstraint> constraints = {constraint1, constraint2};
  TemporalCSPInstance temporalCspInstance(temporalConstraintLanguage, constraints);
  std::cout << MiClosed::solve(temporalCspInstance) << std::endl;*/


int main() {
    WeakLinearOrder order1(4, {0,0,1,1});
    WeakLinearOrder order2(4, {0,1,2,3});
    std::set<WeakLinearOrder> tuples;
    tuples.insert(order1);
    tuples.insert(order2);
    std::vector<TemporalRelation> relations;
    TemporalRelation relation(4, tuples);
    relations.push_back(relation);
    TemporalConstraintLanguage temporalConstraintLanguage(relations);
    TemporalConstraint constraint1(temporalConstraintLanguage, 0, {1,2,3,4});
    TemporalConstraint constraint2(temporalConstraintLanguage, 0, {1,2,4,5});
    TemporalConstraint constraint3(temporalConstraintLanguage, 0, {1,2,5,3});


    std::vector<TemporalConstraint> constraints = {constraint1, constraint2, constraint3};
    TemporalCSPInstance temporalCspInstance(temporalConstraintLanguage, constraints);
    std::cout << LLClosed::solve(temporalCspInstance) << std::endl;
    return 0;
}
