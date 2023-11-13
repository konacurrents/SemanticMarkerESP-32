/**
*  \link OTAImageUpdate  \endlink
*/

//
//  OTAImageUpdate.c
//  M5Stick
//
//  Created by Scott Moody on 3/8/22.
//
//!Code from AWS_S3_OTA_Update, it only uses HTTP (not https)
//!https://www.tutorialspoint.com/esp32_for_iot/performing_the_over_the_air_update_of_esp32_firmware.htm
#include "OTAImageUpdate.h"
#ifdef USE_MQTT_NETWORKING

#include <WiFi.h>

#include <Update.h>

//the WiFi client for connecting to the website to grab the binary image.
WiFiClient _client;

// Variables to validate
// response from S3
long _contentLength = 0;
bool _isValidContentType = false;

// S3 Bucket Config
//!String _hostIP = "konacurrents.com"; // Host => bucket-name.s3.region.amazonaws.com
//! host IP address (cannot be https)
String _hostIP = "KnowledgeShark.org";
//! Non https. For HTTPS 443. As of today, HTTPS doesn't work.
int _port = 80;

#ifdef ESP_M5
//!location of bin file
String _binName = "/OTA/ESP_IOT.ino.m5stick_c_plus.bin"; // bin file name with a slash in front.
#endif
#ifdef ESP_32
#ifdef BOARD
//!location of bin file
String _binName = "/OTA/OTA_Board/ESP_IOT.ino.esp32.bin"; // bin file name with a slash in front.
#else
//!location of bin file
String _binName = "/OTA/ESP_IOT.ino.esp32.bin"; // bin file name with a slash in front.
#endif
#endif
//TODO note: this needs an ESP_M5 version and others ...

//! Utility to extract header value from headers
String getHeaderValue(String header, String headerName) {
  return header.substring(strlen(headerName.c_str()));
}

//#define JUST_TESTING

//! OTA Logic
void execOTA()
{
  SerialDebug.println("Connecting to: " + String(_hostIP) + ", bin = " + String(_binName));
#ifdef TRY_WITHOUT_THIS_MAYBE_OTHER_THREAD_GOOFS
#ifdef USE_MQTT_NETWORKING
    //note needs # or won't send. the {device} is tacked on..
    sendMessageMQTT((char*)"#UPDATING_OTA");
#endif
#endif

    //try this..  still talking some time..
    stopProcessesForOTAUpdate_mainModule();
#ifdef USE_DISPLAY_MODULE
    showOTAUpdatingMessage();
#endif

    
#ifdef JUST_TESTING
  return;
#endif
    //see if delay gets BLE out of it's funk.
    //https://esp32.com/viewtopic.php?t=10411
//https://arduino.stackexchange.com/questions/72876/task-watchdog-gets-triggered-during-bluetooth-initialization

  //! Connect to S3
  if (_client.connect(_hostIP.c_str(), _port)) {
    // Connection Succeed.
    // Fecthing the bin
    SerialDebug.println("Fetching Bin: " + String(_binName));

    //! Get the contents of the bin file
    _client.print(String("GET ") + _binName + " HTTP/1.1\r\n" +
                 "Host: " + _hostIP + "\r\n" +
                 "Cache-Control: no-cache\r\n" +
                 "Connection: close\r\n\r\n");

    // Check what is being sent
    //    SerialDebug.print(String("GET ") + _binName + " HTTP/1.1\r\n" +
    //                 "Host: " + _hostIP + "\r\n" +
    //                 "Cache-Control: no-cache\r\n" +
    //                 "Connection: close\r\n\r\n");

    unsigned long timeout = millis();
    while (_client.available() == 0) {
        //delay(1); 
      if (millis() - timeout > 5000) {
        SerialDebug.println("Client Timeout !");
        _client.stop();
        return;
      }
    }
    // Once the response is available,
    // check stuff

    /**
      Response Structure
      HTTP/1.1 200 OK
      x-amz-id-2: NVKxnU1aIQMmpGKhSwpCBh8y2JPbak18QLIfE+OiUDOos+7UftZKjtCFqrwsGOZRN5Zee0jpTd0=
      x-amz-request-id: 2D56B47560B764EC
      Date: Wed, 14 Jun 2017 03:33:59 GMT
      Last-Modified: Fri, 02 Jun 2017 14:50:11 GMT
      ETag: "d2afebbaaebc38cd669ce36727152af9"
      Accept-Ranges: bytes
      Content-Type: application/octet-stream
      Content-Length: 357280
      Server: AmazonS3

      {{BIN FILE CONTENTS}}

    */
    while (_client.available()) {
      // read line till /n
      String line = _client.readStringUntil('\n');
      // remove space, to check if the line is end of headers
      line.trim();

      // if the the line is empty,
      // this is end of headers
      // break the while and feed the
      // remaining `client` to the
      // Update.writeStream();
      if (!line.length()) {
        //headers ended
        break; // and get the OTA started
      }

      //! Check if the HTTP Response is 200
      //! else break and Exit Update
      if (line.startsWith("HTTP/1.1")) {
        if (line.indexOf("200") < 0) {
          SerialDebug.println("Got a non 200 status code from server. Exiting OTA Update.");
          break;
        }
      }

      //! extract headers here
      //! Start with content length
      if (line.startsWith("Content-Length: ")) {
        _contentLength = atol((getHeaderValue(line, "Content-Length: ")).c_str());
        SerialDebug.println("Content-Length: " + String(_contentLength) + " bytes from server");
      }

      //! Next, the content type
      if (line.startsWith("Content-Type: ")) {
        String contentType = getHeaderValue(line, "Content-Type: ");
        SerialDebug.println("Got " + contentType + " payload.");
        if (contentType == "application/octet-stream") {
          _isValidContentType = true;
        }
      }
    }
  } else {
    // Connect to S3 failed
    // May be try?
    // Probably a choppy network?
    SerialDebug.println("Connection to " + String(_hostIP) + " failed. Please check your setup");
    // retry??
    // execOTA();
  }

  // Check what is the contentLength and if content type is `application/octet-stream`
  SerialDebug.println("contentLength : " + String(_contentLength) + ", _isValidContentType : " + String(_isValidContentType));

  //! check contentLength and content type
  if (_contentLength && _isValidContentType) {
    //! Check if there is enough to OTA Update
    bool canBegin = Update.begin(_contentLength);

    // If yes, begin
    if (canBegin) {
        SerialDebug.println("Begin OTA streaming those bytes from the server..");
        SerialDebug.println("This may take 2 - 5 mins to complete. Things might be quiet for a while.. Patience!");
      // No activity would appear on the Serial monitor
      // So be patient. This may take 2 - 5mins to complete
      size_t written = Update.writeStream(_client);

      if (written == _contentLength) {
        SerialDebug.println("Written : " + String(written) + " successfully *****");
      } else {
        SerialDebug.println("******* ERROR: Written only : " + String(written) + "/" + String(_contentLength));
        // retry??
        // execOTA();
      }

      if (Update.end()) {
        SerialInfo.println("OTA done!");
        if (Update.isFinished()) {
          SerialDebug.println("Update successfully completed. Rebooting.");
            blinkMessageCallback((char*)"OTA Done");

            ESP.restart();
        } else {
            SerialDebug.println("Update not finished? Something went wrong!");
            for (int i=0;i<3;i++)
            {
                blinkMessageCallback((char*)"OTA error");
            }
        }
      } else {
          SerialDebug.println("Error Occurred. Error #: " + String(Update.getError()));
          for (int i=0;i<3;i++)
          {
              blinkMessageCallback((char*)"OTA error");
          }
      }
    } else {
      // not enough space to begin OTA
      // Understand the partitions and
      // space availability
      SerialError.println("**** Not enough space to begin OTA ** REPORT THIS TO DEVELOPERS");
      _client.flush();
    }
  } else {
    SerialError.println("There was no content in the response");
    _client.flush();
  }
    
    //!restart ..  which will ony effect if not rebooting..
   restartProcessesForOTAUpdate_mainModule();

}

//!address of bin string
char _binAddress[100];

//!connects to host and grabs the http file and tries to update the binary (OTA)
//! kind = esp32, m5
void performOTAUpdate(char *hostname, char *httpAddress)
{
    char *after = rindex(hostname,'/');
    after++;
   // hostname = &hostname[i+1];
  //not using kind
  _hostIP = after;
//strip http://
  sprintf(_binAddress, "/%s", httpAddress);
  _binName = _binAddress;

  execOTA();
}


//!retrieves from constant location
void performOTAUpdateSimple()
{
    SerialDebug.println("OTAImageUpdate.performOTAUpdateSimple");
    execOTA();
}

#endif // use MQTT
