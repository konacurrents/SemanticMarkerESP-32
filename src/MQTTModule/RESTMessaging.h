
#ifndef RESTMessaging_h
#define RESTMessaging_h
#include "../../Defines.h"

#ifdef USE_REST_MESSAGING

//! setup the connection to SemanticMarker.org
void setupSecureRESTCall();

//! these are sent to SemanticMarker.org and use that hosts security
void sendSecureRESTCall(String message);
#endif

#endif //REST_MESSAGING_h
