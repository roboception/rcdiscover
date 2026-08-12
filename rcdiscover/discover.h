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

#ifndef RCDISCOVER_DISCOVER
#define RCDISCOVER_DISCOVER

#include "deviceinfo.h"
#include <string>

#ifdef WIN32
#include "socket_windows.h"
#else
#include "socket_linux.h"
#endif

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

namespace rcdiscover
{

class Discover
{
  public:
#ifdef WIN32
    typedef SocketWindows SocketType;
#else
    typedef SocketLinux SocketType;
#endif

  public:

    /**
      Initializes a socket ready for broadcasting requests.

      NOTE: Exceptions are thrown in case of severe network errors.
    */

    Discover();
    ~Discover();

    /**
      Broadcasts a discovery command request.
    */

    void broadcastRequest();
    void broadcastRequest(const std::vector<std::string> &iface);

    /**
      Returns a discovery response. This method should be called until there is
      no further response.

      @param info    Info object that will be filled if there is an available
                     response.
      @param timeout Timeout in Milliseconds.
      @return        True if there was a valid response. In this case, the info
                     object contains valid information. False in case of a
                     timeout.
    */

    bool getResponse(std::vector<DeviceInfo> &info, int timeout_per_socket=1000);

  private:
    /**
      Starts one persistent listener thread per socket, each continuously
      waiting for and collecting valid discovery responses into pending_
      until the object is destroyed. This avoids spawning a fresh batch of
      threads on every getResponse() call.
    */

    void startListening();
    void listenOnSocket(SocketType &socket);

    std::vector<SocketType> sockets_;
    std::vector<std::tuple<std::uint8_t, std::uint8_t>> req_nums_;

    std::vector<std::thread> listener_threads_;
    std::atomic<bool> stop_;

    std::mutex mutex_;
    std::condition_variable cv_;
    std::vector<DeviceInfo> pending_;
};

}

#endif
