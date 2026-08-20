// -----
// OneButton.h - Library for detecting button clicks, doubleclicks and long
// press pattern on a single button. This class is implemented for use with the
// Arduino environment. Copyright (c) by Matthias Hertel,
// http://www.mathertel.de This work is licensed under a BSD style license. See
// http://www.mathertel.de/License.aspx More information on:
// http://www.mathertel.de/Arduino
// -----
// 02.10.2010 created by Matthias Hertel
// 21.04.2011 transformed into a library
// 01.12.2011 include file changed to work with the Arduino 1.0 environment
// 23.03.2014 Enhanced long press functionalities by adding longPressStart and
// longPressStop callbacks
// 21.09.2015 A simple way for debounce detection added.
// 14.05.2017 Debouncing improvements.
// 25.06.2018 Optional third parameter for deactivating pullup.
// 26.09.2018 Anatoli Arkhipenko: Included solution to use library with other
// sources of input.
// 26.09.2018 Initialization moved into class declaration.
// 26.09.2018 Jay M Ericsson: compiler warnings removed.
// 29.01.2020 improvements from ShaggyDog18
// -----

#ifndef OneButton_h
#define OneButton_h


// ----- Callback function types -----

extern "C" {
typedef void (*callbackFunction)(void);
typedef void (*parameterizedCallbackFunction)(void *);
}


class OneButton
{
public:
  // ----- Constructor -----
  OneButton();

  /**
   * Initialize the OneButton library.
   * @param pin The pin to be used for input from a momentary button.
   * @param activeLow Set to true when the input level is LOW when the button is pressed, Default is true.
   * @param pullupActive Activate the internal pullup when available. Default is true.
   */
  OneButton(const int pin, const bool activeLow = true);

  // ----- Set runtime parameters -----

  /**
   * set the given onebutton pin as input.
   */
  void begin(const bool pullupActive = true);

  /**
   * set # millisec after safe click is assumed.
   */
  void setDebounceTicks(const int ticks);

  /**
   * set # millisec after single click is assumed.
   */
  void setClickTicks(const int ticks);

  /**
   * set # millisec after press is assumed.
   */
  void setPressTicks(const int ticks);

  /**
   * Attach an event to be called when a single click is detected.
   * @param newFunction This function will be called when the event has been detected.
   */
  void attachClick(callbackFunction newFunction);
  void attachClick(parameterizedCallbackFunction newFunction, void *parameter);

  /**
   * Attach an event to be called after a double click is detected.
   * @param newFunction This function will be called when the event has been detected.
   */
  void attachDoubleClick(callbackFunction newFunction);
  void attachDoubleClick(parameterizedCallbackFunction newFunction, void *parameter);

  /**
   * Attach an event to be called after a multi click is detected.
   * @param newFunction This function will be called when the event has been detected.
   */
  void attachMultiClick(callbackFunction newFunction);
  void attachMultiClick(parameterizedCallbackFunction newFunction, void *parameter);

  /**
   * Attach an event to fire when the button is pressed and held down.
   * @param newFunction
   */
  void attachLongPressStart(callbackFunction newFunction);
  void attachLongPressStart(parameterizedCallbackFunction newFunction, void *parameter);

  /**
   * Attach an event to fire as soon as the button is released after a long press.
   * @param newFunction
   */
  void attachLongPressStop(callbackFunction newFunction);
  void attachLongPressStop(parameterizedCallbackFunction newFunction, void *parameter);

  /**
   * Attach an event to fire periodically while the button is held down.
   * @param newFunction
   */
  void attachDuringLongPress(callbackFunction newFunction);
  void attachDuringLongPress(parameterizedCallbackFunction newFunction, void *parameter);

  // ----- State machine functions -----

  /**
   * @brief Call this function every some milliseconds for checking the input
   * level at the initialized digital pin.
   */
  void tick(void);


  /**
   * @brief Call this function every time the input level has changed.
   * Using this function no digital input pin is checked because the current
   * level is given by the parameter.
   */
  void tick(bool level);


  /**
   * Reset the button state machine.
   */
  void reset(void);


  /*
   * return number of clicks in any case: single or multiple clicks
   */
  int getNumberClicks(void);


  /**
   * @return true if we are currently handling button press flow
   * (This allows power sensitive applications to know when it is safe to power down the main CPU)
   */
  bool isIdle() const { return _state == OCS_INIT; }

  /**
   * @return true when a long press is detected
   */
  bool isLongPressed() const { return _state == OCS_PRESS; };


private:
  int _pin;                         // hardware pin number.
  unsigned int _debounceTicks = 50; // number of ticks for debounce times.
  unsigned int _clickTicks = 400;   // number of msecs before a click is detected.
  unsigned int _pressTicks = 800;   // number of msecs before a long button press is detected

  int _buttonPressed;

  // These variables will hold functions acting as event source.
  callbackFunction _clickFunc = NULL;
  parameterizedCallbackFunction _paramClickFunc = NULL;
  void *_clickFuncParam = NULL;

  callbackFunction _doubleClickFunc = NULL;
  parameterizedCallbackFunction _paramDoubleClickFunc = NULL;
  void *_doubleClickFuncParam = NULL;

  callbackFunction _multiClickFunc = NULL;
  parameterizedCallbackFunction _paramMultiClickFunc = NULL;
  void *_multiClickFuncParam = NULL;

  callbackFunction _longPressStartFunc = NULL;
  parameterizedCallbackFunction _paramLongPressStartFunc = NULL;
  void *_longPressStartFuncParam = NULL;

  callbackFunction _longPressStopFunc = NULL;
  parameterizedCallbackFunction _paramLongPressStopFunc = NULL;
  void *_longPressStopFuncParam;

  callbackFunction _duringLongPressFunc = NULL;
  parameterizedCallbackFunction _paramDuringLongPressFunc = NULL;
  void *_duringLongPressFuncParam = NULL;

  // These variables that hold information across the upcoming tick calls.
  // They are initialized once on program start and are updated every time the
  // tick function is called.

  // define FiniteStateMachine
  enum stateMachine_t : int {
    OCS_INIT = 0,
    OCS_DOWN = 1,
    OCS_UP = 2,
    OCS_COUNT = 3,
    OCS_PRESS = 6,
    OCS_PRESSEND = 7,
    UNKNOWN = 99
  };

  /**
   *  Advance to a new state and save the last one to come back in cas of bouncing detection.
   */
  void _newState(stateMachine_t nextState);

  stateMachine_t _state = OCS_INIT;
  stateMachine_t _lastState = OCS_INIT; // used for debouncing

  unsigned long _startTime; // start of current input change to checking debouncing
  int _nClicks;             // count the number of clicks with this variable
  int _maxClicks = 1;       // max number (1, 2, multi=3) of clicks of interest by registration of event functions.
};



// ----- Initialization and Default Values -----

/**
 * @brief Construct a new OneButton object but not (yet) initialize the IO pin.
 */
OneButton::OneButton()
{
  _pin = -1;
  // further initialization has moved to OneButton.h
}

/**
 * Initialize the OneButton library.
 * @param pin The pin to be used for input from a momentary button.
 * @param activeLow Set to true when the input level is LOW when the button is pressed, Default is true.
 * @param pullupActive Activate the internal pullup when available. Default is true.
 */
OneButton::OneButton(const int pin, const bool activeLow)
{
  // OneButton();
  _pin = pin;

  if (activeLow) {
    // the button connects the input pin to GND when pressed.
    _buttonPressed = 0;

  } else {
    // the button connects the input pin to VCC when pressed.
    _buttonPressed = 1;
  } // if
} // OneButton


// set the given onebutton pin as input.
void OneButton::begin(const bool pullupActive)
{
    // use the given pin as input.
    #if defined(TW_ASR_PRO)
    pinMode(_pin,input);
    set_pin_to_gpio_mode(_pin);
    // activate internal PULLUP resistor.
    if (pullupActive) {
        dpmu_set_io_pull(pinToFun[_pin],DPMU_IO_PULL_UP);
    }
    #elif defined(__ASR_H__)
    pinMode(_pin,input);
    if((_pin<=8) && (_pin >=5))
    {
        setPinFun(_pin,SECOND_FUNCTION);
    }
    else
    {
        setPinFun(_pin,FIRST_FUNCTION);
    }
    #elif defined(Arduino_h) || defined(_ARDUINO_H_)
    // activate internal PULLUP resistor.
    if (pullupActive) {
        pinMode(_pin, INPUT_PULLUP);
    } else {
        pinMode(_pin, INPUT);
    } // if
    #elif defined(__CH57x_COMM_H__)
    // activate internal PULLUP resistor.
    if (pullupActive) {
        pinMode(_pin, GPIO_ModeIN_PU);
    } else {
        pinMode(_pin, GPIO_ModeIN_Floating);
    } // if
    #else
    // activate internal PULLUP resistor.
    if (pullupActive) {
        pinMode(_pin, GPIO_Mode_IPU);
    } else {
        pinMode(_pin, GPIO_Mode_IN_FLOATING);
    } // if
    #endif
} // setDebounceTicks



// explicitly set the number of millisec that have to pass by before a click is assumed stable.
void OneButton::setDebounceTicks(const int ticks)
{
  _debounceTicks = ticks;
} // setDebounceTicks


// explicitly set the number of millisec that have to pass by before a click is detected.
void OneButton::setClickTicks(const int ticks)
{
  _clickTicks = ticks;
} // setClickTicks


// explicitly set the number of millisec that have to pass by before a long button press is detected.
void OneButton::setPressTicks(const int ticks)
{
  _pressTicks = ticks;
} // setPressTicks


// save function for click event
void OneButton::attachClick(callbackFunction newFunction)
{
  _clickFunc = newFunction;
} // attachClick


// save function for parameterized click event
void OneButton::attachClick(parameterizedCallbackFunction newFunction, void *parameter)
{
  _paramClickFunc = newFunction;
  _clickFuncParam = parameter;
} // attachClick


// save function for doubleClick event
void OneButton::attachDoubleClick(callbackFunction newFunction)
{
  _doubleClickFunc = newFunction;
  #if defined(__ASR_H__) || defined(__CH57x_COMM_H__)
  _maxClicks = fmax(_maxClicks, 2);
  #else
  _maxClicks = max(_maxClicks, 2);
  #endif
} // attachDoubleClick


// save function for parameterized doubleClick event
void OneButton::attachDoubleClick(parameterizedCallbackFunction newFunction, void *parameter)
{
  _paramDoubleClickFunc = newFunction;
  _doubleClickFuncParam = parameter;
  #if defined(__ASR_H__) || defined(__CH57x_COMM_H__)
  _maxClicks = fmax(_maxClicks, 2);
  #else
  _maxClicks = max(_maxClicks, 2);
  #endif
} // attachDoubleClick


// save function for multiClick event
void OneButton::attachMultiClick(callbackFunction newFunction)
{
  _multiClickFunc = newFunction;
  #if defined(__ASR_H__) || defined(__CH57x_COMM_H__)
  _maxClicks = fmax(_maxClicks, 100);
  #else
  _maxClicks = max(_maxClicks, 100);
  #endif
} // attachMultiClick


// save function for parameterized MultiClick event
void OneButton::attachMultiClick(parameterizedCallbackFunction newFunction, void *parameter)
{
  _paramMultiClickFunc = newFunction;
  _multiClickFuncParam = parameter;
  #if defined(__ASR_H__) || defined(__CH57x_COMM_H__)
  _maxClicks = fmax(_maxClicks, 100);
  #else
  _maxClicks = max(_maxClicks, 100);
  #endif
} // attachMultiClick


// save function for longPressStart event
void OneButton::attachLongPressStart(callbackFunction newFunction)
{
  _longPressStartFunc = newFunction;
} // attachLongPressStart


// save function for parameterized longPressStart event
void OneButton::attachLongPressStart(parameterizedCallbackFunction newFunction, void *parameter)
{
  _paramLongPressStartFunc = newFunction;
  _longPressStartFuncParam = parameter;
} // attachLongPressStart


// save function for longPressStop event
void OneButton::attachLongPressStop(callbackFunction newFunction)
{
  _longPressStopFunc = newFunction;
} // attachLongPressStop


// save function for parameterized longPressStop event
void OneButton::attachLongPressStop(parameterizedCallbackFunction newFunction, void *parameter)
{
  _paramLongPressStopFunc = newFunction;
  _longPressStopFuncParam = parameter;
} // attachLongPressStop


// save function for during longPress event
void OneButton::attachDuringLongPress(callbackFunction newFunction)
{
  _duringLongPressFunc = newFunction;
} // attachDuringLongPress


// save function for parameterized during longPress event
void OneButton::attachDuringLongPress(parameterizedCallbackFunction newFunction, void *parameter)
{
  _paramDuringLongPressFunc = newFunction;
  _duringLongPressFuncParam = parameter;
} // attachDuringLongPress


void OneButton::reset(void)
{
  _state = OneButton::OCS_INIT;
  _lastState = OneButton::OCS_INIT;
  _nClicks = 0;
  _startTime = 0;
}


// ShaggyDog ---- return number of clicks in any case: single or multiple clicks
int OneButton::getNumberClicks(void)
{
  return _nClicks;
}


/**
 * @brief Check input of the configured pin and then advance the finite state
 * machine (FSM).
 */
void OneButton::tick(void)
{
  if (_pin >= 0) {
    tick(digitalRead(_pin) == _buttonPressed);
  }
}


/**
 *  @brief Advance to a new state and save the last one to come back in cas of bouncing detection.
 */
void OneButton::_newState(stateMachine_t nextState)
{
  _lastState = _state;
  _state = nextState;
} // _newState()


/**
 * @brief Run the finite state machine (FSM) using the given level.
 */
void OneButton::tick(bool activeLevel)
{
  unsigned long now = millis(); // current (relative) time in msecs.
  unsigned long waitTime = (now - _startTime);
  // Implementation of the state machine
  switch (_state) {
  case OneButton::OCS_INIT:
    // waiting for level to become active.
    if (activeLevel) {
      _newState(OneButton::OCS_DOWN);
      _startTime = now; // remember starting time
      _nClicks = 0;
    } // if
    break;

  case OneButton::OCS_DOWN:
    // waiting for level to become inactive.

    if ((!activeLevel) && (waitTime < _debounceTicks)) {
      // button was released to quickly so I assume some bouncing.
      _newState(_lastState);

    } else if (!activeLevel) {
      _newState(OneButton::OCS_UP);
      _startTime = now; // remember starting time

    } else if ((activeLevel) && (waitTime > _pressTicks)) {
      if (_longPressStartFunc) _longPressStartFunc();
      if (_paramLongPressStartFunc) _paramLongPressStartFunc(_longPressStartFuncParam);
      _newState(OneButton::OCS_PRESS);
    } // if
    break;

  case OneButton::OCS_UP:
    // level is inactive

    if ((activeLevel) && (waitTime < _debounceTicks)) {
      // button was pressed to quickly so I assume some bouncing.
      _newState(_lastState); // go back

    } else if (waitTime >= _debounceTicks) {
      // count as a short button down
      _nClicks++;
      _newState(OneButton::OCS_COUNT);
    } // if
    break;

  case OneButton::OCS_COUNT:
    // dobounce time is over, count clicks

    if (activeLevel) {
      // button is down again
      _newState(OneButton::OCS_DOWN);
      _startTime = now; // remember starting time

    } else if ((waitTime > _clickTicks) || (_nClicks == _maxClicks)) {
      // now we know how many clicks have been made.

      if (_nClicks == 1) {
        // this was 1 click only.
        if (_clickFunc) _clickFunc();
        if (_paramClickFunc) _paramClickFunc(_clickFuncParam);

      } else if (_nClicks == 2) {
        // this was a 2 click sequence.
        if (_doubleClickFunc) _doubleClickFunc();
        if (_paramDoubleClickFunc) _paramDoubleClickFunc(_doubleClickFuncParam);

      } else {
        // this was a multi click sequence.
        if (_multiClickFunc) _multiClickFunc();
        if (_paramMultiClickFunc) _paramMultiClickFunc(_multiClickFuncParam);
      } // if

      reset();
    } // if
    break;

  case OneButton::OCS_PRESS:
    // waiting for menu pin being release after long press.

    if (!activeLevel) {
      _newState(OneButton::OCS_PRESSEND);
      _startTime = now;

    } else {
      // still the button is pressed
      if (_duringLongPressFunc) _duringLongPressFunc();
      if (_paramDuringLongPressFunc) _paramDuringLongPressFunc(_duringLongPressFuncParam);
    } // if
    break;

  case OneButton::OCS_PRESSEND:
    // button was released.

    if ((activeLevel) && (waitTime < _debounceTicks)) {
      // button was released to quickly so I assume some bouncing.
      _newState(_lastState); // go back

    } else if (waitTime >= _debounceTicks) {
      if (_longPressStopFunc) _longPressStopFunc();
      if (_paramLongPressStopFunc) _paramLongPressStopFunc(_longPressStopFuncParam);
      reset();
    }
    break;

  default:
    // unknown state detected -> reset state machine
    _newState(OneButton::OCS_INIT);
    break;
  } // if

} // OneButton.tick()


// end.

#endif
