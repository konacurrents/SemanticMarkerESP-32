#pragma once
#include "../../../Defines.h"

#ifdef USE_CAMERA_MODULE

#ifdef ARDUINO_ARCH_ESP32
#include "platglue-esp32.h"
#else
#include "platglue-posix.h"
#endif

#endif
