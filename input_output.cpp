#include <iostream>
#include <fstream>
#include <unordered_map>
#include "constraint.h"

static TemporalCSPInstance loadFromFile(std::string filename)
{
    std::ifstream file(filename);
    int numberOfRelations;
    file >> numberOfRelations;
    std::vector<TemporalRelation> relations;
    for (int i = 0; i < numberOfRelations; i++)
    {
        int arity;
        file >> arity;
        int numberOfWLOs;
        file >> numberOfWLOs;
        std::set<WeakLinearOrder> weakLinearOrders;
        for (int j = 0; j < numberOfWLOs; j++)
        {
            std::vector<int> v(arity);
            for (int k = 0; k < arity; k++)
            {
                file >> v[k];
            }
            WeakLinearOrder weakLinearOrder(arity, v);
            weakLinearOrders.insert(weakLinearOrder);
        }
        TemporalRelation temporalRelation(arity, weakLinearOrders);
        relations.push_back(temporalRelation);
    }
    TemporalConstraintLanguage language(relations);
    std::vector<TemporalConstraint> constraints;
    int numberOfConstraints;
    file >> numberOfConstraints;
    for (int i = 0; i < numberOfConstraints; i++)
    {
        int relationNumber;
        file >> relationNumber;
        std::vector<int> variables(language.relations[relationNumber].arity);
        for (int j = 0; j < language.relations[relationNumber].arity; j++)
        {
            file >> variables[j];
        }
        TemporalConstraint constraint(language, relationNumber, variables);
        constraints.push_back(constraint);
    }
    file.close();
    TemporalCSPInstance instance(language, constraints);
    return instance;
}

static void printRelation(int number, TemporalRelation relation)
{
    std::cout << "R_" << number << " = {(";
    for (int i = 1; i < relation.arity; i++)
    {
        std::cout << "x_" << i << ", ";
    }
    std::cout << "x_" << relation.arity << "): ";
    int order_cnt = 0;
    for (auto order : relation.tuples)
    {
        std::unordered_map<int, std::vector<int>> orders;
        auto l = order.toList();
        int max = 0;
        for (int i = 1; i <= relation.arity; i++)
        {
            orders[l[i - 1]].push_back(i);
            if (l[i - 1] > max)
            {
                max = l[i - 1];
            }
        }
        std::cout << "(";
        for (int i = max; i >= 0; i--)
        {
            for (int j = 0; j < orders[i].size(); j++)
            {
                std::cout << "x_" << orders[i][j];
                if (j != orders[i].size() - 1)
                {
                    std::cout << " = ";
                }
            }
            if (i != 0)
            {
                std::cout << " > ";
            }
        }
        std::cout << ")";
        if (order_cnt != relation.tuples.size() - 1)
        {
            std::cout << " or ";
        }
        order_cnt++;
    }
    std::cout << "}" << std::endl;
}

static void printConstraint(TemporalConstraint constraint)
{
    std::cout << "R_" << constraint.relation << "(";
    for (int i = 0; i < constraint.variables.size(); i++)
    {
        std::cout << "x_" << constraint.variables[i];
        if (i != constraint.variables.size() - 1)
        {
            std::cout << ", ";
        }
    }
    std::cout << ")" << std::endl;
}

static void printInstance(TemporalCSPInstance instance)
{
    std::cout << "Relations:" << std::endl;
    int i = 0;
    for (auto relation : instance.constraintLanguage.relations)
    {
        printRelation(i++, relation);
    }
    std::cout << "Constraints:" << std::endl;

    for (auto constraint : instance.constraints)
    {
        printConstraint(constraint);
    }
}