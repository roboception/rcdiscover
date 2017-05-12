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

std::string mac2string(const uint64_t mac)
{
  std::ostringstream out;

  out << std::hex << std::setfill('0');
  out << std::setw(2) << ((mac>>40)&0xff) << ':' << std::setw(2) << ((mac>>32)&0xff) << ':'
      << std::setw(2) << ((mac>>24)&0xff) << ':' << std::setw(2) << ((mac>>16)&0xff) << ':'
      << std::setw(2) << ((mac>>8)&0xff) << ':' << std::setw(2) << (mac&0xff);

  return out.str();
}

std::string ip2string(const uint32_t ip)
{
  std::ostringstream out;

  out << ((ip>>24)&0xff) << '.' << ((ip>>16)&0xff) << '.'
      << ((ip>>8)&0xff) << '.' << (ip&0xff);

  return out.str();
}

#endif // UTILS_H
