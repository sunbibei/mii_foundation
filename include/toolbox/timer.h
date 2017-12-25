/*
 * timer.h
 *
 *  Created on: Dec 22, 2017
 *      Author: bibei
 */

#ifndef INCLUDE_TOOLBOX_TIMER_H_
#define INCLUDE_TOOLBOX_TIMER_H_

#include <chrono>

namespace middleware {

class Timer {
public:
  Timer();

  /*!
   * @brief The timer whether is running.
   * @return return true if the timer is running.
   */
  bool is_running();

  /*!
   * @brief Start the timer.
   */
  void start();

  /*!
   * @brief The duration (in ms)
   */
  int64_t dt();
  /*!
   * @brief The duration (in s)
   */
  double  dt_s();

  /*!
   * @brief Stop the timer, and return timespan from start();
   * @param span The dufault is nullptr, if you don't care the timespan.
   */
  void stop(int64_t* span = nullptr);

private:
  ///! time control (in ms)
  int64_t dt_;
  std::chrono::high_resolution_clock::time_point curr_update_t_;
  std::chrono::high_resolution_clock::time_point last_update_t_;

  ///! these variables for debug.
  std::chrono::high_resolution_clock::time_point t0_;
  std::chrono::high_resolution_clock::time_point t1_;

  const std::chrono::high_resolution_clock::time_point INVALID_TIME_POINT;
};

} /* namespace middleware */

#endif /* INCLUDE_TOOLBOX_TIMER_H_ */
