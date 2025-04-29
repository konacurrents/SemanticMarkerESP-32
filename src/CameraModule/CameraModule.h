#ifndef CameraModule_h
#define CameraModule_h

#include "../../Defines.h"

#ifdef USE_CAMERA_MODULE

//! setup
void setup_CameraModule();

//!loop
void loop_CameraModule();

//!take a picture
void takePicture_CameraModule();



//! BUTTON PROCESSING abstraction
//!short press on buttonA (top button)
void buttonA_ShortPress_CameraModule();
//!long press on buttonA (top button)
void buttonA_LongPress_CameraModule();

#endif

#endif // CameraModule_h
