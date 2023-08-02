#include <unifex/sync_wait.hpp>
#include <unifex/just.hpp>
#include <unifex/task.hpp>
#include <unifex/then.hpp>
#include <type_traits>
#include <iostream>

int main() {
    //--------------------------------------
    // Sender is lazily executed.
    //--------------------------------------
    { // just + then
        auto sender
            = unifex::just()
            | unifex::then([]() {
                  std::cout << "execute callback" << std::endl;
                  return 42;
              }); // Q: Will "execute callback" be printed?
        std::cout << "sender created" << std::endl;
        auto res
            = std::move(sender)
            | unifex::sync_wait();
        std::cout << "Result: " << *res << std::endl;
    }

    // unifex::task is both sender and coroutine.
    {
        auto task = []() -> unifex::task<int> {
            // This lambda function is a coroutine.
            // When the lambda is called, the coroutine will suspend immediately.
            // This is the design choice of unifex::task. (lazy evaluation)
            // The coroutine will be resumed when the task is actually executed.
            std::cout << "execute makeTask" << std::endl;
            co_return 43;
        }();
        // Q: What is the type of task?
        // Q: Will "execute makeTask" be printed?
        static_assert(std::is_same_v<decltype(task), unifex::task<int>>);
        std::cout << "task created" << std::endl;
        auto res
            = std::move(task)
            | unifex::sync_wait();
        std::cout << "Result: " << *res << std::endl;
    }
    return 0;
}
