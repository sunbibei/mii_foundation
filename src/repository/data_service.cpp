/*
 * data_service.cpp
 *
 *  Created on: Dec 26, 2017
 *      Author: bibei
 */

#include <repository/data_service.h>
#include <foundation/cfg_reader.h>
#include <foundation/thread/threadpool.h>
#include <repository/resource/joint_manager.h>
#include <repository/resource/force_sensor.h>
#include <toolbox/timer.h>

#include <stdio.h>

namespace middleware {

struct DataSources {
  MiiVector<const double*> jnt_pos;
  MiiVector<const double*> jnt_vel;
  MiiVector<const double*> jnt_tor;
  MiiVector<const double*> jnt_cmd;
  MiiVector<const double*> leg_td;
};

const size_t DS_BUF_SIZE = 128;

DataService::DataService(const MiiString& _l)
  : Label(_l), enable_(false), path_("."),
    ofn_(""), buffer_(nullptr),
    tick_alive_(false), tick_duration_(20),
    timer_(nullptr), sources_(nullptr) {
  ;
}

MiiString __cvtJnt2Str(JntType t) {
  switch (t) {
  case JntType::YAW:
    return "YAW";
  case JntType::HIP:
    return "HIP";
  case JntType::KNEE:
    return "KNEE";
  case JntType::N_JNTS:
    return "N_JNTS";
  default:
    return "UNKNOWN_JNT";
  }
}

MiiString __cvtLeg2Str(LegType t) {
  switch (t) {
  case LegType::FL:
    return "FL";
  case LegType::FR:
    return "FR";
  case LegType::HL:
    return "HL";
  case LegType::HR:
    return "HR";
  case LegType::N_LEGS:
    return "N_LEGS";
  default:
    return "UNKNOWN_LEG";
  }
}

bool DataService::init() {
  auto cfg = MiiCfgReader::instance();

  ///! No enable.
  if (!cfg->get_value(getLabel(), "enable", enable_) || !enable_)
    return true;

  double frequency = 50.0;
  cfg->get_value(getLabel(), "frequency", frequency);
  tick_duration_ = std::chrono::milliseconds((int64_t)1000.0/frequency);
  tick_alive_    = true;

  path_ = ".";
  cfg->get_value(getLabel(), "path", path_);

  buffer_ = new char[DS_BUF_SIZE];
  memset(buffer_, 0x00, DS_BUF_SIZE * sizeof(char));

  time_t _time;
  time(&_time);
  tm* _tm = std::localtime(&_time);

  sprintf(buffer_, "qr_log_%4d-%02d-%02d_%02d-%02d-%02d", _tm->tm_year + 1900, _tm->tm_mon,
      _tm->tm_mday, _tm->tm_hour, _tm->tm_min, _tm->tm_sec);
  ofn_ = buffer_;
  ofd_.open(path_ + "/" + ofn_);
  if (!ofd_.is_open()) return false;
  ofd_ << "time ";

  sources_ = new DataSources;

  MiiString sub_tag = Label::make_label(getLabel(), "source");
  MiiVector<MiiString> str;
  cfg->get_value(sub_tag, "joint_states", str);
  MiiVector<LegType> legs;
  cfg->get_value(sub_tag, "legs", legs);
  bool is_cmd = false;
  cfg->get_value(sub_tag, "joint_commands", is_cmd);
  if (is_cmd) str.push_back("command");

  if (legs.empty()) {
    legs = {LegType::FL, LegType::FR, LegType::HL, LegType::HR};
//    legs.push_back(LegType::FL);
//    legs.push_back(LegType::FR);
//    legs.push_back(LegType::HL);
//    legs.push_back(LegType::HR);
  }

  auto _jnts = JointManager::instance();
  while (!str.empty()) {
    const auto& t = str.back();
    for (const auto& l : legs) {
      for (const auto& j : {JntType::YAW, JntType::HIP, JntType::KNEE}) {
        auto jnt = _jnts->getJointHandle(l, j);

        if (0 == t.compare("position")) {
          sources_->jnt_pos.push_back(jnt->joint_position_const_pointer());
        } else if (0 == t.compare("velocity")) {
          sources_->jnt_vel.push_back(jnt->joint_velocity_const_pointer());
        } else if (0 == t.compare("torque")) {
          sources_->jnt_tor.push_back(jnt->joint_torque_const_pointer());
        } else if (0 == t.compare("command")) {
          sources_->jnt_cmd.push_back(jnt->joint_command_const_pointer());
        } else {
          LOG_ERROR << "Error joint states indices!";
        }

        ofd_ << jnt->joint_name() << t << " ";
      } // end for j
    } // end for l

    str.pop_back();
  } // end while

  cfg->get_value(sub_tag, "tds", str);
  sources_->leg_td.resize(str.size());
  for (const auto& td : str) {
    auto ptd = Label::getHardwareByName<ForceSensor>(td);
    if (nullptr == ptd) {
      LOG_ERROR << "Can't found the named " << td << " TD sensor";
      continue;
    }

    sources_->leg_td[ptd->leg_type()] = ptd->force_data_const_pointer();
    ofd_ << __cvtLeg2Str(ptd->leg_type()) << "_TD" << " ";
  }

  ofd_ << std::endl;
  return true;
}

DataService::~DataService() {
  tick_alive_ = false;
  if (ofd_.is_open()) ofd_.close();

  delete buffer_;
  buffer_ = nullptr;
}

void DataService::tick() {
  TIMER_INIT

  timer_ = new Timer;
  timer_->start();
  while (tick_alive_) {
    ofd_ << timer_->dt() << " ";
    for (const auto& val : sources_->jnt_pos)
      ofd_ << *val << " ";
    for (const auto& val : sources_->jnt_vel)
      ofd_ << *val << " ";
    for (const auto& val : sources_->jnt_tor)
      ofd_ << *val << " ";
    for (const auto& val : sources_->jnt_cmd)
      ofd_ << *val << " ";
    for (const auto& val : sources_->leg_td)
      ofd_ << *val << " ";

    ofd_ << std::endl;
    TIMER_CONTROL(tick_duration_)
  }

  delete timer_;
  timer_ = nullptr;
}

} /* namespace middleware */

#include <class_loader/class_loader_register_macro.h>
CLASS_LOADER_REGISTER_CLASS(middleware::DataService, Label)
