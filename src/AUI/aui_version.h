#pragma once

#include "aui_config.h"

#define AUI_VERSION_MAJOR            0
#define AUI_VERSION_MINOR            3
#define AUI_VERSION_REVISION         14

// Freeze-Version: 0 = kein Freeze
// Beispiel: 100 = freeze 1.0, 150 = freeze 1.5, 200 = freeze 2.0
#define AUI_VERSION_API_FREEZE       0

#ifndef AUI_VERSION_EXTNAME
#define AUI_VERSION_EXTNAME           "dev"
#endif



#define AUI_STRINGIFY2(x) #x
#define AUI_STRINGIFY(x)  AUI_STRINGIFY2(x)

#define AUI_FREEZE_MAJOR   (AUI_VERSION_API_FREEZE / 100)
#define AUI_FREEZE_MINOR   ((AUI_VERSION_API_FREEZE % 100) / 10)

#if AUI_VERSION_API_FREEZE == 0
    #define AUI_FREEZE_STRING ""
#else
    #define AUI_FREEZE_STRING \
        "api" \
        AUI_STRINGIFY(AUI_FREEZE_MAJOR) "." \
        AUI_STRINGIFY(AUI_FREEZE_MINOR)
#endif

#define AUI_VERSION_STRING \
    AUI_STRINGIFY(AUI_VERSION_MAJOR) "." \
    AUI_STRINGIFY(AUI_VERSION_MINOR) "." \
    AUI_STRINGIFY(AUI_VERSION_REVISION) \
    AUI_FREEZE_STRING "-" \
    AUI_VERSION_EXTNAME


