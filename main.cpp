#include <iostream>
#include "tests/tests.cpp"

int main() {
    WeakLinearOrder order1(4, {0, 0, 1, 1}); // {(a,b,c,d) : (a=b<c=d)}
    WeakLinearOrder order2(4, {0, 1, 2, 3}); // {(a,b,c,d) : (a<b<c<d)}

    TemporalRelation relation1(4, {order1, order2}); // R_0 = {(a,b,c,d) : (a=b^b<c^c=d)v(a<b<c<d)}
    TemporalConstraintLanguage temporalConstraintLanguage({relation1}); //sklada sie z jednej relacji - R_0


    TemporalConstraint constraint1(temporalConstraintLanguage, 0,
                                   {1, 2, 3, 4}); // R_0(x_1,x_2,x_3,x_4) - relacja R_0 o indeksie 0
    TemporalConstraint constraint2(temporalConstraintLanguage, 0,
                                   {1, 2, 4, 5}); // R_0(x_1,x_2,x_4,x_5) - relacja R_0 o indeksie 0
    TemporalConstraint constraint3(temporalConstraintLanguage, 0,
                                   {1, 2, 5, 3}); // R_0(x_1,x_2,x_5,x_3) - relacja R_0 o indeksie 0
    TemporalCSPInstance temporalCspInstance(temporalConstraintLanguage,
                                            {constraint1, constraint2,
                                             constraint3}); // R_0(x_1,x_2,x_3,x_4)^R_0(x_1,x_2,x_4,x_5)^R_0(x_1,x_2,x_5,x_3)

    std::cout << LLClosed::solve(temporalCspInstance) << std::endl;
    return 0;
}
