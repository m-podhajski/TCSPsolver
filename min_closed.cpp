#include <algorithm>
#include <unordered_map>
#include "constraint.h"

class MinClosed {

    static bool empty_intersection(const std::set<int> &x, const std::vector<int> &y) {
        auto i = x.begin();
        auto j = y.begin();
        while (i != x.end() && j != y.end()) {
            if (*i == *j)
                return false;
            else if (*i < *j)
                ++i;
            else
                ++j;
        }
        return true;
    }

    static std::set<int> maximalMinSet(std::set<int> &set, const TemporalConstraint &constraint) {
        std::set<int> maximalMin;
        for (auto tuple: constraint.constraintLanguage.relations[constraint.relation].tuples) {
            auto tupleList = tuple.toList();
            std::unordered_map<int, int> values;
            bool unsatisfiable = false;
            for (int i = 0; i < constraint.variables.size(); i++) {
                if (constraint.variables[i] == -1) continue;
                if (values.find(constraint.variables[i]) == values.end()) {
                    values[constraint.variables[i]] = tupleList[i];
                } else if (values[constraint.variables[i]] != tupleList[i]) {
                    unsatisfiable = true;
                    break;
                }
            }
            if (!unsatisfiable) {
                int min = constraint.variables.size();
                for (int variable : constraint.variables) {
                    if (variable != -1 && values[variable] < min)
                        min = values[variable];
                }
                for (int variable : constraint.variables) {
                    if (variable != -1 && values[variable] == min) {
                        maximalMin.insert(variable);
                    }
                }
            }
        }
        std::set<int> intersectPrim;
        std::set_intersection(maximalMin.begin(), maximalMin.end(), set.begin(), set.end(),
                              std::inserter(intersectPrim, intersectPrim.begin()));
        return intersectPrim;
    }

    static void findFreeSet(const TemporalCSPInstance &instance, std::set<int> &set) {
        set = instance.variables();
        bool recheck = true;
        while (recheck) {
            recheck = false;
            for (const auto &constraint: instance.constraints) {
                if (!empty_intersection(set, constraint.variables)) {
                    int setSize = set.size();
                    std::set<int> minSet = maximalMinSet(set, constraint);
                    for (auto element: constraint.variables) {
                        set.erase(element);
                    }
                    set.merge(minSet);
                    if (set.size() != setSize)
                        recheck = true;
                }
            }
        }
    }

    static void orderedProjection(TemporalCSPInstance &instance, const std::set<int> &set) {
        for (auto &constraint: instance.constraints) {
            constraint.orderedProjection(set);
        }
    }

public:
    static bool solve(const TemporalCSPInstance &cspInstance) {
        TemporalCSPInstance instance = cspInstance;
        while (!instance.variables().empty()) {
            std::set<int> S;
            findFreeSet(instance, S);
            if (S.empty()) {
                return false;
            }
            orderedProjection(instance, S);
        }
        return true;

    }
};