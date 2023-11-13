//
//  OTAImageUpdate.h
//  M5Stick
//
//  Created by Scott Moody on 3/8/22.
//

#ifndef OTAImageUpdate_h
#define OTAImageUpdate_h
#include "../../Defines.h"

#ifdef USE_MQTT_NETWORKING

//!connects to host and grabs the http file and tries to update the binary (OTA)
void performOTAUpdate(char *hostname, char *httpAddress);

//!retrieves from constant location
void performOTAUpdateSimple();

#endif

#endif /* OTAImageUpdate_h */
