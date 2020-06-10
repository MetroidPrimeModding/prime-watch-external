#include "common.h"
#include <sys/types.h>
#include <cstdio>

#ifdef WIN32
#include <signal.h>
#include <windows.h>
#else
#include <sys/unistd.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/stat.h>

#endif

#include <iostream>
#include <unordered_map>
#include <chrono>
#include "json.hpp"

#ifdef WIN32
#   ifndef MSG_NOSIGNAL
#       define MSG_NOSIGNAL 0
#   endif
#endif

using namespace std;
using namespace nlohmann;

#define MESSAGEID_READ 0x0
#define MESSAGEID_DATA 0x1
#define MESSAGEID_QUIT 0x2

bool run = true;

void sighup(int sig) {
  cout << "SIGNAL " << sig << endl;
  run = false;
}

void handleClient(istream &in, ostream &o);

int main() {
  findAndAttachProcess();

  //Not sure what I'm doing wrong here
#ifndef WIN32 // Windows doesn't support SIGHUP
  signal(SIGHUP, sighup);
#endif
  signal(SIGINT, sighup);
  signal(SIGABRT, sighup);

  handleClient(::cin, ::cout);
  return 0;
}

void handleClient(istream &in, ostream &o) {
  auto time = chrono::system_clock::now().time_since_epoch().count();
  string filename = "logs/loads_" + to_string(time) + ".log";

  while (run) {
    // read request
    uint32_t size;
    uint32_t reqMsg;
    in.get(reinterpret_cast<char *>(&size), sizeof(size));
    in.get(reinterpret_cast<char *>(&reqMsg), sizeof(reqMsg));

    if (reqMsg == MESSAGEID_READ) {
      json json_message;

      string message = json_message.dump();
      if (message.length() == 0 || message == "null") {
        message = "{}";
      }
      uint32_t outSize = message.length();
      uint32_t outMsg = MESSAGEID_QUIT;
      o.write(reinterpret_cast<const char *>(&outSize), sizeof(outSize));
      o.write(reinterpret_cast<const char *>(&outMsg), sizeof(outMsg));
      o.write(message.c_str(), outSize);
    } else {
      // For now, just quit on all other messages
      uint32_t outSize = 0;
      uint32_t outMsg = MESSAGEID_QUIT;
      o.write(reinterpret_cast<const char *>(&outSize), sizeof(outSize));
      o.write(reinterpret_cast<const char *>(&outMsg), sizeof(outMsg));
    }

#ifdef WIN32
    Sleep(16);
#else
    usleep(16);
#endif
  }
}
