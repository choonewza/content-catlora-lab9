#ifndef RelayModule_h
#define RelayModule_h

#include "Arduino.h"
#include "DateTimeLib.h"

#define _DEBUG 1

#define RELAY_NORMAL_MODE 0
#define RELAY_TURNON_DATETIME_MODE 1

class RelayModule
{
protected:
  String name;
  uint8_t pin;
  bool isActiveLow = true;
  uint32_t startTurnOnUnixtime = 0;
  uint32_t stopTurnOnUnixtime = 0;
  uint8_t mode = RELAY_NORMAL_MODE;
  //Mode 0 is Normal Mode
  //Mode 1 is Trun-On by DateTime Mode

public:
  static const uint8_t ACTIVE_LOW = 0;
  static const uint8_t ACTIVE_HIGH = 1;
  virtual ~RelayModule();
  RelayModule(uint8_t pin);
  RelayModule(uint8_t pin, uint8_t mode);
  RelayModule(const char *name);
  RelayModule(const char *name, uint8_t pin);
  RelayModule(const char *name, uint8_t pin, uint8_t mode);
  void begin();
  void begin(uint8_t pin);
  void begin(uint8_t pin, uint8_t mode);
  void begin(const char *name);
  void begin(const char *name, uint8_t pin);
  void begin(const char *name, uint8_t pin, uint8_t mode);
  void debug(String msg);
  uint8_t getState();
  uint8_t setState(uint8_t state);
  uint8_t getPinout();
  String getName();
  void setName(const char *name);
  void setActiveMode(uint8_t mode);
  virtual void on();
  virtual void off();
  bool equal(uint8_t pin);
  void trunOnWhen(const char *startDateTime, const char *stopDateTime);
  void trunOnWhen(String startDateTime, String stopDateTime);
  void trunOnWhen(uint16_t sYear, uint8_t sMonth, uint8_t sDay, uint8_t sHour, uint8_t sMinute, uint8_t sSecond, uint16_t eYear, uint8_t eMonth, uint8_t eDay, uint8_t eHour, uint8_t eMinute, uint8_t eSecond);
  void doAction(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
  void reset();
};
#endif