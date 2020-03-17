/*
 * rcdiscover - the network discovery tool for Roboception devices
 *
 * Copyright (c) 2017 Roboception GmbH
 * All rights reserved
 *
 * Author: Heiko Hirschmueller
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

#include "deviceinfo.h"

#include <sstream>

namespace rcdiscover
{

namespace
{

/*
  Extract at most len bytes from p as characters and returns them as string.
  Extraction ends if a null byte is encountered or if len bytes have been
  extracted.

  @param p   Pointer to byte array.
  @param len Maximum number of bytes to extract.
  @return    Extracted string.
*/

std::string extract(const uint8_t *p, size_t len)
{
  std::ostringstream out;

  while (*p != 0 && len > 0)
  {
    out << static_cast<char>(*p);

    p++;
    len--;
  }

  return out.str();
}

}

DeviceInfo::DeviceInfo(std::string iface_name) :
    iface_name(std::move(iface_name))
{
  clear();
}

void DeviceInfo::set(const uint8_t *raw, size_t len)
{
  // clear stored information

  clear();

  // extract information

  if (len >= 4)
  {
    major=(static_cast<int>(raw[0])<<8)|raw[1];
    minor=(static_cast<int>(raw[2])<<8)|raw[3];
  }

  if (len >= 16)
  {
    mac=0;
    for (int i=0; i<6; i++) mac=(mac<<8)|raw[10+i];
  }

  if (len >= 40)
  {
    ip=0;
    for (int i=0; i<4; i++) ip=(ip<<8)|raw[36+i];
  }

  if (len >= 56)
  {
    subnet=0;
    for (int i=0; i<4; i++) subnet=(subnet<<8)|raw[52+i];
  }

  if (len >= 72)
  {
    gateway=0;
    for (int i=0; i<4; i++) gateway=(gateway<<8)|raw[68+i];
  }

  if (len >= 104) manufacturer_name=extract(raw+72, 32);
  if (len >= 136) model_name=extract(raw+104, 32);
  if (len >= 168) device_version=extract(raw+136, 32);
  if (len >= 216) manufacturer_info=extract(raw+168, 48);
  if (len >= 232) serial_number=extract(raw+216, 16);
  if (len >= 248) user_name=extract(raw+232, 16);
}

void DeviceInfo::clear()
{
  major=minor=0;
  mac=0;
  ip=0;
  subnet=0;
  gateway=0;

  manufacturer_name.erase();
  model_name.erase();
  device_version.erase();
  manufacturer_info.erase();
  serial_number.erase();
  user_name.erase();
}

}