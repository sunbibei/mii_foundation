/*
 * msg_queue.cpp
 *
 *  Created on: Feb 7, 2018
 *      Author: bibei
 */

#include "foundation/ipc/msg_queue.h"
#include "foundation/internal/sync.h"

using namespace internal;

SINGLETON_IMPL(MsgQueue)

MsgQueue::MsgQueue() {
  __init_key_map();
}

MsgQueue::~MsgQueue() {
  for (const auto& itr : key_map_)
    __sub_count_key_map(itr.first);

  key_map_.clear();
}

bool MsgQueue::create_msgq(const std::string& _n) {
  if (key_map_.end() != key_map_.find(_n)) return true;

  key_t _key = __find_ava_key(_n, IPC_TYPE::MSG_QUEUE);
  // if (IPC_PRIVATE == _key)
  ///! First, try to find the memory.
  int _msg_id = msgget(_key, IPC_EXCL);
  if (-1 == _msg_id) { // Could not found.
    _msg_id = msgget(_key, IPC_CREAT | 0666);
    if (-1 == _msg_id) {
      LOG_ERROR << "Can't create the named message queue '" << _n << "'";
      return false;
    } else {
      LOG_INFO << "Create the named message queue '" << _n << "'";
      __add_key_map(_n, _key, _msg_id, IPC_TYPE::MSG_QUEUE);
    }
  } else { // Found
    ; // Nothing to do here.
    LOG_INFO << "Found the named message queue '" << _n << "'";
  }

  key_map_.insert(std::make_pair(_n, _key));
  id_map_.insert(std::make_pair(_n,  _msg_id));
  __add_count_key_map(_n);
  return true;
}

/*!
 * @brief Get the data.
 */
bool MsgQueue::read_from_msgq(const std::string& _n, MsgBase* _msg, size_t _s) {
  auto _msgq_id = get_msgq_id(_n);
  if (-1 == _msgq_id) return false;

  return (msgrcv(_msgq_id, _msg, _s, 0, IPC_NOWAIT) > 0);
}

/*!
 * @brief Get the data.
 */
bool MsgQueue::write_to_msgq(const std::string& _n, const MsgBase* _msg, size_t _s) {
  auto _msgq_id = get_msgq_id(_n);
  if (-1 == _msgq_id) return false;

  return (msgsnd(_msgq_id, _msg, _s, IPC_NOWAIT) > 0);
}

int MsgQueue::get_msgq_id(const std::string& _n) {
  if (id_map_.end() == id_map_.find(_n)) return -1;
  else return id_map_[_n];
}

void MsgQueue::clear() {
  __clear(IPC_TYPE::MSG_QUEUE);
}
