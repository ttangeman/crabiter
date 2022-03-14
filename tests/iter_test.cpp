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

    std::cout << "\n";
}

void test_filter_and_map() {
    std::vector<int> nums = {1, 2, 3, 4};
    
    for (int i : nums) {
        std::cout << i << " ";
    }

    std::cout << "\n";

    crab::iter(nums)
    .filter([](int i) { 
        return i % 2 == 0; 
    })
    .map([](int i) { 
        return i * 2; 
    })
    .for_each([](int i) {
        std::cout << i << " ";
    });

    std::cout << "\n";
}

void test_nth() {
    std::vector<int> nums = {1, 2, 3, 4};
    
    auto iter = crab::iter(nums);
    std::cout << iter.nth(0).value() << " ";
    std::cout << iter.nth(0).value() << " ";
    std::cout << iter.nth(0).value() << " ";
    std::cout << iter.nth(0).value() << "\n";
}

int main(void) {
    test_filter();
    test_map();
    test_filter_and_map();
    test_nth();
}