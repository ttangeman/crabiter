#include "../src/crabiter.h"

#include <iostream>
#include <vector>

int main(void) {
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