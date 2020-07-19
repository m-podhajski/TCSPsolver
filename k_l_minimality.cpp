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
        return std::includes(A.begin(), A.end(), B.begin(), B.end());
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
            TemporalConstraint constraint(temporalConstraintLanguage, relations.size() - 1, subset);
            constraints.push_back(constraint);
        }
        TemporalCSPInstance temporalCspInstance(temporalConstraintLanguage, constraints);
        return temporalCspInstance;
    }

    static TemporalCSPInstance remove(TemporalCSPInstance cspInstance, int size) {
        std::set<int> vars = cspInstance.variables();
        std::vector<int> variables(vars.begin(), vars.end());
        while (true) {
            int endFlag = true;
            for (int i = 1; i <= size; i++) {
                for (auto subset: subsets(variables, i)) {
                    for (int j = 0; j < cspInstance.constraints.size(); j++) {
                        if(!isSubset(subset, cspInstance.constraints[j].variables)){
                            continue;
                        }
                        for (int k = 0; k < cspInstance.constraints.size(); k++) {
                            if (k == j || !isSubset(subset, cspInstance.constraints[k].variables)){
                                continue;
                            }

                            if (false) {
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

