#include <algorithm>
#include <stack>
#include "constraint.h"


class KLMinimality {
private:
    static bool _next(std::vector<int> &numbers, int size) {
        for (int i = size - 1; i >= 0; i--) {
            if (numbers[i] != size - 1) {
                numbers[i]++;
                break;
            } else if (i != 0) {
                numbers[i] = 0;
            } else {
                return false;
            }
        }
        return true;
    }

    static bool isSubset(vector<int> A, vector<int> B) {
        std::sort(A.begin(), A.end());
        std::sort(B.begin(), B.end());
        return std::includes(B.begin(), B.end(), A.begin(), A.end());
    }

    static std::vector<std::vector<int>> subsets(const std::vector<int> &input, int l) {
        int n = input.size();
        std::vector<std::vector<int>> result;
        long long i = (1 << l) - 1;
        while (!(i >> n)) {
            std::vector<int> v;
            for (int j = 0; j < n; j++)
                if (i & (1 << j))
                    v.push_back(input[j]);
            result.push_back(v);
            i = (i + (i & (-i))) | (((i ^ (i + (i & (-i)))) >> 2) / (i & (-i)));
        }
        return result;
    }

    static TemporalCSPInstance generateDummyConstraints(TemporalCSPInstance &cspInstance, int size) {
        std::set<WeakLinearOrder> weakLinearOrders;
        std::vector<int> numbers(size);
        weakLinearOrders.insert(WeakLinearOrder(size, numbers));
        while (_next(numbers, size)) {
            weakLinearOrders.insert(WeakLinearOrder(size, numbers));
        }
        TemporalRelation fullRelation(size, weakLinearOrders);
        std::set<int> vars = cspInstance.variables();
        std::vector<int> variables(vars.begin(), vars.end());
        std::vector<TemporalRelation> relations = cspInstance.constraintLanguage.relations;
        relations.push_back(fullRelation);
        TemporalConstraintLanguage temporalConstraintLanguage(relations);
        std::vector<TemporalConstraint> constraints = cspInstance.constraints;
        for (auto subset: subsets(variables, size)) {
            bool shouldAdd = true;
            for (auto constraint: cspInstance.constraints) {
                if (isSubset(subset, constraint.variables)) {
                    shouldAdd = false;
                    break;
                }
            }
            if (!shouldAdd) {
                continue;
            }
            TemporalConstraint constraint(temporalConstraintLanguage, relations.size() - 1, subset);
            constraints.push_back(constraint);
        }
        TemporalCSPInstance temporalCspInstance(temporalConstraintLanguage, constraints);
        return temporalCspInstance;
    }

    static vector<int> normalize(vector<int> list, std::vector<int> subset, std::vector<int> variables) {
        std::vector<int> result(subset.size());
        for (int j = 0; j < subset.size(); j++) {
            for (int i = 0; i < variables.size(); i++) {
                if (subset[j] == variables[i]) {
                    result[j] = list[i];
                }
            }
        }
        std::set<int> sortedTuple(result.begin(), result.end());
        int i = 0;
        std::unordered_map<int, int> normalizedTuple;
        for (auto variable: sortedTuple) {
            normalizedTuple[variable] = i;
            i++;
        }
        list.resize(result.size());
        for (i = 0; i < result.size(); i++) {
            int newValue = normalizedTuple[result[i]];
            list[i] = newValue;
        }
        return list;
    }

    static bool remove_not_in_intersection(TemporalCSPInstance &cspInstance, int constraint1Index, int constraint2Index,
                                           std::vector<int> subset) {
        std::unordered_map<int, std::vector<int>> linearOrderSubset1;
        std::unordered_map<int, std::vector<int>> linearOrderSubset2;
        auto order1 = cspInstance.constraintLanguage.relations[cspInstance.constraints[constraint1Index].relation].tuples;
        vector<WeakLinearOrder> constraint1(order1.begin(), order1.end());
        auto order2 = cspInstance.constraintLanguage.relations[cspInstance.constraints[constraint2Index].relation].tuples;
        vector<WeakLinearOrder> constraint2(order2.begin(), order2.end());
        for (int i = 0; i < constraint1.size(); i++) {
            auto linearOrder = constraint1[i];
            auto list = linearOrder.toList();
            list = normalize(list, subset, cspInstance.constraints[constraint1Index].variables);
            linearOrderSubset1[i] = list;
        }
        for (int i = 0; i < constraint2.size(); i++) {
            auto linearOrder = constraint2[i];
            auto list = linearOrder.toList();
            list = normalize(list, subset, cspInstance.constraints[constraint2Index].variables);
            linearOrderSubset2[i] = list;
        }
        std::vector<int> remainingOrders1, remainingOrders2;
        for (int i = 0; i < constraint1.size(); i++) {
            for (int j = 0; j < constraint2.size(); j++) {
                if (linearOrderSubset1[i] == linearOrderSubset2[j]) {
                    remainingOrders1.push_back(i);
                    break;
                }
            }
        }
        for (int i = 0; i < constraint2.size(); i++) {
            for (int j = 0; j < constraint1.size(); j++) {
                if (linearOrderSubset2[i] == linearOrderSubset1[j]) {
                    remainingOrders2.push_back(i);
                    break;
                }
            }
        }
        if (remainingOrders1.size() == constraint1.size() && remainingOrders2.size() == constraint2.size()) {
            return false;
        }
        if (remainingOrders1.size() != constraint1.size()) {
            std::set<WeakLinearOrder> weakLinearOrders;
            for (auto i: remainingOrders1) {
                weakLinearOrders.insert(constraint1[i]);
            }
            TemporalRelation relation(constraint1[0].list.size(), weakLinearOrders);
            std::vector<TemporalRelation> relations = cspInstance.constraintLanguage.relations;
            relations.push_back(relation);
            TemporalConstraintLanguage language(relations);
            std::vector<TemporalConstraint> constraints = cspInstance.constraints;
            for (auto &constraint: constraints) {
                constraint.constraintLanguage = language;
            }
            constraints[constraint1Index].relation = relations.size() - 1;
            TemporalCSPInstance instance(language, constraints);
            cspInstance = instance;
        }
        if (remainingOrders2.size() != constraint2.size()) {
            std::set<WeakLinearOrder> weakLinearOrders;
            for (auto i: remainingOrders2) {
                weakLinearOrders.insert(constraint2[i]);
            }
            TemporalRelation relation(constraint2[0].list.size(), weakLinearOrders);
            std::vector<TemporalRelation> relations = cspInstance.constraintLanguage.relations;
            relations.push_back(relation);
            TemporalConstraintLanguage language(relations);
            std::vector<TemporalConstraint> constraints = cspInstance.constraints;
            for (auto &constraint: constraints) {
                constraint.constraintLanguage = language;
            }
            constraints[constraint2Index].relation = relations.size() - 1;
            TemporalCSPInstance instance(language, constraints);
            cspInstance = instance;
        }
        return true;
    }

    static TemporalCSPInstance remove(TemporalCSPInstance cspInstance, int size) {
        std::set<int> vars = cspInstance.variables();
        std::vector<int> variables(vars.begin(), vars.end());
        while (true) {
            int endFlag = true;
            for (int i = 1; i <= size; i++) {
                for (auto subset: subsets(variables, i)) {
                    for (int j = 0; j < cspInstance.constraints.size(); j++) {
                        if (!isSubset(subset, cspInstance.constraints[j].variables)) {
                            continue;
                        }
                        for (int k = 0; k < cspInstance.constraints.size(); k++) {
                            if (k == j || !isSubset(subset, cspInstance.constraints[k].variables)) {
                                continue;
                            }
                            if (remove_not_in_intersection(cspInstance, j, k, subset)) {
                                endFlag = false;
                            }
                        }
                    }
                }
            }
            if (endFlag) {
                break;
            }
        }
        return cspInstance;
    }

public:
    static TemporalCSPInstance solve(TemporalCSPInstance &cspInstance, int k, int l) {
        TemporalCSPInstance newInstance = generateDummyConstraints(cspInstance, l);
        return remove(newInstance, k);
    }
};

