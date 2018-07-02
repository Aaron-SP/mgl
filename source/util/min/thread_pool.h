/* Copyright [2013-2018] [Aaron Springstroh, Minimal Graphics Library]

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#ifndef __THREAD_POOL__
#define __THREAD_POOL__

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <random>
#include <stdexcept>
#include <thread>
#include <vector>

namespace min
{

class work_item
{
  private:
    std::function<void(std::mt19937 &gen, const size_t)> _f;
    size_t _begin;
    size_t _length;

  public:
    work_item(const std::function<void(std::mt19937 &gen, const size_t)> &f, const size_t begin, const size_t length)
        : _f(f), _begin(begin), _length(length) {}

    inline void work(std::mt19937 &gen) const
    {
        const size_t end = _begin + _length;
        for (size_t i = _begin; i < end; i++)
        {
            // Do the work for this item
            _f(gen, i);
        }
    }
};

class thread
{
  private:
    std::vector<work_item> _work;
    std::atomic<bool> _sleep;
    std::atomic<bool> _state;
    std::thread _thread;
    std::mt19937 _gen;

  public:
    thread()
        : _sleep(true), _state(false),
          _gen(std::chrono::high_resolution_clock::now().time_since_epoch().count()) {}

    inline std::thread &get_thread()
    {
        return _thread;
    }
    inline void join()
    {
        _thread.join();
    }
    inline std::mt19937 &rand()
    {
        return _gen;
    }
    inline void seed(const size_t seed)
    {
        _gen.seed(seed);
    }
    inline void set_sleep(const bool flag)
    {
        _sleep = flag;
    }
    inline void set_state(const bool flag)
    {
        _state = flag;
    }
    inline bool sleep() const
    {
        return _sleep;
    }
    inline bool state() const
    {
        return _state;
    }
    inline std::vector<work_item> &work()
    {
        return _work;
    }
};

class thread_pool
{
  private:
    unsigned _thread_count;
    std::vector<thread> _threads;
    std::mutex _sleep_lock;
    std::condition_variable _more_data;
    std::atomic<bool> _die;
    std::atomic<bool> _turbo;
    std::mt19937 _gen;

    inline void notify()
    {
        // If we are sleeping, wait for all threads to sleep before killing queue
        if (!_turbo)
        {
            // Wake up idle threads
            _more_data.notify_all();
        }
    }
    inline void wait_sleep()
    {
        // Wait for all workers to quit
        while (!_turbo)
        {
            // Count threads asleep
            size_t count = 0;
            for (size_t i = 0; i < _thread_count - 1; i++)
            {
                // Make sure all threads are asleep
                std::unique_lock<std::mutex> lock(_sleep_lock);

                // See if worker is finished
                if (_threads[i].sleep() == false)
                {
                    count++;
                }
            }

            // If all workers are finished
            if (count == _thread_count - 1)
            {
                break;
            }
        }
    }
    inline void wait_done() const
    {
        // Wait for all workers to quit
        while (true)
        {
            // Count threads asleep
            size_t count = 0;
            for (size_t i = 0; i < _thread_count - 1; i++)
            {
                // See if worker is finished
                if (_threads[i].state() == false)
                {
                    count++;
                }
            }

            // If all workers are finished
            if (count == _thread_count - 1)
            {
                break;
            }
        }
    }
    inline void wait()
    {
        if (!_turbo)
        {
            wait_sleep();
        }
        else
        {
            wait_done();
        }
    }
    inline void work(const size_t index)
    {
        while (true)
        {
            // Sleep on condition
            if (!_turbo)
            {
                // Acquire mutex to sleep on condition
                std::unique_lock<std::mutex> lock(_sleep_lock);

                // ATOMIC: Signal Sleeping
                _threads[index].set_sleep(false);

                // Wait on more data
                _more_data.wait(lock, [this, index]() { return (_threads[index].state() || _die) || _turbo; });
            }

            // Do work
            if (_threads[index].state())
            {
                // Get access to work
                std::vector<work_item> &items = _threads[index].work();
                const size_t size = items.size();

                // Do all work in this queue
                for (size_t i = 0; i < size; i++)
                {
                    items[i].work(_threads[index].rand());
                }

                // Clear the queue
                items.clear();

                // ATOMIC: Signal Finished
                _threads[index].set_state(false);
            }
            else if (_die)
            {
                // Kill thread
                break;
            }
        }
    }

  public:
    thread_pool() : _thread_count(std::thread::hardware_concurrency()),
                    _threads(_thread_count - 1), _die(false), _turbo(false),
                    _gen(std::chrono::high_resolution_clock::now().time_since_epoch().count())
    {
        // Error out if can't determine core count
        if (_thread_count < 1)
        {
            throw std::runtime_error("thread_pool: can't determine number of CPU cores");
        }

        // Boot all threads
        for (size_t i = 0; i < _thread_count - 1; i++)
        {
            // Boot the thread
            _threads[i].get_thread() = std::thread(&thread_pool::work, this, i);
        }
    }
    ~thread_pool()
    {
        // Kill all threads in pool
        kill();

        // Join all threads
        for (size_t i = 0; i < _thread_count - 1; i++)
        {
            _threads[i].join();
        }
    }
    inline void kill()
    {
        // Wait for threads to finish work
        wait();

        // Signal dead queue
        _die = true;

        // Notify threads
        notify();
    }
    inline void seed(const size_t seed)
    {
        for (size_t i = 0; i < _thread_count - 1; i++)
        {
            // Seed the generator
            _threads[i].seed(seed);
        }

        _gen.seed(seed);
    }
    inline void sleep()
    {
        // ATOMIC: Set state of all workers to 'sleep'
        for (size_t i = 0; i < _thread_count - 1; i++)
        {
            // Signal finished
            _threads[i].set_sleep(true);
        }

        // Put all threads to sleep
        _turbo = false;
    }
    inline void wake()
    {
        // Wait for all workers to sleep
        wait_sleep();

        // Turn off sleeping
        _turbo = true;

        // Wake up idle threads
        _more_data.notify_all();
    }
    inline void run(const std::function<void(std::mt19937 &gen, const size_t)> &f, const size_t start, const size_t stop)
    {
        // Wait for all workers to sleep
        wait();

        // Load queue with work
        const size_t length = (stop - start) / _thread_count;

        size_t begin = start;
        for (size_t i = 0; i < _thread_count - 1; i++)
        {
            // Create work for thread
            _threads[i].work().emplace_back(f, begin, length);

            // Increment next work item
            begin += length;
        }

        // ATOMIC: Set state of all workers to 'run'
        for (size_t i = 0; i < _thread_count - 1; i++)
        {
            // Signal that there is work to do
            _threads[i].set_sleep(true);
            _threads[i].set_state(true);
        }

        // Notify threads
        notify();

        // Boot the residual work on this thread
        const size_t remain = stop - begin;
        work_item item(f, begin, remain);
        item.work(_gen);

        // Wait for all workers to finish work
        wait_done();
    }
};
}

#endif
