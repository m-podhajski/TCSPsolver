#include <iostream>
#include <vector>
#include <regex>
#include "../input_output.cpp"

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

static bool
satisfiesConstraints(std::vector<int> list, std::vector<std::vector<std::pair<std::pair<int, int>, int>>> constraints) {
    for (auto ors : constraints) {
        bool isSatisfied = true;
        for (auto cons : ors) {
            auto op = cons.second;
            if (op == 0 && !(list[cons.first.first - 1] <= list[cons.first.second - 1])) {
                isSatisfied = false;
                break;
            }
            if (op == 1 && !(list[cons.first.first - 1] == list[cons.first.second - 1])) {
                isSatisfied = false;
                break;
            }
            if (op == 2 && !(list[cons.first.first - 1] < list[cons.first.second - 1])) {
                isSatisfied = false;
                break;
            }
        }
        if (isSatisfied) {
            return true;
        }
    }
    return false;
}

static TemporalRelation
generateRelation(int size, std::vector<std::vector<std::pair<std::pair<int, int>, int>>> constraints) {
    std::set<WeakLinearOrder> weakLinearOrders;
    std::vector<int> numbers(size);
    if (satisfiesConstraints(numbers, constraints)) {
        weakLinearOrders.insert(WeakLinearOrder(size, numbers));
    }
    while (_next(numbers, size)) {
        if (satisfiesConstraints(numbers, constraints)) {
            weakLinearOrders.insert(WeakLinearOrder(size, numbers));
        }
    }
    TemporalRelation fullRelation(size, weakLinearOrders);
    return fullRelation;
}

static std::vector<std::vector<std::pair<std::pair<int, int>, int>>> parseConstraint(std::string constraint) {
    std::regex regex{R"(or)"};
    std::sregex_token_iterator it{constraint.begin(), constraint.end(), regex, -1};
    std::vector<std::string> v1{it, {}};
    std::vector<std::vector<std::pair<std::pair<int, int>, int>>> constraints;
    for (auto s1 : v1) {
        std::vector<std::pair<std::pair<int, int>, int>> currentConstraint;
        std::regex regex1{R"(and)"};
        std::sregex_token_iterator it1{s1.begin(), s1.end(), regex1, -1};
        std::vector<std::string> v2{it1, {}};
        for (auto s2 : v2) {
            s2.erase(std::remove_if(s2.begin(), s2.end(), [](char ch) { return ch == '(' || ch == ')' || ch == ' '; }),
                     s2.end());

            if (regex_search(s2, std::regex{R"(<=)"})) {
                std::regex regex1{R"(<=)"};
                std::sregex_token_iterator it3{s2.begin(), s2.end(), regex1, -1};
                std::vector<std::string> v3{it3, {}};
                currentConstraint.push_back(std::make_pair(std::make_pair(std::stoi(v3[0]), std::stoi(v3[1])), 0));
            } else if (regex_search(s2, std::regex{R"(=)"})) {
                std::regex regex1{R"(=)"};
                std::sregex_token_iterator it3{s2.begin(), s2.end(), regex1, -1};
                std::vector<std::string> v3{it3, {}};
                currentConstraint.push_back(std::make_pair(std::make_pair(std::stoi(v3[0]), std::stoi(v3[1])), 1));
            } else if (regex_search(s2, std::regex{R"(<)"})) {
                std::regex regex1{R"(<)"};
                std::sregex_token_iterator it3{s2.begin(), s2.end(), regex1, -1};
                std::vector<std::string> v3{it3, {}};
                currentConstraint.push_back(std::make_pair(std::make_pair(std::stoi(v3[0]), std::stoi(v3[1])), 2));
            }
        }
        constraints.push_back(currentConstraint);
    }
    return constraints;
}

int main() {
    std::cout << "Generating relation:" << std::endl;
    int arity;
    std::cout << "Arity: ";
    std::cin >> arity;
    std::cout << "Input relation:" << std::endl;
    std::string constraint;
    std::getline(std::cin, constraint);
    std::getline(std::cin, constraint);
    auto constraints = parseConstraint(constraint);
    auto relation = generateRelation(arity, constraints);
    printRelation(0, relation);
    std::cout << "Input format:" << std::endl;
    std::cout << relation.arity << std::endl
              << relation.tuples.size() << std::endl;
    for (auto wlo : relation.tuples) {
        for (auto a : wlo.toList()) {
            std::cout << a << " ";
        }
        std::cout << std::endl;
    }
}