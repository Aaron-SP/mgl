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
#ifndef __LOOPSYNC__
#define __LOOPSYNC__

#include <chrono>
#include <cmath>
#include <thread>

namespace min
{

class loop_sync
{
  private:
    // History buffer
    static constexpr uint8_t _error_count = 16;
    double _error[_error_count];
    double _idle[_error_count];
    uint8_t _begin;
    uint8_t _end;
    double _ie;
    double _idle_time;
    double _de;

    // Time properties
    std::chrono::high_resolution_clock::time_point _start;
    std::chrono::high_resolution_clock::time_point _current_time;

    // Control properties
    double _accum_time;
    double _set_point;
    double _kp;
    double _ki;
    double _kd;
    double _dt;
    double _error_tol;

    inline void calculate_control_parameters(const double idle_time)
    {
        // Drop off oldest record from buffer
        _begin = (_begin + 1) % _error_count;

        // Create new record on buffer
        const double prev = _error[_end];
        _end = (_end + 1) % _error_count;

        // Elapsed since start of frame
        // Ensure not zero, max 2000 FPS
        if (_dt < 0.0005)
        {
            _dt = 0.0005;
        }

        // Calculate integral of errors
        _ie = _error[_end] = _set_point - _dt;
        for (uint8_t i = _begin; i != _end; i = (i + 1) % _error_count)
        {
            _ie += _error[i];
        }

        // Calculate integral of idle time per frame
        _idle_time = _idle[_end] = idle_time;
        for (uint8_t i = _begin; i != _end; i = (i + 1) % _error_count)
        {
            _idle_time += _idle[i];
        }

        // Calculate the derivative of errors
        _de = (_error[_end] - prev);
    }
    inline double calculate_delay(const double idle_time) const
    {
        // Calculate the delay using PID equation
        double delay;
        if (std::abs(_error[_end]) > _error_tol)
        {
            const double p = _kp * _error[_end];
            const double i = _ki * _ie;
            const double d = _kd * _de;
            delay = idle_time + (p + i + d);

            // Check if delay is unstable
            if (delay > _set_point)
            {
                delay = _set_point;
            }
        }
        else
        {
            delay = idle_time;
        }

        // return delay
        return delay;
    }
    inline double diff()
    {
        // Calculate current time
        _current_time = std::chrono::high_resolution_clock::now();

        return std::chrono::duration<double>(_current_time - _start).count();
    }

  public:
    loop_sync(const double fps)
        : _error{}, _idle{}, _begin(0), _end(_error_count - 1),
          _ie(0.0), _idle_time(0.0), _de(0.0),
          _accum_time(0.0), _set_point(1.0 / fps),
          _kp(0.75), _ki(1.25), _kd(1.5), _dt(0.0),
          _error_tol(0.002) {}

    inline double get_fps() const
    {
        // Return the average fps
        return _error_count / (_set_point * _error_count - _ie);
    }
    inline double idle() const
    {
        // return the average idle time
        return (_idle_time * 100.0) / (_set_point * _error_count);
    }
    inline void start()
    {
        _start = std::chrono::high_resolution_clock::now();
    }
    double sync()
    {
        // Calculate time spent rendering
        _dt = diff();
        const double idle_time = _set_point - _dt;

        // Calculate the delay using PID equation and account for accumulated time
        const double adjust_delay = calculate_delay(idle_time) - _accum_time;

        // If we have time to kill, sleep on it
        if (adjust_delay > 0.0)
        {
            // Sleep for the calculated delay
            std::this_thread::sleep_for(std::chrono::duration<double>(adjust_delay));

            // Get time spent sleeping in seconds
            _dt = diff();

            // Set the accumulation if we over-slept
            _accum_time = _dt - adjust_delay;
        }
        else
        {
            // We took too long rendering, accumulate this time
            _accum_time -= idle_time;
        }

        // Calculate PID variables
        calculate_control_parameters(idle_time);

        // Return the frame time step in seconds
        return _dt;
    }
};
}

#endif
