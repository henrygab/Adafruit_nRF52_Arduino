
#pragma once


#include "Arduino.h"
#include "HardwarePWM.h"

#define WIRING_CONFLICT_STATIC_INLINE static inline __attribute__((always_inline))
// #define WIRING_CONFLICT_STATIC_INLINE static inline __attribute__((noinline))


class AnalogWriteState {
private:
  static const uint16_t MAX_ANALOG_PWM_CHANNELS = HWPWM_MODULE_NUM * HardwarePWM::MAX_CHANNELS_PER_PWM_INSTANCE;
  #ifdef NRF52832_XXAA
  static const uint16_t NRF_PIN_COUNT = 32;
  #elif defined(NRF52840_XXAA)
  static const uint16_t NRF_PIN_COUNT = 40;
  #else
  #error Unsupported MCU
  #endif

  // NOTE: Static class members must be initialized in a single translation unit,
  //       which typically means outside of the class body, in a separate C++ file

  // reading the PWM registers can take 4+ cycles (AHB @ 16MHz)
  // Therefore, keep two arrays that map between PWM channels
  // and nRF port/pin, to optimize lookup speed.
  // These array use the NRF Port.Pin number, not arduino pin number.
  // So, base the map on the number of ports available in the chip,
  // not based on the number of arduino pins defined.
  static uint32_t _AnalogWrite_Channel_to_nrfPin[AnalogWriteState::MAX_ANALOG_PWM_CHANNELS];
  static uint16_t _AnalogWrite_nrfPin_to_Channel[AnalogWriteState::NRF_PIN_COUNT];
  // Cache the last-set analog resolution, so when adding a new analog PWM item, it will be set to that last-cached value
  static uint8_t  _AnalogResolutionInBits;

  // TODO: add `constexpr` to below functions that could be `constexpr`

public: // these functions are to be used only by `analogWrite()`, `digitalWrite()`, `pinMode()`,  and the like
  constexpr static uint16_t INVALID_CHANNEL_INDEX = 0xFFFFu;

  WIRING_CONFLICT_STATIC_INLINE uint8_t get_AnalogResolutionInBits(void) { return AnalogWriteState::_AnalogResolutionInBits; }
  WIRING_CONFLICT_STATIC_INLINE void    set_AnalogResolutionInBits(uint8_t v) { AnalogWriteState::_AnalogResolutionInBits = v;    }
  WIRING_CONFLICT_STATIC_INLINE HardwarePWM*  ChannelIndexToHwPWM(uint16_t index) {
    if (index >= AnalogWriteState::MAX_ANALOG_PWM_CHANNELS) return nullptr;
    switch (index / HardwarePWM::MAX_CHANNELS_PER_PWM_INSTANCE) {
      case 0:
        return &HwPWM0;
      case 1:
        return &HwPWM1;
      case 2:
        return &HwPWM2;

      #if HWPWM_MODULE_NUM > 3
      case 3:
        return &HwPWM3;
      #endif // HWPWM_MODULE_NUM > 3
    }
    return nullptr;
  }
  WIRING_CONFLICT_STATIC_INLINE uint16_t IndexToChannel(uint16_t index) {
    if (index >= AnalogWriteState::MAX_ANALOG_PWM_CHANNELS) return AnalogWriteState::INVALID_CHANNEL_INDEX; // try to crash if used as index?
    return index % HardwarePWM::MAX_CHANNELS_PER_PWM_INSTANCE;
  }
  WIRING_CONFLICT_STATIC_INLINE uint16_t ArduinoPinToPwmChannel(uint32_t arduinoPin) {
    if (arduinoPin >= PINS_COUNT) return AnalogWriteState::INVALID_CHANNEL_INDEX;

    // the tracking array uses nRF Port.Pin, not arduino...
    uint32_t nrfPin = g_ADigitalPinMap[arduinoPin];

    // this should never be true, but cannot static_assert() ....
    if (nrfPin >= AnalogWriteState::NRF_PIN_COUNT) return AnalogWriteState::INVALID_CHANNEL_INDEX;

    // was the pin assigned to a PWM controller by analogWrite()?
    return _AnalogWrite_nrfPin_to_Channel[nrfPin];
  }

public: // this function is to be called exactly once during initialization of the platform, by the BSP (e.g., in `main()`)
  static void Initialize(void);
};

inline __attribute__((always_inline)) void AnalogWriteState::Initialize(void) {
  AnalogWriteState::_AnalogResolutionInBits = 8;
  for (size_t i = 0; i < AnalogWriteState::MAX_ANALOG_PWM_CHANNELS; i++) {
    AnalogWriteState::_AnalogWrite_Channel_to_nrfPin[i] = static_cast<uint32_t>(-1);
  }
  for (size_t i = 0; i < AnalogWriteState::NRF_PIN_COUNT; i++) {
    AnalogWriteState::_AnalogWrite_nrfPin_to_Channel[i] = AnalogWriteState::INVALID_CHANNEL_INDEX;
  }
}

