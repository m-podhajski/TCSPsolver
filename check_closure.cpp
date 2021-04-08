#include <unordered_map>
#include <set>
#include <iostream>
#include "constraint.h"

class CheckClosure {
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

    static bool minClosure(const TemporalConstraintLanguage &constraint) {
        for (auto relation: constraint.relations) {
            std::vector<std::set<int>> minSets;
            for (auto tuple: relation.tuples) {
                auto list = tuple.toList();
                std::set<int> M;
                for (int i = 0; i < tuple.arity; i++) {
                    if (list[i] == 0) {
                        M.insert(i);
                    }
                }
                minSets.push_back(M);
            }
            for (auto minSet1: minSets) {
                for (auto minSet2: minSets) {
                    bool hasApropriateSet = false;
                    std::set<int> unionSet = minSet1;
                    unionSet.insert(minSet2.begin(), minSet2.end());
                    for (auto minSet3: minSets) {
                        if (minSet3 == unionSet) {
                            hasApropriateSet = true;
                            continue;
                        }
                    }
                    if (!hasApropriateSet) {
                        return false;
                    }
                }
            }
        }

        return true;
    }

    static bool miClosure(const TemporalConstraintLanguage &constraint) {
        for (auto relation: constraint.relations) {
            std::vector<std::set<int>> minSets;
            for (auto tuple: relation.tuples) {
                auto list = tuple.toList();
                std::set<int> M;
                for (int i = 0; i < tuple.arity; i++) {
                    if (list[i] == 0) {
                        M.insert(i);
                    }
                }
                minSets.push_back(M);
            }
            for (auto minSet1: minSets) {
                for (auto minSet2: minSets) {
                    bool hasApropriateSet = false;
                    std::set<int> intersect;
                    std::set_intersection(minSet1.begin(), minSet1.end(), minSet2.begin(), minSet2.end(),
                                          std::inserter(intersect, intersect.begin()));
                    if (intersect.size() == 0) {
                        continue;
                    }
                    for (auto minSet3: minSets) {
                        if (minSet3 == intersect) {
                            hasApropriateSet = true;
                            continue;
                        }
                    }
                    if (!hasApropriateSet) {
                        return false;
                    }
                }
            }
        }

        return true;
    }

    static bool mxClosure(const TemporalConstraintLanguage &constraint) {
        for (auto relation: constraint.relations) {
            std::vector<std::set<int>> minSets;
            for (auto tuple: relation.tuples) {
                auto list = tuple.toList();
                std::set<int> M;
                for (int i = 0; i < tuple.arity; i++) {
                    if (list[i] == 0) {
                        M.insert(i);
                    }
                }
                minSets.push_back(M);
            }
            for (auto minSet1: minSets) {
                for (auto minSet2: minSets) {
                    if (minSet1 == minSet2) {
                        continue;
                    }
                    bool hasApropriateSet = false;
                    std::set<int> symmetricDifferenceSet1;
                    std::set_difference(minSet1.begin(), minSet1.end(), minSet2.begin(), minSet2.end(),
                                        std::inserter(symmetricDifferenceSet1, symmetricDifferenceSet1.begin()));
                    std::set<int> symmetricDifferenceSet;
                    std::set_difference(minSet2.begin(), minSet2.end(), minSet1.begin(), minSet1.end(),
                                        std::inserter(symmetricDifferenceSet, symmetricDifferenceSet.begin()));
                    symmetricDifferenceSet.insert(symmetricDifferenceSet1.begin(), symmetricDifferenceSet1.end());
                    for (auto minSet3: minSets) {
                        if (minSet3 == symmetricDifferenceSet) {
                            hasApropriateSet = true;
                            continue;
                        }
                    }
                    if (!hasApropriateSet) {
                        return false;
                    }
                }
            }
        }

        return true;
    }
};