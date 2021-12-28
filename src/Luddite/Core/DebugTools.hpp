#pragma once
#include "Logging.hpp"

#define GET_TYPENAME_STRING(x) #x

#ifdef LD_DEBUG
#define LD_VERIFY(expression, ...) if (!(expression)) {LD_LOG_CRITICAL(__VA_ARGS__);}
// #define LD_VERIFY(expression) if (!expression) {LD_LOG_ERROR("Expression Failed: {}", #expression);}
#else
#define LD_VERIFY(expression, ...) do{}while (false)
//#define LD_VERIFY(expression) do{}while(false)
#endif //LD_DEBUG
