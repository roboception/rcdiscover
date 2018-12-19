/*
* Roboception GmbH
* Munich, Germany
* www.roboception.com
*
* Copyright (c) 2017 Roboception GmbH
* All rights reserved
*
* Author: Raphael Schaller
*/
#include "gige_request_counter.h"

#include <atomic>
#include <limits>

namespace rcdiscover
{

std::tuple<std::uint8_t, std::uint8_t> GigERequestCounter::getNext()
{
  static std::atomic_uint_fast16_t counter(0);

  std::uint_fast16_t new_value;
  std::uint_fast16_t current;
  do
  {
    current = counter;
    if (current == std::numeric_limits<std::uint16_t>::max())
    {
      // skip 0
      new_value = 1;
    }
    else
    {
      new_value = static_cast<std::uint_fast16_t>(current + 1);
    }
  }
  while (!counter.compare_exchange_weak(current, new_value));

  return std::make_tuple(static_cast<std::uint8_t>(new_value >> 8),
                         static_cast<std::uint8_t>(new_value));
}

}
