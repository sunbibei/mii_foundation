/*
 * MsgQueue.cpp
 *
 *  Created on: Feb 7, 2018
 *      Author: bibei
 */

#include "msg_queue.h"
#include "internal/sync.h"

using namespace internal;

SINGLETON_IMPL(MsgQueue)

MsgQueue::MsgQueue() {

}

MsgQueue::~MsgQueue() {
  // TODO Auto-generated destructor stub
}

bool MsgQueue::create_msgq(const std::string& _n) {
  if (key_map_.end() != key_map_.find(_n)) return true;

  key_t _key = __find_ava_key(_n, IPC_TYPE::MSG_QUEUE);

  ///! First, try to find the memory.
  int _msg_id = msgget(_key, IPC_EXCL);
  if (-1 == _msg_id) { // Could not found.
    _msg_id = msgget(_key, IPC_CREAT | 0666);
    if (-1 == _msg_id) {
      LOG_ERROR << "Can't create the named message queue '" << _n << "'";
      return false;
    } else {
      LOG_INFO << "Create the named shared memory '" << _n << "'";
      __add_key_map(_n, _key, _msg_id, IPC_TYPE::MSG_QUEUE);
    }
  } else { // Found
    ; // Nothing to do here.
    LOG_INFO << "Found the named shared memory '" << _n << "'";
  }

  key_map_.insert(std::make_pair(_n, _key));
  return true;
}

