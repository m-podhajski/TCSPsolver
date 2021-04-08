#include <algorithm>
#include <stack>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include "constraint.h"


class LLClosed {
private:
    static std::set<int>
    edges(int vertex, std::unordered_map<int, std::map<TemporalConstraint *, WeakLinearOrder *>> &leastValueTuple,
          std::unordered_map<int, std::map<TemporalConstraint *, int>> &variableToConstraintPosition) {
        std::set<int> edges;
        for (const auto &vertexConstraints: leastValueTuple[vertex]) {
            int min = vertexConstraints.second->toList().size();
            for (int i = 0; i < vertexConstraints.second->toList().size(); i++) {
                if (vertexConstraints.first->variables[i] != -1 && vertexConstraints.second->toList()[i] < min) {
                    min = vertexConstraints.second->toList()[i];
                }
            }
            if (vertexConstraints.second->toList()[variableToConstraintPosition[vertex][vertexConstraints.first]] !=
                min) {
                for (const auto &vertex1: leastValueTuple) {
                    if (vertex != vertex1.first) {
                        edges.insert(vertex1.first);
                    }
                }
            } else {
                for (auto i: vertexConstraints.second->equivalenceClass[variableToConstraintPosition[vertex][vertexConstraints.first]]) {
                    if (vertexConstraints.first->variables[i] != -1) {
                        edges.insert(vertexConstraints.first->variables[i]);
                    }
                }
            }
        }
        edges.erase(vertex);
        return edges;
    }

    static void strongConnect(int vertex, int &index, std::unordered_map<int, bool> &stackMember,
                              std::unordered_map<int, int> &lowLink,
                              std::stack<int> &stack, std::unordered_map<int, int> &sscomponents,
                              std::unordered_map<int, std::map<TemporalConstraint *, WeakLinearOrder *>> &leastValueTuple,
                              std::unordered_map<int, std::map<TemporalConstraint *, int>> &variableToConstraintPosition,
                              std::vector<std::set<int>> &components) {
        sscomponents[vertex] = index;
        lowLink[vertex] = index;
        index++;
        stack.push(vertex);
        stackMember[vertex] = true;

        for (auto edgeVertex: edges(vertex, leastValueTuple, variableToConstraintPosition)) {
            if (sscomponents[edgeVertex] == -1) {
                strongConnect(edgeVertex, index, stackMember, lowLink, stack, sscomponents, leastValueTuple,
                              variableToConstraintPosition, components);
                lowLink[vertex] = std::min(lowLink[vertex], lowLink[edgeVertex]);
            } else if (stackMember[edgeVertex]) {
                lowLink[vertex] = std::min(lowLink[vertex], sscomponents[edgeVertex]);
            }
        }
        if (lowLink[vertex] == sscomponents[vertex]) {
            std::set<int> component;
            int w = -1;
            do {
                w = stack.top();
                stack.pop();
                stackMember[w] = false;
                component.insert(w);
            } while (w != vertex);
            components.push_back(component);
        }
    }

    static void stronglyConnectedComponents(
            std::unordered_map<int, std::map<TemporalConstraint *, WeakLinearOrder *>> &leastValueTuple,
            std::unordered_map<int, std::map<TemporalConstraint *, int>> &variableToConstraintPosition,
            std::vector<std::set<int>> &components) {
        int index = 0;
        std::stack<int> stack;
        std::unordered_map<int, int> sscomponents;
        for (const auto &a: leastValueTuple) {
            sscomponents[a.first] = -1;
        }
        std::unordered_map<int, bool> stackMember;
        std::unordered_map<int, int> lowLink;
        for (const auto &vertex: leastValueTuple) {
            if (sscomponents[vertex.first] == -1) {
                strongConnect(vertex.first, index, stackMember, lowLink, stack, sscomponents, leastValueTuple,
                              variableToConstraintPosition, components);
            }
        }
    }

    static bool spec(TemporalCSPInstance cspInstance, std::set<int> &newVariables) {
        std::unordered_map<int, std::map<TemporalConstraint *, WeakLinearOrder *>> leastValueTuple;
        std::unordered_map<int, std::map<TemporalConstraint *, int>> variableToConstraintPosition;
        std::unordered_map<int, std::unordered_set<TemporalConstraint *>> blocked;
        std::vector<std::set<int>> components;
        int variablesSize = cspInstance.variables().size();
        for (auto &constraint : cspInstance.constraints) {
            TemporalRelation relation = constraint.constraintLanguage.relations[constraint.relation];
            for (int i = 0; i < constraint.variables.size(); i++) {
                int variable = constraint.variables[i];
                WeakLinearOrder weakLinearOrder = *std::max_element(relation.tuples.begin(), relation.tuples.end(),
                                                                    [i](const WeakLinearOrder &a,
                                                                        const WeakLinearOrder &b) {
                                                                        std::vector<int> aTuple = a.toList();
                                                                        std::vector<int> bTuple = b.toList();
                                                                        if (aTuple[i] == bTuple[i]) {
                                                                            return a.equivalenceClassNumber <
                                                                                   b.equivalenceClassNumber;
                                                                        } else return aTuple[i] > bTuple[i];
                                                                    });
                auto *weakLinearOrder2 = new WeakLinearOrder(weakLinearOrder);
                leastValueTuple[variable][&constraint] = weakLinearOrder2;
                variableToConstraintPosition[variable][&constraint] = i;
                bool isBlocked = true;
                for (WeakLinearOrder weakLinearOrder1: relation.tuples) {
                    if (weakLinearOrder.toList()[i] == 0) {
                        isBlocked = false;
                    }
                }
                if (isBlocked) {
                    blocked[variable].insert(&constraint);
                }
            }
        }
        stronglyConnectedComponents(leastValueTuple, variableToConstraintPosition, components);
        std::vector<int> sinks;
        std::set<int> sinkSet;
        std::unordered_map<int, int> variableToComponent;
        for (int i = 0; i < components.size(); i++) {
            for (auto variable: components[i]) {
                variableToComponent[variable] = i;
            }
        }
        for (const auto &component: components) {
            if (component.size() == 1 && blocked[*component.begin()].empty()) {
                if (edges(*component.begin(), leastValueTuple, variableToConstraintPosition).empty()) {
                    sinks.push_back(*component.begin());
                    sinkSet.insert(*component.begin());
                }
            }
        }
        std::set<int> sinkY;
        while (!sinks.empty()) {
            int currentSink = sinks.front();
            sinks.pop_back();
            sinkSet.erase(currentSink);
            auto constraintsOfSink = leastValueTuple[currentSink];
            sinkY.insert(currentSink);
            leastValueTuple.erase(currentSink);
            variableToConstraintPosition.erase(currentSink);
            blocked.erase(currentSink);
            for (auto &constraint: constraintsOfSink) {

                constraint.first->orderedProjection({currentSink});

                for (int i = 0; i < constraint.first->variables.size(); i++) {
                    int variable = constraint.first->variables[i];
                    if (variable == -1) {
                        continue;
                    }
                    if (blocked[variable].find(constraint.first) != blocked[variable].end()) {
                        int min = leastValueTuple[variable][constraint.first]->toList().size();
                        for (int j = 0; j < leastValueTuple[variable][constraint.first]->toList().size(); j++) {
                            if (constraint.first->variables[j] != -1 &&
                                leastValueTuple[variable][constraint.first]->toList()[j] < min) {
                                min = leastValueTuple[variable][constraint.first]->toList()[j];
                            }
                        }
                        if (leastValueTuple[variable][constraint.first]->toList()[i] == min) {
                            blocked[variable].erase(constraint.first);
                        }
                    }
                    if (blocked[variable].empty() &&
                        edges(variable, leastValueTuple, variableToConstraintPosition).empty()) {
                        sinkSet.insert(variable);
                        sinks.push_back(variable);
                    }
                }
            }

        }
        if (sinkY.size() == variablesSize) {
            return true;
        }
        components.clear();
        stronglyConnectedComponents(leastValueTuple, variableToConstraintPosition, components);
        for (const auto &component: components) {
            bool isSinkComponent = true;
            for (auto variable: component) {
                if (!(blocked[variable].empty() &&
                      edges(variable, leastValueTuple, variableToConstraintPosition).size() == component.size() - 1)) {
                    isSinkComponent = false;
                }
            }
            if (isSinkComponent) {
                newVariables.clear();
                newVariables.insert(component.begin(), component.end());
                return true;
            }
        }
        return false;
    }

    static TemporalCSPInstance
    contractCSPInstance(const TemporalCSPInstance &instance, const std::set<int> &newVariables) {
        int variable = *newVariables.begin();
        std::vector<TemporalConstraint> constraints;
        for (const TemporalConstraint &constraint: instance.constraints) {
            std::vector<int> variables;
            for (int variable1: constraint.variables) {
                if (newVariables.find(variable1) != newVariables.end()) {
                    variables.push_back(variable);
                } else {
                    variables.push_back(variable1);
                }
            }

            TemporalConstraint constraint1(constraint.constraintLanguage, constraint.relation, variables);
            constraints.push_back(constraint1);
        }
        return TemporalCSPInstance(instance.constraintLanguage, constraints);
    }

public:
    static bool solve(TemporalCSPInstance &cspInstance) {
        std::set<int> newVariables;
        bool specResult = spec(cspInstance, newVariables);
        if (!newVariables.empty()) {
            TemporalCSPInstance cspInstancePrim = contractCSPInstance(cspInstance, newVariables);
            return solve(cspInstancePrim);
        } else {
            return specResult;
        }
    }
};

