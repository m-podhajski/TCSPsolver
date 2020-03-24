#ifndef TCSPSOLVER_CONSTRAINT_H
#define TCSPSOLVER_CONSTRAINT_H

#include <list>
#include <set>
#include <utility>
#include <vector>

class WeakLinearOrder {
private:
    void create_order_graph(const std::vector<int> &tuple) {
        orderGraph.resize(arity);
        for (int i = 0; i < arity; i++) {
            for (int j = 0; j < arity; j++) {
                if (tuple[i] <= tuple[j]) {
                    orderGraph[i].insert(j);
                }
            }
        }
    }

public:
    const int arity;
    std::vector<std::set<int>> orderGraph;

    WeakLinearOrder(int arity, const std::vector<int> &tuple) : arity(arity) {
        create_order_graph(tuple);
    }

    WeakLinearOrder(int arity, std::vector<std::set<int>> orderGraph) : arity(arity),
                                                                        orderGraph(std::move(orderGraph)) {}
};

bool operator<(const WeakLinearOrder &wko1, const WeakLinearOrder &wko2) {
    if (wko1.arity != wko2.arity)
        return wko1.arity < wko2.arity;
    for (int i = 0; i < wko1.arity; i++) {
        if (wko1.orderGraph[i] != wko2.orderGraph[i])
            return wko1.orderGraph[i] < wko2.orderGraph[i];
    }
    return false;
}


class TemporalRelation {
public:
    const int arity;
    std::set<WeakLinearOrder> tuples;

    TemporalRelation(int arity, std::set<WeakLinearOrder> tuples) : arity(arity), tuples(std::move(tuples)) {}
};

class TemporalConstraintLanguage {
public:
    std::list<TemporalRelation> relations;

    explicit TemporalConstraintLanguage(std::list<TemporalRelation> relations) : relations(std::move(relations)) {}
};

#endif //TCSPSOLVER_CONSTRAINT_H
