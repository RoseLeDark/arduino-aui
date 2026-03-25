#pragma once

#include "aui_fast_type.h"

#define AUI_TRUE             1
#define AUI_FALSE            0

#define private_static private 


template <typename T>
struct value2size { enum { size = sizeof(T) * 8 }; };

using aui_int8_t           = aui_fast_type<value2size<int8_t>::size,           int8_t>;
using aui_int16_t          = aui_fast_type<value2size<int16_t>::size,          int16_t>;
using aui_int32_t          = aui_fast_type<value2size<int32_t>::size,          int32_t>;

using aui_uint8_t          = aui_fast_type<value2size<uint8_t>::size,          uint8_t>;
using aui_uint16_t         = aui_fast_type<value2size<uint16_t>::size,         uint16_t>;
using aui_uint32_t         = aui_fast_type<value2size<uint32_t>::size,         uint32_t>;

using auier_t              = uint8_t;
using aui_bool_t           = uint8_t;