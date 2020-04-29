#include <algorithm>
#include <unordered_map>
#include "constraint.h"

class MiClosed {
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

    static std::set<int> minimalMinSet(std::set<int> &set, const TemporalConstraint &constraint){
        std::set<int> minimalMin;
        for (auto tuple: constraint.constraintLanguage.relations[constraint.relation].tuples) {
            std::set<int> currentMin;
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
                        currentMin.insert(variable);
                    }
                }
            }
            bool doesContain = true;
            std::set<int> intersectPrim;
            std::set_intersection(constraint.variables.begin(), constraint.variables.end(), set.begin(), set.end(),
                                  std::inserter(intersectPrim, intersectPrim.begin()));
            for(auto variable: intersectPrim){
                if(currentMin.find(variable) == currentMin.end()){
                    doesContain = false;
                }
            }
            if(doesContain){
                if(minimalMin.empty()){
                    minimalMin.insert(currentMin.begin(), currentMin.end());
                }else{
                    std::set<int> intersect;
                    std::set_intersection(minimalMin.begin(), minimalMin.end(), currentMin.begin(), currentMin.end(),
                                          std::inserter(intersect, intersect.begin()));
                    minimalMin = intersect;
                }
            }
        }
        return minimalMin;
    }

    static void findFreeSet(const TemporalCSPInstance &instance, std::set<int> &set) {
       for(auto variable: instance.variables()){
           set.clear();
           set.insert(variable);
           bool recheck = true, correct = true;
           while (recheck and correct){
               recheck = false;
               for(const auto& constraint: instance.constraints){
                   if(!empty_intersection(set, constraint.variables)){
                       auto minSet = minimalMinSet(set, constraint);
                       int setSize = set.size();
                       if(minSet.empty()){
                           correct = false;
                       }else{
                           set.merge(minSet);
                           if (set.size() != setSize)
                               recheck = true;
                       }
                   }
               }
           }
           if(correct) return;
       }
       set.clear();
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