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
#include <thread>

namespace min
{

class loop_sync
{
  private:
    // History buffer
    static constexpr uint8_t _error_count = 15;
    std::vector<double> _error;
    uint8_t _begin;
    uint8_t _end;
    double _ie;
    double _de;

    // Time properties
    std::chrono::high_resolution_clock::time_point _start;
    std::chrono::high_resolution_clock::time_point _current_time;

    // Control properties
    double _accum_time;
    double _idle_time;
    double _set_point;
    double _kp;
    double _ki;
    double _kd;
    double _dt;

    inline void calculate_control_parameters()
    {
        // Drop off oldest record from buffer
        _begin = (_begin + 1) % _error_count;

        // Create new record on buffer
        const double prev = _error[_end];
        _end = (_end + 1) % _error_count;

        // Elapsed since start of frame, ensure not zero
        if (_dt < 0.001)
        {
            _dt = 0.001;
        }

        // Calculate the error
        _error[_end] = (_set_point - _dt);

        // Calculate integral of errors
        _ie = 0.0;
        for (uint8_t i = _begin; i != _end; i = (i + 1) % _error_count)
        {
            _ie += _error[i] * _dt;
        }

        // Calculate the derivative of errors
        _de = (_error[_end] - prev);
    }
    inline double diff()
    {
        // Calculate current time
        _current_time = std::chrono::high_resolution_clock::now();

        return std::chrono::duration<double>(_current_time - _start).count();
    }

  public:
    loop_sync(const double fps)
        : _error(_error_count, 0.0), _begin(0), _end(_error_count - 1), _ie(0.0), _de(0.0),
          _accum_time(0.0), _idle_time(0.0), _set_point(1.0 / fps),
          _kp(1.0), _ki(0.25 / (_error_count * _set_point * _set_point)), _kd(1.0), _dt(0.0) {}

    inline double get_fps() const
    {
        // Return the average fps
        return 1.0 / (_set_point + (_ie / _error_count));
    }
    inline double idle() const
    {
        return _idle_time;
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

        // Calculate the delay using PID equation
        const double p = _kp * _error[_end];
        const double i = _ki * _ie;
        const double d = _kd * _de;

        const double delay = idle_time * (p + i + d + 1.0);
        const double adjust_delay = delay - _accum_time;

        // If we have time to kill, sleep on it
        if (adjust_delay > 0.0)
        {
            // Sleep for the calculated delay
            std::this_thread::sleep_for(std::chrono::duration<double>(adjust_delay));

            // Get time spent sleeping in seconds
            _dt = diff();

            // Set the accumulation if we over-slept
            _accum_time = _dt - adjust_delay;

            // Calculate the idle percentage (%)
            _idle_time = (idle_time / _set_point) * 100;
        }
        else
        {
            // We took too long rendering, accumulate this time
            _accum_time -= idle_time;
            _idle_time = 0;
        }

        // Calculate PID variables
        calculate_control_parameters();

        // Return the frame time step in seconds
        return _dt;
    }
};
}

#endif
