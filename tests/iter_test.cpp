#include "../src/crabiter.h"

#include <iostream>

void test_filter() {
    std::vector<int> nums = {1, 2, 3, 4};
    std::vector<int> evens = 
        crab::iter(nums)
        .filter([](int i) { return i % 2 == 0; })
        .collect();

    for (int i : nums) {
        std::cout << i << " ";
    }

    std::cout << "\n";

    for (int i : evens) {
        std::cout << i << " ";
    }

    std::cout << "\n";
}

void test_map() {
    std::vector<int> nums = {1, 2, 3, 4};
    
    for (int i : nums) {
        std::cout << i << " ";
    }

    std::cout << "\n";

    nums = crab::iter(nums)
        .map([](int i) { return i * 2; })
        .collect();

    for (int i : nums) {
        std::cout << i << " ";
    }
}

int main(void) {
    test_filter();
    test_map();
}