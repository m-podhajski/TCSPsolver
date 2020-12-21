#ifndef TCSPSOLVER_CONSTRAINT_H
#define TCSPSOLVER_CONSTRAINT_H

#include <list>
#include <set>
#include <utility>
#include <vector>
#include <unordered_map>

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
    int arity;

    //krawedz oznacza <=, brak krawedzi !<= czyli >
    std::vector<std::set<int>> orderGraph;
    std::vector<std::set<int>> equivalenceClass;
    std::vector<int> list;
    int equivalenceClassNumber{};

    WeakLinearOrder(int arity, const std::vector<int> &tuple) : arity(arity) {
        createOrderGraph(tuple);
        equivalenceClass.resize(arity);
        for (int i = 0; i < arity; i++) {
            for (int j = 0; j < arity; j++) {
                if (tuple[i] == tuple[j] && i != j) {
                    equivalenceClass[i].insert(j);
                }
            }
        }
        std::set<int> classes;
        for (int i = 0; i < arity; i++) {
            classes.insert(tuple[i]);
        }
        equivalenceClassNumber = classes.size();
        std::set<int> sortedTuple(tuple.begin(), tuple.end());
        int i = 0;
        std::unordered_map<int, int> normalizedTuple;
        for (auto variable: sortedTuple) {
            normalizedTuple[variable] = i;
            i++;
        }
        list.resize(arity);
        for (i = 0; i < arity; i++) {
            int newValue = normalizedTuple[tuple[i]];
            list[i] = newValue;
        }
    }

    WeakLinearOrder(int arity, std::vector<std::set<int>> orderGraph) : arity(arity),
                                                                        orderGraph(std::move(orderGraph)) {
    }

    std::vector<int> toList() const {
        return list;
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

    bool operator<(const TemporalConstraint &p) const {
        if (this->relation != p.relation) {
            return this->relation < p.relation;
        }
        for (int i = 0; i < variables.size(); i++) {
            if (this->variables[i] != p.variables[i]) {
                return this->variables[i] < p.variables[i];
            }
        }
        return false;
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
