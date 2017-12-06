/*
 * rcdiscover - the network discovery tool for rc_visard
 *
 * Copyright (c) 2017 Roboception GmbH
 * All rights reserved
 *
 * Author: Raphael Schaller
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef UTILS_H
#define UTILS_H

#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <array>

inline std::string mac2string(const uint64_t mac)
{
  std::ostringstream out;

  out << std::hex << std::setfill('0');
  out << std::setw(2) << ((mac>>40)&0xff) << ':' << std::setw(2) << ((mac>>32)&0xff) << ':'
      << std::setw(2) << ((mac>>24)&0xff) << ':' << std::setw(2) << ((mac>>16)&0xff) << ':'
      << std::setw(2) << ((mac>>8)&0xff) << ':' << std::setw(2) << (mac&0xff);

  return out.str();
}

inline std::string ip2string(const uint32_t ip)
{
  std::ostringstream out;

  out << ((ip>>24)&0xff) << '.' << ((ip>>16)&0xff) << '.'
      << ((ip>>8)&0xff) << '.' << (ip&0xff);

  return out.str();
}

template<uint32_t n>
std::array<std::string, n> split(const std::string& s, const char sep)
{
  std::array<std::string, n> result;

  std::istringstream iss(s);
  for (uint32_t i = 0; i < n; ++i)
  {
    if (!std::getline(iss, result[i], sep))
    {
      throw std::out_of_range("n");
    }
  }

  return result;
}

template<uint32_t n>
std::array<uint8_t, n> string2byte(const std::string& s,
                                   const int base,
                                   const char sep)
{
  const auto splitted = split<n>(s, sep);

  std::array<uint8_t, n> result;

  std::transform(std::begin(splitted),
                 std::end(splitted),
                 std::begin(result),
                 [&base](const std::string& s)
  {
    return std::stoul(s, nullptr, base);
  });

  return result;
}

inline std::array<uint8_t, 6> string2mac(const std::string& mac)
{
  return string2byte<6>(mac, 16, ':');
}

inline std::array<uint8_t, 4> string2ip(const std::string& ip)
{
  return string2byte<4>(ip, 10, '.');
}

#endif // UTILS_H
