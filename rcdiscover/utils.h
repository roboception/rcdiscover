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

#ifndef UTILS_H
#define UTILS_H

#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <algorithm>

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
