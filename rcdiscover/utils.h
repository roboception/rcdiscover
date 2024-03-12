/*
 * rcdiscover - the network discovery tool for Roboception devices
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

#ifndef RCDISCOVER_UTILS_H
#define RCDISCOVER_UTILS_H

#include <cstdint>
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

  if (!s.empty())
  {
    if (s.front() == sep || s.back() == sep)
    {
      throw std::invalid_argument("strings starts or ends with separator");
    }
  }

  std::istringstream iss(s);
  for (uint32_t i = 0; i < n; ++i)
  {
    if (!std::getline(iss, result[i], sep))
    {
      throw std::out_of_range("n");
    }
  }

  std::string tmp;
  if (std::getline(iss, tmp, sep))
  {
    throw std::out_of_range("n");
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
                 [&base](const std::string& s) -> std::uint8_t
  {
    const auto v = std::stoul(s, nullptr, base);
    if (v > 255)
    {
      throw std::out_of_range("number is larger than 255");
    }
    return static_cast<std::uint8_t>(v);
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

template<std::size_t N> struct MinFittingType { };
template<> struct MinFittingType<1> { using type = std::uint8_t; };
template<> struct MinFittingType<2> { using type = std::uint16_t; };
template<> struct MinFittingType<3> { using type = std::uint32_t; };
template<> struct MinFittingType<4> { using type = std::uint32_t; };
template<> struct MinFittingType<5> { using type = std::uint64_t; };
template<> struct MinFittingType<6> { using type = std::uint64_t; };
template<> struct MinFittingType<7> { using type = std::uint64_t; };
template<> struct MinFittingType<8> { using type = std::uint64_t; };

template<std::size_t N>
typename MinFittingType<N>::type
byteArrayToInt(const std::array<std::uint8_t, N> &a)
{
  using ReturnType = typename MinFittingType<N>::type;
  ReturnType result{};
  for (std::size_t i = 0; i < N; ++i)
  {
    result |= (static_cast<ReturnType>(a[i]) << ((N - 1 - i) * 8));
  }
  return result;
}

inline bool wildcardMatch(std::string::const_iterator str_first,
                          std::string::const_iterator str_last,
                          std::string::const_iterator p_first,
                          std::string::const_iterator p_last)
{
  if (str_first == str_last && p_first == p_last)
  { return true; }

  if (str_first == str_last)
  {
    if (*p_first == '*')
    {
      // if there is no more character after * => match
      return std::next(p_first) == p_last;
    }
  }

  if (p_first == p_last)
  {
    return false;
  }

  if (*p_first == '?' || std::tolower(*p_first) == std::tolower(*str_first))
  {
    return wildcardMatch(std::next(str_first), str_last,
                         std::next(p_first), p_last);
  }

  if (*p_first == '*')
  {
    return wildcardMatch(std::next(str_first), str_last, p_first, p_last) ||
           wildcardMatch(str_first, str_last, std::next(p_first), p_last);
  }

  return false;
}

#endif // RCDISCOVER_UTILS_H
