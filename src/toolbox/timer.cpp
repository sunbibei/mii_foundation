/*
 * timer.cpp
 *
 *  Created on: Dec 22, 2017
 *      Author: bibei
 */

#include <toolbox/timer.h>

namespace middleware {

Timer::Timer()
  : INVALID_TIME_POINT(std::chrono::high_resolution_clock::time_point::max()) {
  curr_update_t_ = INVALID_TIME_POINT;
  last_update_t_ = INVALID_TIME_POINT;
  dt_ = 0;
  t0_ = INVALID_TIME_POINT;
  t1_ = INVALID_TIME_POINT;
}

bool Timer::is_running() {
  return (last_update_t_ != INVALID_TIME_POINT);
}

void Timer::start() {
  curr_update_t_ = std::chrono::high_resolution_clock::now();
  last_update_t_ = curr_update_t_;
  t0_            = curr_update_t_;
}

/*!
 * @brief The duration (in ms)
 */
int64_t Timer::dt() {
  curr_update_t_ = std::chrono::high_resolution_clock::now();
  dt_            = std::chrono::duration_cast<std::chrono::milliseconds>
      (curr_update_t_ - last_update_t_).count();
  last_update_t_ = curr_update_t_;
  return dt_;
}

double Timer::dt_s() {
  return dt()/1000.0;
}

void Timer::stop(int64_t* span) {
  t1_ = std::chrono::high_resolution_clock::now();
  curr_update_t_ = INVALID_TIME_POINT;
  last_update_t_ = INVALID_TIME_POINT;
  if (span) *span= std::chrono::duration_cast<std::chrono::milliseconds>
                      (t1_ - t0_).count();
}

} /* namespace middleware */
