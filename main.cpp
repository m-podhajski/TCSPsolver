#include <iostream>
#include "tests/tests.cpp"
#include "./input_output.cpp"
#include "./check_closure.cpp"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Format: ./TCSPsolver inputfile" << std::endl;
        return 1;
    }
    auto instance = loadFromFile(argv[1]);
    //printInstance(instance);
    std::cout << CheckClosure::mxClosure(instance.constraintLanguage) << std::endl;
    std::cout << "\nSolving:" << std::endl;
    if (Backtracking::solve(instance)) {
        std::cout << "Instance is satisfiable" << std::endl;
    } else {
        std::cout << "Instance is not satisfiable" << std::endl;
    }
    return 0;
    if (CheckClosure::miClosure(instance.constraintLanguage)) {
        std::cout << "Language is mi closed, using mi algorithm" << std::endl;
        if (MiClosed::solve(instance)) {
            std::cout << "Instance is satisfiable" << std::endl;
        } else {
            std::cout << "Instance is not satisfiable" << std::endl;
        }
    } else if (CheckClosure::minClosure(instance.constraintLanguage)) {
        std::cout << "Language is min closed, using min algorithm" << std::endl;
        if (MinClosed::solve(instance)) {
            std::cout << "Instance is satisfiable" << std::endl;
        } else {
            std::cout << "Instance is not satisfiable" << std::endl;
        }
    } else if (CheckClosure::mxClosure(instance.constraintLanguage)) {
        std::cout << "Language is mx closed, using mx algorithm" << std::endl;
        if (MxClosed::solve(instance)) {
            std::cout << "Instance is satisfiable" << std::endl;
        } else {
            std::cout << "Instance is not satisfiable" << std::endl;
        }
    } else if (CheckClosure::llClosure(instance.constraintLanguage)) {
        std::cout << "Language is ll closed, using ll algorithm" << std::endl;
        if (LLClosed::solve(instance)) {
            std::cout << "Instance is satisfiable" << std::endl;
        } else {
            std::cout << "Instance is not satisfiable" << std::endl;
        }
    } else {
        std::cout << "Using (2, 3) minimality algorithm + backtracking" << std::endl;
        TemporalCSPInstance klMinimalInstance = KLMinimality::solve(instance, 2, 3);
        if (Backtracking::solve(klMinimalInstance)) {
            std::cout << "Instance is satisfiable" << std::endl;
        } else {
            std::cout << "Instance is not satisfiable" << std::endl;
        }
    }
    return 0;
}
