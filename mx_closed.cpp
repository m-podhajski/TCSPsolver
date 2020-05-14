#include <algorithm>
#include "constraint.h"

using namespace std;

class MxClosed {

    static int find_max_row(const std::vector<std::vector<unsigned int>> &A, int column, int current_row, int n) {
        int row = current_row;
        for (int i = current_row + 1; i < n; i++) {
            if (A[i][column] > A[row][column]) row = i;
        }
        return row;
    }


    static void gauss(std::vector<std::vector<unsigned int>> &A, int n, int m) {
        int current_row = 0;
        for (int column = 0; column < m; column++) {
            if (current_row == n) {
                break;
            }

            int max_row = find_max_row(A, column, current_row, n);

            if (A[max_row][column] == 0) {
                continue;
            }

            for (int i = column; i < m + 1; i++) {
                std::swap(A[max_row][i], A[current_row][i]);
            }


            for (int i = 0; i < n; i++) {
                if (i == current_row) continue;
                unsigned int c = A[i][column] & A[current_row][column];
                for (int j = column; j <= m; ++j) {
                    A[i][j] ^= (A[current_row][j] & c);
                }
            }
            current_row++;
        }
    }

    static void findFreeSet(const TemporalCSPInstance &instance, std::set<int> &set) {
        std::vector<std::vector<unsigned int>> matrix;
        vector<int> variables;
        std::unordered_map<int, int> toVariable;
        int i = 0;
        for (auto variable: instance.variables()) {
            if (variable == -1) continue;
            variables.push_back(variable);
            toVariable[variable] = i;
            i++;
        }
        int size = variables.size();
        for (const auto &constraint: instance.constraints) {
            vector<unsigned int> row(size + 1);
            for (auto variable: constraint.variables) {
                if (variable == -1) continue;
                row[toVariable[variable]]++;
                row[toVariable[variable]] %= 2;
            }
            matrix.push_back(row);
        }
        gauss(matrix, instance.constraints.size(), size);
        if (instance.constraints.size() >= size) {
            int notSatisfiable = true;
            for (i = 0; i < size; i++) {
                if (matrix[i][i] == 0) {
                    notSatisfiable = false;
                }
            }
            if (notSatisfiable)
                return;
        }
        for (i = 0; i < std::min(size, (int) instance.constraints.size()); i++) {
            std::set<int> solution;
            for (int j = 0; j < matrix[i].size(); j++) {
                if (matrix[i][j] == 1)
                    solution.insert(j);
            }
            if (solution.size() > 1) {
                for (auto variable: solution) {
                    set.insert(variables[variable]);
                }
            }
        }
        for (i = 0; i < size; i++) {
            int variableOccurrences = 0;
            for (int j = 0; j < std::min(size, (int) instance.constraints.size()); j++) {
                variableOccurrences += matrix[j][i];
            }
            if (variableOccurrences == 0) {
                set.insert(variables[i]);
            }
        }
    }

    static void orderedProjection(TemporalCSPInstance &instance, const std::set<int> &set) {
        std::set<int> satisfied;
        for (int i = 0; i < instance.constraints.size(); i++) {
            instance.constraints[i].orderedProjection(set);
            vector<int> active;
            for (int j = 0; j < instance.constraints[i].variables.size(); j++) {
                if (instance.constraints[i].variables[j] != -1)
                    active.push_back(j);
            }
            if (active.size() == 1) {
                satisfied.insert(i);
            }
        }
        vector<TemporalConstraint> newConstraints;
        for (int i = 0; i < instance.constraints.size(); i++) {
            if (satisfied.find(i) == satisfied.end()) {
                newConstraints.push_back(instance.constraints[i]);
            }
        }
        instance.constraints = newConstraints;

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