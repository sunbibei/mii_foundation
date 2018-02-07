/*
 * sync.cpp
 *
 *  Created on: Feb 7, 2018
 *      Author: bibei
 */

#include "sync.h"

namespace internal {
///////////////////////////////////////////////////////////////////////////////
//////////////           The helper method define first   /////////////////////
///////////////////////////////////////////////////////////////////////////////
bool __init_key_map() {
  ///! First, try to find the memory.
  int _shm_id = shmget(KEY_MAP_OF_KEY, 0, 0);
  if (-1 == _shm_id) { // Could not found.
    _shm_id = shmget(KEY_MAP_OF_KEY, MAX_SHM_SIZE * sizeof(_PrivateKeyMap), IPC_CREAT | 0666);
    if (-1 == _shm_id) {
      LOG_ERROR << "Can't initialize the sync shared memory '" << SYNC_IDENTIFY << "'";
      return false;
    } else {
      LOG_INFO << "Create the sync shared memory '" << SYNC_IDENTIFY << "'";
      __add_key_map(SYNC_IDENTIFY, KEY_MAP_OF_KEY, _shm_id, IPC_TYPE::SHM);
    }
  } else { // Found
    ; // Nothing to do here.
    LOG_INFO << "Found the sync shared memory '" << SYNC_IDENTIFY << "'";
  }

  return true;
}

bool __add_key_map(const std::string& _n, key_t _key, int _id, IPC_TYPE _type) {
  auto _addr = shmat(shmget(KEY_MAP_OF_KEY, 0, 0), NULL, 0);
  if (nullptr == _addr) return false;
  _PrivateKeyMap* _key_map = (_PrivateKeyMap*) _addr;

  ///! initialize the structure.
  if (0 == SYNC_IDENTIFY.compare(_n)) {
    LOG_DEBUG << "Initialize the all of key_map";
    size_t off = 0;
    strcpy(_key_map[off].name, _n.c_str());
    _key_map[off].type   = SHM;
    _key_map[off].key    = _key;
    _key_map[off].id     = _id;
    _key_map[off].count  = 0;

    ++off;
    for (; off < MAX_SHM_SIZE; ++off) {
      memset(_key_map[off].name, 0x00, MAX_NAME_SIZE * sizeof(char));
      _key_map[off].key    = IPC_PRIVATE;
      _key_map[off].type   = UNKNOWN;
      _key_map[off].id     = -1;
      _key_map[off].count  = 0;
    }
  } else { ///! Add a new block.
    LOG_DEBUG << "Add a new key_map";
    for (size_t offset = 0; offset < MAX_SHM_SIZE; ++offset) {
      if (IPC_PRIVATE == _key_map[offset].key) {
        LOG_DEBUG << "OK";
        strcpy(_key_map[offset].name, _n.c_str());
        _key_map[offset].key    = _key;
        _key_map[offset].type   = _type;
        _key_map[offset].id     = _id;
        _key_map[offset].count  = 0;
        break;
      }
    }
  }

  return true;
}

bool __add_count_key_map(const std::string& _n) {
  auto _addr = shmat(shmget(KEY_MAP_OF_KEY, 0, 0), NULL, 0);
  if (nullptr == _addr) return false;
  _PrivateKeyMap* _key_map = (_PrivateKeyMap*) _addr;

  for (size_t offset = 0; offset < MAX_SHM_SIZE; ++offset) {
    if (0 == _n.compare(_key_map[offset].name)) {
      ++_key_map[offset].count;
      LOG_DEBUG << "The count of '" << _n << "' is " << _key_map[offset].count;
      return true;
    }
  }

  return false;
}

bool __sub_count_key_map(const std::string& _n) {
  auto _addr = shmat(shmget(KEY_MAP_OF_KEY, 0, 0), NULL, 0);
  if (nullptr == _addr) return false;
  _PrivateKeyMap* _key_map = (_PrivateKeyMap*) _addr;

  for (size_t offset = 0; offset < MAX_SHM_SIZE; ++offset) {
    if (0 == _n.compare(_key_map[offset].name)) {
      --_key_map[offset].count;
      LOG_DEBUG << "The count of '" << _n << "' is " << _key_map[offset].count;

      if (0 == _key_map[offset].count) {
        LOG_DEBUG << "remove the shared memory -> '" << _n << "'";
        if (-1 != _key_map[offset].id) {
          if (SHM == _key_map[offset].type)
            shmctl(_key_map[offset].id, IPC_RMID, NULL);
          else if (MSG_QUEUE == _key_map[offset].type)
            msgctl(_key_map[offset].id, IPC_RMID, NULL);
          else
            LOG_ERROR << "What fucking code."; // unreached code
        }
        memset(_key_map[offset].name, 0x00, MAX_NAME_SIZE * sizeof(char));
        _key_map[offset].key    = IPC_PRIVATE;
        _key_map[offset].type   = UNKNOWN;
        _key_map[offset].id     = -1;
        _key_map[offset].count  = 0;
      }
      for (size_t off = 0; off < MAX_SHM_SIZE; ++off)
        if (IPC_PRIVATE != _key_map[off].key) return true;

      LOG_DEBUG << "remove the shared memory -> '" << SYNC_IDENTIFY << "'";
      shmctl(shmget(KEY_MAP_OF_KEY, 0, 0), IPC_RMID, NULL);
      return true;
    }
  }

  return false;
}

key_t __find_ava_key(const std::string& _n, IPC_TYPE type) {
  auto _addr = shmat(shmget(KEY_MAP_OF_KEY, 0, 0), NULL, 0);
  if (nullptr == _addr) return false;
  _PrivateKeyMap* _key_map = (_PrivateKeyMap*) _addr;

  for (size_t offset = 0; offset < MAX_SHM_SIZE; ++offset) {
    if (0 == _n.compare(_key_map[offset].name)) {
      return _key_map[offset].key;
    }
  }

  for (key_t key = KEY_MAP_OF_KEY + 0x01; key < KEY_MAP_OF_KEY + MAX_SHM_SIZE; key += 0x01)
    if (SHM == type)
      if (-1 == shmget(key, 0, 0)) return key;
    else if (MSG_QUEUE == type) {
      if (-1 == msgget(key, IPC_EXCL)) return key;
    } else {
      LOG_ERROR << "What fucking code.";
      ; // unreached code
    }

  return IPC_PRIVATE;
}

} /*end namespace internal */
