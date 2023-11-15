#pragma once

#define attr(att) __attribute__((att))

#define ctor attr(constructor)
#define ret_addr(x) __builtin_return_address(x)