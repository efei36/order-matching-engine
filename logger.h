#pragma once

#ifdef DO_DEBUG
#define LOG_DEBUG(message) std::cout << message << std::endl;
#else
#define LOG_DEBUG(message)
#endif