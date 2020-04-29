#include <algorithm>
#include <stack>
#include "constraint.h"

class LLClosed {
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

    class Graph {
    private:
        std::vector<std::set<int>> graph;
        std::set<std::set<int>> components;
        std::vector<TemporalConstraint> constraints;


        void strongConnect(int vertex, int &index, std::vector<bool> &stackMember, std::vector<int> &lowLink,
                           std::stack<int> &stack, std::vector<int> &sscomponents) {
            sscomponents[vertex] = index;
            lowLink[vertex] = index;
            index++;
            stack.push(vertex);
            stackMember[vertex] = true;
            for (int edgeVertex: graph[vertex]) {
                if (sscomponents[edgeVertex] == -1) {
                    strongConnect(edgeVertex, index, stackMember, lowLink, stack, sscomponents);
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
                components.insert(component);
            }
        }


        //Tarjan's algorithm
        void stronglyConnectedComponents() {
            int index = 0;
            std::stack<int> stack;
            std::vector<int> sscomponents(graph.size(), -1);
            std::vector<bool> stackMember(graph.size());
            std::vector<int> lowLink(graph.size());
            for (int vertex = 0; vertex < graph.size(); vertex++) {
                if (sscomponents[vertex] == -1) {
                    strongConnect(vertex, index, stackMember, lowLink, stack, sscomponents);
                }
            }
        }

    public:
        bool containsSinkComponent() {
            for (const auto &component: components) {
                bool isBlocked = false;
                for (auto constraint: constraints) {
                    if (constraint.isBlocked(*component.begin())) {
                        isBlocked = true;
                        break;
                    }
                }
                if (!isBlocked) {
                    return true;
                }
            }
            return false;
        }

        std::set<int> returnSinkComponent() {
            std::set<int> sinkComponent;
            for (const auto &component: components) {
                bool isBlocked = false;
                for (auto constraint: constraints) {
                    if (constraint.isBlocked(*component.begin())) {
                        isBlocked = true;
                        break;
                    }
                }
                if (!isBlocked) {
                    return component;
                }
            }
            return sinkComponent;
        }

        bool containsSink() {
            for (const auto &component: components) {
                if (component.size() == 1) {
                    bool isBlocked = false;
                    for (auto constraint: constraints) {
                        if (constraint.isBlocked(*component.begin())) {
                            isBlocked = true;
                            break;
                        }
                    }
                    if (!isBlocked) {
                        return true;
                    }
                }
            }
            return false;
        }

        int returnSink() {
            for (const auto &component: components) {
                if (component.size() == 1) {
                    bool isBlocked = false;
                    for (auto constraint: constraints) {
                        if (constraint.isBlocked(*component.begin())) {
                            isBlocked = true;
                            break;
                        }
                    }
                    if (!isBlocked) {
                        return *component.begin();
                    }
                }
            }
            return -1;
        }

        explicit Graph(TemporalCSPInstance &instance) {
            int size = *instance.variables().rbegin() + 1;
            graph.resize(size);
            for (TemporalConstraint constraint: instance.constraints) {
                for (int i = 0; i < graph.size(); i++) {
                    std::set<int> intersect(instance.variables());
                    for (auto s: constraint.minimalVariables()) {
                        if (s.find(i) != s.end()) {
                            std::set<int> intersectPrim;
                            std::set_intersection(s.begin(), s.end(), intersect.begin(), intersect.end(),
                                                  std::inserter(intersectPrim, intersectPrim.begin()));
                            intersect = intersectPrim;
                        }
                    }
                    graph[i].merge(intersect);
                    graph[i].erase(i);
                }
            }
            stronglyConnectedComponents();
            constraints = instance.constraints;
        }

        void reconstructGraph(int vertex) {
            std::set<int> deleteComponent;
            deleteComponent.insert(vertex);
            components.erase(deleteComponent);
        }
    };

    static Graph constructGraph(TemporalCSPInstance &instance) {
        return Graph(instance);
    }


    static bool spec(TemporalCSPInstance &cspInstance, std::set<int> &newVariables) {
        Graph graph = constructGraph(cspInstance);
        std::set<int> Y;
        std::set<int> X(cspInstance.variables());
        TemporalCSPInstance cspInstancePrim = cspInstance;
        while (graph.containsSink()) {
            int s = graph.returnSink();
            Y.insert(s);
            graph.reconstructGraph( s);
        }
        if (X == Y) {
            return true;
        } else if (graph.containsSinkComponent()) {
            newVariables.merge(graph.returnSinkComponent());
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
    static bool checkClosure(const TemporalConstraintLanguage &constraint) {
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

