#include <unifex/sync_wait.hpp>
#include <unifex/just.hpp>
#include <type_traits>
#include <iostream>

void test_combined() {
    auto res = unifex::sync_wait(unifex::just(42));
    static_assert(std::is_same_v<decltype(res), std::optional<int>>);
    std::cout << "Result: " << *res << std::endl;
}

void test_separated() {
    auto task = unifex::just(std::make_unique<int>(43));
    auto res = unifex::sync_wait(std::move(task)); // Q: Can std::move be omitted? Why?
    // Q: What type is res?
    std::cout << "Result: " << **res << std::endl;
}

int main() {
    test_combined();
    test_separated();
    return 0;
}
