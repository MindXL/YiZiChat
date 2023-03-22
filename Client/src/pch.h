#pragma once

#include <string>
#include <string_view>

#include <iostream>
#include <thread>
#include <chrono>
using namespace std::chrono_literals;

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>

#include <YiZi/YiZi.h>

#include "Buffer.h"
#include "Socket.h"
#include "ConnectionHandler.h"
