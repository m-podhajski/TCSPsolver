#ifndef TCSPSOLVER_CONSTRAINT_H
#define TCSPSOLVER_CONSTRAINT_H

#include <list>
#include <set>
#include <utility>
#include <vector>

class WeakLinearOrder {
private:
    void createOrderGraph(const std::vector<int> &tuple) {
        orderGraph.resize(arity);
        for (int i = 0; i < arity; i++) {
            for (int j = 0; j < arity; j++) {
                if (tuple[i] <= tuple[j]) {
                    orderGraph[i].insert(j);
                }
            }
        }
    }

public:
    const int arity;

    //krawedz oznacza <=, brak krawedzi !<= czyli >
    std::vector<std::set<int>> orderGraph;

    WeakLinearOrder(int arity, const std::vector<int> &tuple) : arity(arity) {
        createOrderGraph(tuple);
    }

    WeakLinearOrder(int arity, std::vector<std::set<int>> orderGraph) : arity(arity),
                                                                        orderGraph(std::move(orderGraph)) {}

    std::vector<int> toList() {
        std::vector<int> result;
        result.reserve(arity);
        for(int i=0;i<arity;i++){
            result.push_back(arity-orderGraph[i].size());
        }
        return result;
    }

    std::set<int> minimalEntries() const {
        std::set<int> minimals;
        for (int vertex = 0; vertex < arity; vertex++) {
            if (orderGraph[vertex].size() == arity) {
                minimals.insert(vertex);
            }
        }
        return minimals;
    }
};

bool operator<(const WeakLinearOrder &wko1, const WeakLinearOrder &wko2) {
    if (wko1.arity != wko2.arity)
        return wko1.arity < wko2.arity;
    for (int i = 0; i < wko1.arity; i++) {
        if (wko1.orderGraph[i] != wko2.orderGraph[i])
            return wko1.orderGraph[i] < wko2.orderGraph[i];
    }
    return false;
}


class TemporalRelation {
public:
    int arity;
    std::set<WeakLinearOrder> tuples;

    TemporalRelation(int arity, std::set<WeakLinearOrder> tuples) : arity(arity), tuples(std::move(tuples)) {}
};

class TemporalConstraintLanguage {
public:
    std::vector<TemporalRelation> relations;

    explicit TemporalConstraintLanguage(std::vector<TemporalRelation> relations) : relations(std::move(relations)) {}
};

class TemporalConstraint {
public:
    TemporalConstraintLanguage constraintLanguage;
    int relation;
    std::vector<int> variables;

    TemporalConstraint(TemporalConstraintLanguage constraintLanguage, int relation,
                       std::vector<int> variables) : constraintLanguage(std::move(constraintLanguage)),
                                                     relation(relation),
                                                     variables(std::move(variables)) {}

    void orderedProjection(std::set<int> set) {
        for (int &variable : variables) {
            if (set.find(variable) != set.end()) {
                variable = -1;
            }
        }
        int variablesSize = 0;
        for (auto variable: variables) {
            if (variable != -1) variablesSize++;
        }
        for (auto tuple = constraintLanguage.relations[relation].tuples.begin();
             tuple != constraintLanguage.relations[relation].tuples.end();) {
            for (int i = 0; i < variables.size(); i++) {
                if (variables[i] == -1 && tuple->orderGraph[i].size() < variablesSize) {
                    constraintLanguage.relations[relation].tuples.erase(tuple);
                }
            }
            tuple++;
        }
    }


    //for every tuple in relation set of variables that are on minimal entry
    std::vector<std::set<int>> minimalVariables() const {
        std::vector<std::set<int>> minimals;
        for (const auto &tuple: constraintLanguage.relations[relation].tuples) {
            std::set<int> minimalVariables;
            for (auto minimalEntry: tuple.minimalEntries()) {
                if (variables[minimalEntry] != -1)
                    minimalVariables.insert(variables[minimalEntry]);
            }
            minimals.push_back(minimalVariables);
        }
        return minimals;
    }

    bool isBlocked(int variable) const {
        for (auto minimals: minimalVariables()) {
            if (minimals.find(variable) != minimals.end())
                return false;
        }
        return true;
    }
};

class TemporalCSPInstance {

public:
    TemporalConstraintLanguage constraintLanguage;
    std::vector<TemporalConstraint> constraints;

    TemporalCSPInstance(TemporalConstraintLanguage constraintLanguage,
                        std::vector<TemporalConstraint> constraints) : constraintLanguage(std::move(
            constraintLanguage)), constraints(std::move(constraints)) {

    }

    std::set<int> variables() const {
        std::set<int> variables;
        for (auto constraint: this->constraints) {
            variables.insert(constraint.variables.begin(), constraint.variables.end());
        }
        variables.erase(-1);
        return variables;
    }


};

#endif //TCSPSOLVER_CONSTRAINT_H
