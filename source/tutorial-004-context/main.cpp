#include "unifex/any_sender_of.hpp"
#include <chrono>
#include <sstream>
#include <thread>
#include <unifex/sync_wait.hpp>
#include <unifex/just.hpp>
#include <unifex/task.hpp>
#include <unifex/then.hpp>
#include <unifex/single_thread_context.hpp>
#include <unifex/static_thread_pool.hpp>
#include <unifex/when_all.hpp>
#include <unifex/sequence.hpp>
#include <unifex/let_value.hpp>
#include <unifex/typed_via.hpp>
#include <type_traits>
#include <iostream>

std::chrono::duration<double> time() {
    static const auto sBeginTime = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>{ std::chrono::high_resolution_clock::now() - sBeginTime };
}

class TaskPool {
public:
    explicit TaskPool(uint32_t threadCount)
        : _pool(threadCount) {}

    unifex::task<uint32_t> process(const std::vector<uint32_t>& data, uint32_t offset, uint32_t num) {
        co_await unifex::schedule(_pool.get_scheduler());
        std::ostringstream oss;
        oss << "[worker " << std::this_thread::get_id() << "] start" << std::endl;
        std::cout << oss.str();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        uint32_t result = 0;
        for (uint32_t i = offset; i < offset + num; ++i) {
            result += data[i];
        }
        co_return result;
    }

    unifex::static_thread_pool _pool;
};

class GraphicsService {
public:
    unifex::task<uint32_t> sequence(std::vector<uint32_t> data) {
        // switch context to graphics
        co_await unifex::schedule(_context.get_scheduler());

        std::cout << "[graphics " << std::this_thread::get_id() << "] begin: " << time() << std::endl;

        // switch context to worker
        uint32_t first = data.size() / 2;
        uint32_t second = data.size() - first;

        // sequence
        auto res1 = co_await _taskPool.process(data, 0, first);
        auto res2 = co_await _taskPool.process(data, first, second);

        // still using worker context
        std::cout << "[worker " << std::this_thread::get_id() << "] finished" << std::endl;

        // switch context to graphics
        co_await unifex::schedule(_context.get_scheduler());
        std::cout << "[graphics " << std::this_thread::get_id() << "] end: " << time() << std::endl;

        // return result
        co_return res1 + res2;
    }

    unifex::task<uint32_t> parallel(std::vector<uint32_t> data) {
        // switch context to graphics
        co_await unifex::schedule(_context.get_scheduler());

        std::cout << "[graphics " << std::this_thread::get_id() << "] begin: " << time() << std::endl;

        // switch context to worker
        uint32_t first = data.size() / 2;
        uint32_t second = data.size() - first;
        auto [res1, res2] = co_await unifex::when_all(
            _taskPool.process(data, 0, first),
            _taskPool.process(data, first, second));

        // still using worker context
        std::cout << "[worker " << std::this_thread::get_id() << "] finished" << std::endl;

        // switch context to graphics
        co_await unifex::schedule(_context.get_scheduler());
        std::cout << "[graphics " << std::this_thread::get_id() << "] end: " << time() << std::endl;

        // return result
        co_return std::get<0>(std::get<0>(res1)) + std::get<0>(std::get<0>(res2));
    }

    unifex::any_sender_of<uint32_t> parallel2(std::vector<uint32_t> data) {
        return unifex::just(this, std::vector<uint32_t>{ std::move(data) })
            | unifex::typed_via(_context.get_scheduler())
            | unifex::let_value(
                [](GraphicsService* service, const std::vector<uint32_t>& data) {
                    uint32_t first = data.size() / 2;
                    uint32_t second = data.size() - first;
                    std::cout << "[graphics " << std::this_thread::get_id() << "] begin: " << time() << std::endl;
                    return unifex::when_all(
                        service->_taskPool.process(data, 0, first),
                        service->_taskPool.process(data, first, second));
                })
            | unifex::typed_via(_context.get_scheduler())
            | unifex::then([](const auto& res1, const auto& res2) {
                  std::cout << "[graphics " << std::this_thread::get_id() << "] end: " << time() << std::endl;
                  return std::get<0>(std::get<0>(res1)) + std::get<0>(std::get<0>(res2));
              });
    }

    unifex::single_thread_context _context;
    TaskPool _taskPool{ 4U };
};

int main() {
    GraphicsService graphics;
    {
        auto result
            = graphics.sequence(std::vector<uint32_t>(200, 1))
            | unifex::sync_wait();

        std::cout << "[main " << std::this_thread::get_id() << "] result = " << *result << std::endl;
    }
    {
        // coroutine version
        auto result
            = graphics.parallel(std::vector<uint32_t>(200, 1))
            | unifex::sync_wait();

        std::cout << "[main " << std::this_thread::get_id() << "] result = " << *result << std::endl;
    }
    {
        // any_sender_of version
        auto result
            = graphics.parallel2(std::vector<uint32_t>(200, 1))
            | unifex::sync_wait();

        std::cout << "[main " << std::this_thread::get_id() << "] result = " << *result << std::endl;
    }

    return 0;
}
