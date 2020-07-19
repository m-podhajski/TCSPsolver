#include "tests/tests.cpp"
int main() {
    Tests::test_ll_closed();
    Tests::test_mx_closed();
    Tests::test_mi_closed();
    Tests::test_min_closed();
    Tests::test_backtracking();
    Tests::test_kl_minimality();
    return 0;
}
