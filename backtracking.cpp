#include <algorithm>
#include <stack>
#include "constraint.h"


class Backtracking {

private:
    static bool checkValues(TemporalConstraint constraint, vector<int> values) {
        for (auto weakLinearOrder: constraint.constraintLanguage.relations[constraint.relation].tuples) {
            bool compatibleOrder = true;
            for (int i = 0; i < values.size(); i++) {
                if (values[i] == -1)
                    continue;
                for (int j = 0; j < values.size(); j++) {
                    if (values[j] == -1)
                        continue;
                    if (values[i] == values[j] && weakLinearOrder.toList()[i] != weakLinearOrder.toList()[j])
                        compatibleOrder = false;
                    if (values[i] < values[j] && weakLinearOrder.toList()[i] >= weakLinearOrder.toList()[j])
                        compatibleOrder = false;
                    if (values[i] > values[j] && weakLinearOrder.toList()[i] <= weakLinearOrder.toList()[j])
                        compatibleOrder = false;
                    if (!compatibleOrder)
                        break;
                }
                if (!compatibleOrder)
                    break;
            }
            if (compatibleOrder)
                return true;
        }
        return false;
    }

    static bool
    recursiveBacktracking(TemporalCSPInstance &cspInstance, std::unordered_map<int, int> &variableValue, int max,
                          int index, const std::vector<int> &variables) {
        if (index == variables.size())
            return true;
        int currentVariable = variables[index];
        for (int i = 0; i < max; i++) {
            variableValue[currentVariable] = i;
            int compatible = true;
            for (auto constraint: cspInstance.constraints) {
                auto conVariables = constraint.variables;
                if (std::find(conVariables.begin(), conVariables.end(), currentVariable) != conVariables.end()) {
                    std::vector<int> values;
                    for (auto variable: conVariables) {
                        values.push_back(variableValue[variable]);
                    }
                    if (!checkValues(constraint, values)) {
                        compatible = false;
                        break;
                    }
                }
            }
            if (!compatible) {
                continue;
            }
            if (recursiveBacktracking(cspInstance, variableValue, max, index + 1, variables)) {
                return true;
            }
        }
        variableValue[currentVariable] = -1;
        return false;
    }

public:
    static bool solve(TemporalCSPInstance &cspInstance) {
        std::set<int> vars = cspInstance.variables();
        std::vector<int> variables(vars.begin(), vars.end());
        int max = variables.size();
        std::unordered_map<int, int> variableValue;
        for (auto variable: variables) {
            variableValue[variable] = -1;
        }
        bool result = recursiveBacktracking(cspInstance, variableValue, max, 0, variables);
        return result;
    }
};

