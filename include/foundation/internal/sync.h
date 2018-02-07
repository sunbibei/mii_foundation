/*
 * res_sync.h
 *
 *  Created on: Feb 7, 2018
 *      Author: bibei
 */

#ifndef INTERNAL_SYNC_H_
#define INTERNAL_SYNC_H_

#include "../utf.h"
#include <sys/shm.h>
#include <sys/msg.h>

namespace internal {

const size_t MAX_NAME_SIZE = 32;
const size_t MAX_SHM_SIZE  = 16;

enum IPC_TYPE{
  UNKNOWN = 0x00,
  SHM,
  MSG_QUEUE,
  N_IPC
};

struct _PrivateKeyMap {
  char     name[MAX_NAME_SIZE];
  IPC_TYPE type;
  key_t    key;
  int      id;
  size_t   count;
};

const key_t       KEY_MAP_OF_KEY = 0x2000;
const std::string SYNC_IDENTIFY  = "SYNC";

bool __init_key_map();
bool __add_key_map(const std::string& _n, key_t _key, int _id, IPC_TYPE _type);
bool __add_count_key_map(const std::string& _n);
bool __sub_count_key_map(const std::string& _n);
key_t __find_ava_key(const std::string& _n, IPC_TYPE type);

void __clear(IPC_TYPE type = N_IPC);

///! For debug
void __print_all_ipc();

} /*end namespace internal */

#endif /* INTERNAL_SYNC_H_ */
