#include <unifex/sync_wait.hpp>
#include <unifex/just.hpp>
#include <unifex/then.hpp>
#include <type_traits>
#include <iostream>

int main() {
    { // function call
        auto res
            = unifex::sync_wait(
                unifex::just(42));
        std::cout << "Result: " << *res << std::endl;
    }
    { // pipe operator
        auto res
            = unifex::just(43)
            | unifex::sync_wait();
        std::cout << "Result: " << *res << std::endl;
    }
    { // piped just + then
        auto res
            = unifex::just()
            | unifex::then([]() { return 44; })
            | unifex::sync_wait();
        std::cout << "Result: " << *res << std::endl;
    }
    {
        // Q: How can we rewrite just + then using function call?
    }
    return 0;
}
