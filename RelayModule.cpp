#include "RelayModule.h"
RelayModule::~RelayModule()
{
    debug("is destroyed.");
}
RelayModule::RelayModule(uint8_t pin)
{
    this->pin = pin;
}
RelayModule::RelayModule(uint8_t pin, uint8_t mode)
{
    this->pin = pin;
    this->isActiveLow = (mode == this->ACTIVE_LOW) ? true : false;
}
RelayModule::RelayModule(const char *name)
{
    this->name = String(name);
}
RelayModule::RelayModule(const char *name, uint8_t pin)
{
    this->name = String(name);
    this->pin = pin;
}
RelayModule::RelayModule(const char *name, uint8_t pin, uint8_t mode)
{
    this->name = String(name);
    this->pin = pin;
    this->isActiveLow = (mode == this->ACTIVE_LOW) ? true : false;
}
void RelayModule::begin()
{
    pinMode(this->pin, OUTPUT);
    off(); //Turn-off RelayModule
    delay(100);
}
void RelayModule::begin(uint8_t pin)
{
    this->pin = pin;
    begin();
}
void RelayModule::begin(uint8_t pin, uint8_t mode)
{
    setActiveMode(mode);
    begin(pin);
}
void RelayModule::begin(const char *name)
{
    this->name = String(name);
    begin();
}
void RelayModule::begin(const char *name, uint8_t pin)
{
    this->name = String(name);
    begin(pin);
}
void RelayModule::begin(const char *name, uint8_t pin, uint8_t mode)
{
    this->name = String(name);
    begin(pin, mode);
}
void RelayModule::debug(String msg)
{
    Serial.print(F("_DEBUG: RelayModule["));
    Serial.print(getName());
    Serial.print(F("] "));
    Serial.println(msg);
}
uint8_t RelayModule::getState()
{
    if (this->isActiveLow)
    {
        return digitalRead(this->pin) == LOW ? HIGH : LOW;
    }
    else
    {
        return digitalRead(this->pin) == HIGH ? HIGH : LOW;
    }
}
uint8_t RelayModule::setState(uint8_t state)
{
    if (this->isActiveLow)
    {
        state == LOW ? this->on() : this->off();
    }
    else
    {
        state == HIGH ? this->on() : this->off();
    }
}
uint8_t RelayModule::getPinout()
{
    return this->pin;
}
String RelayModule::getName()
{
    return this->name;
}
void RelayModule::setName(const char *name)
{
    this->name = String(name);
}
void RelayModule::setActiveMode(uint8_t mode)
{
    if (mode == this->ACTIVE_LOW)
    {
        this->isActiveLow = true;
    }
    else
    {
        this->isActiveLow = false;
    }
}
void RelayModule::on()
{
    if (this->isActiveLow)
    {
        if (digitalRead(this->pin) == HIGH)
        {
            digitalWrite(this->pin, LOW);
            #ifdef _DEBUG
                debug("Trun-on");
            #endif
        }
    }
    else
    {
        if (digitalRead(this->pin) == LOW)
        {
            digitalWrite(this->pin, HIGH);
            #ifdef _DEBUG
                debug("Trun-on");
            #endif
        }
    }
}
void RelayModule::off()
{
    if (this->isActiveLow)
    {
        if (digitalRead(this->pin) == LOW)
        {
            digitalWrite(this->pin, HIGH);
            #ifdef _DEBUG
                debug("Trun-off");
            #endif
        }
    }
    else
    {
        if (digitalRead(this->pin) == HIGH)
        {
            digitalWrite(this->pin, LOW);
            #ifdef _DEBUG
                debug("Trun-off");
            #endif
        }
    }
}
bool RelayModule::equal(uint8_t pin)
{
    return this->pin == pin;
}

void RelayModule::trunOnWhen(const char *startDateTime, const char *stopDateTime)
{
    return trunOnWhen(String(startDateTime), String(stopDateTime));
}

void RelayModule::trunOnWhen(String startDateTime, String stopDateTime)
{
    this->startTurnOnUnixtime = DateTimeLib::unixtime(startDateTime);
    this->stopTurnOnUnixtime = DateTimeLib::unixtime(stopDateTime);
    this->mode = RELAY_TURNON_DATETIME_MODE;
}

void RelayModule::trunOnWhen(uint16_t sYear, uint8_t sMonth, uint8_t sDay, uint8_t sHour, uint8_t sMinute, uint8_t sSecond, uint16_t eYear, uint8_t eMonth, uint8_t eDay, uint8_t eHour, uint8_t eMinute, uint8_t eSecond)
{
    this->startTurnOnUnixtime = DateTimeLib::unixtime(sYear, sMonth, sDay, sHour, sMinute, sSecond);
    this->stopTurnOnUnixtime = DateTimeLib::unixtime(eYear, eMonth, eDay, eHour, eMinute, eSecond);
    this->mode = RELAY_TURNON_DATETIME_MODE;
}

void RelayModule::doAction(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
    if (this->mode == RELAY_TURNON_DATETIME_MODE)
    {
        uint32_t utNow = DateTimeLib::unixtime(year, month, day, hour, minute, second);
        (utNow >= this->startTurnOnUnixtime && utNow <= stopTurnOnUnixtime) ? on() : off();
    }
}

void RelayModule::reset()
{
    this->mode = RELAY_NORMAL_MODE;
    off();
}