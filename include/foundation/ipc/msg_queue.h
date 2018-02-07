/*
 * MsgQueue.h
 *
 *  Created on: Feb 7, 2018
 *      Author: bibei
 */

#ifndef MSG_QUEUE_H_
#define MSG_QUEUE_H_

#include "utf.h"
#include <sys/msg.h>

class MsgQueue {
  SINGLETON_DECLARE(MsgQueue)

public:
  /*!
   * @brief Create the shared memory with the given name.
   */
  bool create_msgq(const std::string&);
  /*!
   * @brief Get the data.
   */
  template<typename _T>
  bool read_from_msgq(const std::string& _n, _T&);
  /*!
   * @brief Get the data.
   */
  template<typename _T>
  bool write_to_msgq(const std::string& _n, const _T&);

private:
  std::map<std::string, key_t> key_map_;
};

#endif /* MSG_QUEUE_H_ */
