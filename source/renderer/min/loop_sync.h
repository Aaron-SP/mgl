/* Copyright [2013-2016] [Aaron Springstroh, Minimal Graphics Library]

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
#ifndef __LOOPSYNC__
#define __LOOPSYNC__

#include <chrono>
#include <thread>

namespace min
{

class loop_sync
{
  private:
    std::chrono::high_resolution_clock::time_point _start;
    std::chrono::high_resolution_clock::time_point _stop;
    size_t _fps_count;
    double _fps_time;
    double _frame_time;
    std::chrono::high_resolution_clock::time_point _current_time;
    double _accum_time;
    double _idle_time;

    inline void get_time()
    {
        _current_time = now();
    }
    inline double delta(const std::chrono::high_resolution_clock::time_point &start, const std::chrono::high_resolution_clock::time_point &stop) const
    {
        return std::chrono::duration<double, std::milli>(stop - start).count();
    }
    inline double seconds(const std::chrono::high_resolution_clock::time_point &start, const std::chrono::high_resolution_clock::time_point &stop) const
    {
        return std::chrono::duration<double>(stop - start).count();
    }
    inline std::chrono::high_resolution_clock::time_point now()
    {
        // Calculate current time
        _current_time = std::chrono::high_resolution_clock::now();

        // Return current time
        return _current_time;
    }
    void stop()
    {
        _stop = now();
    }

  public:
    loop_sync(const double fps) : _fps_count(0), _fps_time(0.0), _frame_time(1000.0 / fps), _accum_time(0.0), _idle_time(0.0) {}
    double get_fps()
    {
        // Calculate the frames per second, time is in milliseconds
        double fps = (_fps_count) / _fps_time;

        // Reset the frame count and time
        _fps_count = 0;
        _fps_time = 0.0;

        // Return the calculated fps
        return fps;
    }
    double idle()
    {
        return _idle_time;
    }
    void start()
    {
        _start = now();
    }
    double sync()
    {
        // Get the stop time
        stop();

        // calculate render time
        const double render_time = delta(_start, _stop);

        // Synchronize the loop to poll a frame at 'tick' seconds
        if (_frame_time > render_time)
        {
            // Calculate the required delay which is the difference between the frame time and the render time
            // 'accum' is a 'catch up' variable and effectively lowers the calculated delay
            const double delay = _frame_time - render_time;
            const double adjust_delay = delay - _accum_time;
            if (adjust_delay > 0.0)
            {
                // Sleep for the calculated delay
                std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(adjust_delay));

                // Get time spent sleeping in milliseconds
                const auto time = now();
                const double slept = delta(_start, time);

                // Calculate the idle percentage (%)
                _idle_time = (delay / _frame_time) * 100;

                // Set the accumulation if we over-slept
                _accum_time = slept - adjust_delay;
            }
            else
            {
                // Play 'catch up' and don't sleep
                _accum_time -= delay;
                _idle_time = 0;
            }
        }
        else
        {
            // We took too long rendering the frame so we need to play catch up
            _accum_time += render_time - _frame_time;
            _idle_time = 0;
        }

        // Calculate the elapsed time since start() called in seconds
        const double out = seconds(_start, _current_time);

        // Update the frame count and the elapsed time
        _fps_count++;
        _fps_time += out;

        // Return the frame time step in seconds
        return out;
    }
};
}

#endif