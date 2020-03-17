/*
 * rcdiscover - the network discovery tool for Roboception devices
 *
 * Copyright (c) 2018 Roboception GmbH
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

#include "rcdiscover-cli/rcdiscover_discover.h"
#include "rcdiscover-cli/rcdiscover_reconnect.h"
#include "rcdiscover-cli/rcdiscover_reset.h"
#include "rcdiscover-cli/rcdiscover_force_ip.h"
#include "rcdiscover-cli/cli_utils.h"

#include <iostream>
#include <map>
#include <functional>

#ifdef WIN32
#include <winsock2.h>
#endif

struct Command
{
  std::string description;
  std::function<int(const std::string &, int, char **)> fun;
};

static const std::map<std::string, Command> commands =
    {
        {"ls", {"List available devices", runDiscover}},
        {"reconnect", {"Reconnect a device", runReconnect}},
        {"forceip", {"Temporarily set the IP of a device", runForceIP}},
        {"reset", {"Reset a device's parameters", runReset}}
    };

class WSA
{
  public:
    WSA()
    {
#ifdef WIN32
      WSADATA wsaData;
      WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
    }

    ~WSA()
    {
#ifdef WIN32
      ::WSACleanup();
#endif
    }
};

int main(int argc, char *argv[])
{
  WSA wsa;

  std::string help_string = std::string("Usage: ") +
      argv[0] + " [-h | --help] [--version] <command> [<args>]\n\n" +
      "Available commands are:\n";

  const int max_command_len = getMaxCommandLen(commands);
  for (const auto &cmd : commands)
  {
    const int min_padding = 3;
    const int padding =
        max_command_len + min_padding - static_cast<int>(cmd.first.length());
    help_string += "    " + cmd.first +
                   std::string(static_cast<std::size_t>(padding), ' ') +
                   cmd.second.description + "\n";
  }

  help_string += std::string("\n") +
      "If no command is given, 'ls' is assumed.\n" +
      "See '" + argv[0] + " <command> --help' for command-specific help.";

  auto command = commands.find("ls");

  int argc_subcommand = argc - 1;
  char **argv_subcommand = argv + 1;

  if (argc > 1)
  {
    const std::string argv_1(argv[1]);
    if (argv_1 == "--help" || argv_1 == "-h")
    {
      std::cout << help_string << std::endl;
      return 0;
    }
    if (argv_1 == "--version")
    {
      std::cout << PACKAGE_VERSION << std::endl;
      return 0;
    }

    if (!argv_1.empty() && argv_1[0] != '-')
    {
      command = commands.find(argv[1]);
      --argc_subcommand;
      ++argv_subcommand;
    }
  }

  if (command == commands.end())
  {
    std::cerr << "Invalid command\n";
    std::cerr << help_string << '\n';
    return 1;
  }

  std::string command_str;
  for (int i = 0; i < (argc - argc_subcommand); ++i)
  {
    if (!command_str.empty())
    { command_str += ' '; }
    command_str += argv[i];
  }

  return command->second.fun(command_str, argc_subcommand, argv_subcommand);
}
