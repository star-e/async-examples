#include <chrono>
#include <thread>
#include <unifex/sync_wait.hpp>
#include <unifex/just.hpp>
#include <unifex/task.hpp>
#include <unifex/then.hpp>
#include <unifex/typed_via.hpp>
#include <unifex/single_thread_context.hpp>
#include <unifex/any_sender_of.hpp>
#include <type_traits>
#include <iostream>

std::chrono::duration<double> time() {
    static const auto sBeginTime = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>{ std::chrono::high_resolution_clock::now() - sBeginTime };
}

class GraphicsService {
public:
    unifex::task<void> execute(std::vector<char> data) {
        co_await unifex::schedule(_context.get_scheduler());
        std::cout << "[graphics " << std::this_thread::get_id() << "] begin: " << time() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "[graphics " << std::this_thread::get_id() << "] end: " << time() << std::endl;
        co_return;
    }

    unifex::any_sender_of<> execute2(std::vector<char> data) {
        return unifex::schedule(_context.get_scheduler())
            | unifex::then([&]() {
                  std::cout << "[graphics " << std::this_thread::get_id() << "] begin: " << time() << std::endl;
                  std::this_thread::sleep_for(std::chrono::seconds(1));
                  std::cout << "[graphics " << std::this_thread::get_id() << "] processed: " << data.size() << std::endl;
                  std::cout << "[graphics " << std::this_thread::get_id() << "] end:" << time() << std::endl;
              });
    }

    unifex::single_thread_context _context;
};

int main() {
    std::cout << "[main " << std::this_thread::get_id() << "]" << std::endl;
    GraphicsService graphics;
    { // simple sync (coroutine)
        std::cout << "simple sync (coroutine)" << std::endl;
        graphics.execute({})
            | unifex::sync_wait();
    }
    { // simple sync (any_sender_of)
        std::cout << "simple sync (any_sender_of)" << std::endl;
        graphics.execute2({})
            | unifex::sync_wait();
    }

    { // then, on graphics thread
        std::cout << "then on graphics thread" << std::endl;
        graphics.execute({})
            | unifex::then([]() {
                  std::cout << "[graphics " << std::this_thread::get_id() << "] callback in graphics thread" << std::endl;
              })
            | unifex::sync_wait();
    }

    return 0;
}
