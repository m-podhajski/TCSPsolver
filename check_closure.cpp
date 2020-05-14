#include <unordered_map>
#include "constraint.h"

class CheckClosure{
private:
    static WeakLinearOrder createThirdOrder(const WeakLinearOrder &order1, const WeakLinearOrder &order2, int e) {
        std::vector<std::set<int>> orderGraph;
        int arity = order1.arity;
        orderGraph.resize(arity);
        for (int i = 0; i < arity; i++) {
            for (int j = 0; j < arity; j++) {
                if (order1.orderGraph[i].find(j) != order1.orderGraph[i].end()
                    && order2.orderGraph[i].find(j) != order2.orderGraph[i].end()) {
                    orderGraph[i].insert(j);
                }
                if (order1.orderGraph[i].find(j) != order1.orderGraph[i].end()
                    && order1.orderGraph[j].find(i) == order1.orderGraph[j].end()
                    && order1.orderGraph[i].find(e) != order1.orderGraph[i].end()) {
                    orderGraph[i].insert(j);
                }
                if (order2.orderGraph[i].find(j) != order2.orderGraph[i].end()
                    && order2.orderGraph[j].find(i) == order2.orderGraph[j].end()
                    && order1.orderGraph[e].find(j) != order1.orderGraph[e].end()
                    && order1.orderGraph[j].find(e) == order1.orderGraph[j].end()) {
                    orderGraph[i].insert(j);
                }
            }
        }

        return WeakLinearOrder(arity, orderGraph);
    }

public:
    static bool llClosure(const TemporalConstraintLanguage &constraint) {
        for (const TemporalRelation &relation: constraint.relations) {
            for (const auto &order1: relation.tuples) {
                for (const auto &order2: relation.tuples) {
                    for (int e = 0; e < relation.arity; e++) {
                        const WeakLinearOrder &thirdOrder = createThirdOrder(order1, order2, e);
                        if (relation.tuples.find(thirdOrder) == relation.tuples.end())
                            return false;
                    }
                }
            }
        }
        return true;
    }
};