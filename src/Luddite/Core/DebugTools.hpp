#pragma once
#include "Logging.hpp"

#ifdef LD_DEBUG
#define LD_VERIFY(expression, message) if (!expression) {LD_LOG_ERROR(#message);}
#define LD_VERIFY(expression) if (!expression) {LD_LOG_ERROR("Expression Failed: {}", #expression);}
#else
#define LD_VERIFY(expression, message) do{}while {false}
#define LD_VERIFY(expression) do{}while {false}
#endif //LD_DEBUG