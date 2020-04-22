/*
  Copyright (c) 2020 SimpleHacks (simplehacks@github.com).  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "Arduino.h"
#include "wiring_conflict.h"


uint32_t AnalogWriteState::_AnalogWrite_Channel_to_nrfPin[AnalogWriteState::MAX_ANALOG_PWM_CHANNELS];
uint16_t AnalogWriteState::_AnalogWrite_nrfPin_to_Channel[AnalogWriteState::NRF_PIN_COUNT];
uint8_t  AnalogWriteState::_AnalogResolutionInBits = 8;

