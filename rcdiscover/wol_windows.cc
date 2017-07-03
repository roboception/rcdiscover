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

#include "wol_windows.h"

#include "socket_windows.h"
#include "wol_exception.h"
#include "operation_not_permitted.h"

#include <ios>
#include <string>
#include <string.h>
#include <vector>
#include <iostream>
#include <sstream>

#include <iphlpapi.h>

namespace rcdiscover
{

WOL_Windows::WOL_Windows(uint64_t hardware_addr, uint16_t port) :
  WOL(hardware_addr, port),
  broadcast_addr_(inet_addr("255.255.255.255"))
{ }

WOL_Windows::WOL_Windows(std::array<uint8_t, 6> hardware_addr, uint16_t port) :
  WOL(std::move(hardware_addr), port),
  broadcast_addr_(inet_addr("255.255.255.255"))
{ }

// #define UDP_CHECKSUM_POS 6

// void computeUdpChecksum(std::vector<uint8_t> &sendbuf, DWORD srcAddress, DWORD dstAddress)
// {
//
//   // TODO change!
//
// 	WORD *buf = (WORD *)payload;
// 	uint16_t length = payloadSize;
// 	WORD *src = (WORD *)&srcAddress, *dst = (WORD *)&dstAddress;
// 	DWORD checksum;
//
// 	*(WORD *)&payload[UDP_CHECKSUM_POS] = 0;
//
// 	checksum = 0;
// 	while (length > 1)
// 	{
// 		checksum += *buf++;
// 		if (checksum & 0x80000000)
// 			checksum = (checksum & 0xFFFF) + (checksum >> 16);
// 		length -= 2;
// 	}
//
// 	if (length & 1)
// 		checksum += *((unsigned char *)buf);
//
// 	checksum += *(src++);
// 	checksum += *src;
//
// 	checksum += *(dst++);
// 	checksum += *dst;
//
// 	checksum += htons(IPPROTO_UDP);
// 	checksum += htons(payloadSize);
//
// 	while (checksum >> 16)
// 		checksum = (checksum & 0xFFFF) + (checksum >> 16);
//
// 	*(WORD *)&payload[UDP_CHECKSUM_POS] = (WORD)(~checksum);
// }

void WOL_Windows::sendUdpImpl(const std::array<uint8_t, 4> *password) const
{
  auto sockets = SocketWindows::createAndBindForAllInterfaces();

  for (auto &socket : sockets)
  {
    sockaddr_in dst_addr;
    dst_addr.sin_family = AF_INET;
    dst_addr.sin_port = htons(getPort());
    dst_addr.sin_addr.s_addr = broadcast_addr_;

    std::vector<uint8_t> sendbuf;
    appendMagicPacket(sendbuf, password);

    socket.enableBroadcast();
    socket.sendto(sendbuf, dst_addr);
  }
}

}
