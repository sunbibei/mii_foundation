/*
 * data_service.h
 *
 *  Created on: Dec 26, 2017
 *      Author: bibei
 */

#ifndef INCLUDE_REPOSITORY_DATA_SERVICE_H_
#define INCLUDE_REPOSITORY_DATA_SERVICE_H_

#include <foundation/label.h>
#include <fstream>
#include <chrono>

namespace middleware {

class DataService: public Label {
public:
  DataService(const MiiString& _l = "data-service");
  virtual bool init() override;

  virtual ~DataService();

public:
  virtual void start();
  virtual void tick();

protected:
  bool          enable_;
  MiiString     path_;
  MiiString     ofn_;
  std::ofstream ofd_;
  char*         buffer_;

  bool                       tick_alive_;
  std::chrono::milliseconds  tick_duration_;

  class Timer*       timer_;
  class DataSources* sources_;

};

} /* namespace middleware */

#endif /* INCLUDE_REPOSITORY_DATA_SERVICE_H_ */
