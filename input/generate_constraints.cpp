#include <iostream>
#include <chrono>
#include <random>

int main()
{
    std::random_device rd;
    std::mt19937::result_type seed = rd() ^ ((std::mt19937::result_type)
                                                 std::chrono::duration_cast<std::chrono::seconds>(
                                                     std::chrono::system_clock::now().time_since_epoch())
                                                     .count() +
                                             (std::mt19937::result_type)
                                                 std::chrono::duration_cast<std::chrono::microseconds>(
                                                     std::chrono::high_resolution_clock::now().time_since_epoch())
                                                     .count());

    std::mt19937 gen(seed);
    std::cout << "Input number of variables: ";
    int variables;
    std::cin >> variables;
    std::cout << "Input arity: ";
    int arity;
    std::cin >> arity;
    std::cout << "Input number of constraints: ";
    int constraints;
    std::cin >> constraints;
    std::cout << "Input relation number: ";
    int relation_number;
    std::cin >> relation_number;
    std::cout << "Input:" << std::endl;
    std::cout << constraints << std::endl;
    for (int i = 0; i < constraints; i++)
    {
        std::cout << relation_number << " ";
        for (int j = 0; j < arity; j++)
        {
            std::mt19937::result_type n;
        while( ( n = gen() ) > std::mt19937::max() -
                                    ( std::mt19937::max() - variables + 1 )%variables )
        {  }

        std::cout << n % variables << " ";
        }
        std::cout << std::endl;
    }
    return 0;
}