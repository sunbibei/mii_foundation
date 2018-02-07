/*
 * MsgQueue.h
 *
 *  Created on: Feb 7, 2018
 *      Author: bibei
 */

#ifndef MSG_QUEUE_H_
#define MSG_QUEUE_H_

#include "../utf.h"
#include <sys/msg.h>

typedef struct {
  long msg_id;
} MsgBase;

class MsgQueue {
  SINGLETON_DECLARE(MsgQueue)

public:
  /*!
   * @brief Create the shared memory with the given name
   */
  bool create_msgq(const std::string&);
  /*!
   * @brief Get the data.
   */
  bool read_from_msgq(const std::string& _n, MsgBase*, size_t);
  /*!
   * @brief Get the data.
   */
  bool write_to_msgq(const std::string& _n, const MsgBase*, size_t);

  /*!
   * @brief Get the message queue id.
   */
  int get_msgq_id(const std::string&);

  static void clear();

private:
  std::map<std::string, key_t> key_map_;
  std::map<std::string, int>   id_map_;
};

#endif /* MSG_QUEUE_H_ */
