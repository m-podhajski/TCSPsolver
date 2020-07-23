#include <cassert>
#include "../ll_closed.cpp"
#include "../min_closed.cpp"
#include "../mi_closed.cpp"
#include "../mx_closed.cpp"
#include "../k_l_minimality.cpp"
#include "../backtracking.cpp"

class Tests {
public:
    static void test_ll_closed() {
        WeakLinearOrder order1(4, {0, 0, 1, 1});
        WeakLinearOrder order2(4, {0, 1, 2, 3});
        std::set<WeakLinearOrder> tuples;
        tuples.insert(order1);
        tuples.insert(order2);
        std::vector<TemporalRelation> relations;
        TemporalRelation relation(4, tuples); // R = {(a,b,c,d) : (a=b^b<c^c=d)v(a<b<c<d)}
        relations.push_back(relation);
        TemporalConstraintLanguage temporalConstraintLanguage(relations);

        TemporalConstraint constraint1(temporalConstraintLanguage, 0, {1, 2, 3, 4});
        TemporalConstraint constraint2(temporalConstraintLanguage, 0, {1, 2, 4, 5});
        TemporalConstraint constraint3(temporalConstraintLanguage, 0, {1, 2, 5, 3});
        std::vector<TemporalConstraint> constraints = {constraint1, constraint2, constraint3};
        TemporalCSPInstance temporalCspInstance(temporalConstraintLanguage,
                                                constraints); // R(x_1,x_2,x_3,x_4)^R(x_1,x_2,x_4,x_5)^R(x_1,x_2,x_5,x_3)

        assert(LLClosed::solve(temporalCspInstance));
    }

    static void test_mi_closed() {

    }

    static void test_min_closed() {
        WeakLinearOrder order1(3, {2, 1, 0});
        WeakLinearOrder order2(3, {2, 0, 1});
        WeakLinearOrder order3(3, {1, 2, 0});
        WeakLinearOrder order4(3, {1, 0, 2});
        WeakLinearOrder order5(3, {1, 0, 0});
        WeakLinearOrder order6(3, {1, 0, 1});
        WeakLinearOrder order7(3, {1, 1, 0});

        TemporalRelation relation(3, {order1, order2, order3, order4, order5, order6,
                                      order7}); // R = {(a,b,c) : a>b v a>c}
        TemporalConstraintLanguage temporalConstraintLanguage({relation});

        TemporalConstraint constraint1(temporalConstraintLanguage, 0, {0, 1, 1});
        TemporalConstraint constraint2(temporalConstraintLanguage, 0, {1, 0, 0});
        TemporalCSPInstance temporalCspInstance(temporalConstraintLanguage,
                                                {constraint1, constraint2}); //R(x_0,x_1,x_1)^R(x_1,x_0,x_0)

        assert(!MinClosed::solve(temporalCspInstance));
    }

    static void test_mx_closed() {
        WeakLinearOrder order1(4, {0, 0, 1, 1});
        WeakLinearOrder order2(4, {1, 1, 0, 0});
        WeakLinearOrder order3(4, {0, 0, 1, 2});
        WeakLinearOrder order4(4, {2, 1, 0, 0});
        TemporalRelation relation(4, {order1, order2, order3,
                                      order4}); // R = {(a,b,c,d) : (a=b^b<c^c=d)v(a=b>c=d)v(a=b^b<c^c<d)v(a>b>c=d)}
        TemporalConstraintLanguage temporalConstraintLanguage({relation});
        TemporalConstraint constraint1(temporalConstraintLanguage, 0, {0, 1, 2, 3});
        TemporalConstraint constraint2(temporalConstraintLanguage, 0, {0, 1, 3, 4});
        TemporalCSPInstance temporalCspInstance(temporalConstraintLanguage,
                                                {constraint1, constraint2}); //R(x_0,x_1,x_2,x_3)^R(x_0,x_1,x_3,x_4)
        assert(MxClosed::solve(temporalCspInstance));
    }

    static void test_kl_minimality() {
        WeakLinearOrder order1(3, {1, 0, 2});
        WeakLinearOrder order2(3, {0, 0, 1});
        WeakLinearOrder order3(3, {0, 1, 2});
        WeakLinearOrder order4(3, {0, 0, 1});
        TemporalRelation relation(3, {order1, order2});
        TemporalRelation relation2(3, {order3, order4});

        TemporalConstraintLanguage temporalConstraintLanguage({relation, relation2});
        TemporalConstraint constraint1(temporalConstraintLanguage, 0, {0, 1, 2});
        TemporalConstraint constraint2(temporalConstraintLanguage, 1, {0, 1, 2});
        TemporalCSPInstance temporalCspInstance(temporalConstraintLanguage,
                                                {constraint1, constraint2});
        TemporalCSPInstance klMinimalInstance = KLMinimality::solve(temporalCspInstance, 2, 3);
        assert(Backtracking::solve(klMinimalInstance));
    }

    static void test_backtracking() {
        WeakLinearOrder order1(4, {0, 1, 2, 2});
        TemporalRelation relation(4, {order1}); // R = {(a,b,c,d) : (a<b<c=d)}
        TemporalConstraintLanguage temporalConstraintLanguage({relation});
        TemporalConstraint constraint1(temporalConstraintLanguage, 0, {0, 1, 2, 3});
        TemporalConstraint constraint2(temporalConstraintLanguage, 0, {3, 4, 5, 6});
        TemporalCSPInstance temporalCspInstance(temporalConstraintLanguage,
                                                {constraint1, constraint2}); //R(x_0,x_1,x_2,x_3)^R(x_3,x_4,x_5,x_6)
        assert(Backtracking::solve(temporalCspInstance));
    }
};