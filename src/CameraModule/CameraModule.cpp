#include "CameraModule.h"
#ifdef USE_CAMERA_MODULE



//! 1.20.24 try to merge the RTSP server (not Web yet)
//!Stream Link: rtsp://192.168.0.221:8554/mjpeg/1
#ifdef USE_MQTT_NETWORKING
#else
//! if not MQTT then try the RTSPServer
#define ENABLE_RTSPSERVER
#endif

#define ENABLE_RTSPSERVER

#ifdef ENABLE_RTSPSERVER

//#include "src/SimStreamer.h"
#include "src/OV2640Streamer.h"
#include "src/CRtspSession.h"

OV2640 _camera;

#endif //ENABLE_RTSPSERVER


#ifdef ENABLE_RTSPSERVER
WiFiServer _rtspServer(8554);
#endif  //ENABLE_RTSPSERVER


//! Users/scott/Library/Arduino15/packages/m5stack/hardware/esp32/2.0.3/tools/sdk
#include "esp_camera.h"

#include "FS.h"                // SD Card ESP32
#include "soc/soc.h"           // Disable brownour problems
#include "soc/rtc_cntl_reg.h"  // Disable brownour problems

// define the number of bytes you want to access
#define EEPROM_SIZE 1

// Pin definition for CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    15
#define XCLK_GPIO_NUM     27
#define SIOD_GPIO_NUM     25
#define SIOC_GPIO_NUM     23


#define Y9_GPIO_NUM       19
#define Y8_GPIO_NUM       36
#define Y7_GPIO_NUM       18
#define Y6_GPIO_NUM       39
#define Y5_GPIO_NUM        5
#define Y4_GPIO_NUM       34
#define Y3_GPIO_NUM       35
#define Y2_GPIO_NUM       32
#define VSYNC_GPIO_NUM    22
#define HREF_GPIO_NUM     26
#define PCLK_GPIO_NUM     21

#define CAMERA_LED_GPIO 2

#define BAT_OUTPUT_HOLD_PIN 33
#define BAT_ADC_PIN 38

#define Ext_PIN_1 4
#define Ext_PIN_2 13

int _pictureNumber = 0;


/**
 * @brief Data structure of camera frame buffer
 */
#ifdef JUST_COMMENT
typedef struct {
    uint8_t * buf;              /*!< Pointer to the pixel data */
    size_t len;                 /*!< Length of the buffer in bytes */
    size_t width;               /*!< Width of the buffer in pixels */
    size_t height;              /*!< Height of the buffer in pixels */
    pixformat_t format;         /*!< Format of the pixel data */
    struct timeval timestamp;   /*!< Timestamp since boot of the first DMA buffer of the frame */
} camera_fb_t;
#endif


//!! split this up..  Sure seems like the picture is taken right now!!
void initCameraSensor();

//! config of camera
camera_config_t _cameraConfig;

//! setup
void setup_CameraModule()
{
    
    //! config of camera
    // camera_config_t cameraConfig;
    
    //disable brownout detector
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
    
    _cameraConfig.ledc_channel = LEDC_CHANNEL_0;
    _cameraConfig.ledc_timer = LEDC_TIMER_0;
    _cameraConfig.pin_d0 = Y2_GPIO_NUM;
    _cameraConfig.pin_d1 = Y3_GPIO_NUM;
    _cameraConfig.pin_d2 = Y4_GPIO_NUM;
    _cameraConfig.pin_d3 = Y5_GPIO_NUM;
    _cameraConfig.pin_d4 = Y6_GPIO_NUM;
    _cameraConfig.pin_d5 = Y7_GPIO_NUM;
    _cameraConfig.pin_d6 = Y8_GPIO_NUM;
    _cameraConfig.pin_d7 = Y9_GPIO_NUM;
    _cameraConfig.pin_xclk = XCLK_GPIO_NUM;
    _cameraConfig.pin_pclk = PCLK_GPIO_NUM;
    _cameraConfig.pin_vsync = VSYNC_GPIO_NUM;
    _cameraConfig.pin_href = HREF_GPIO_NUM;
    _cameraConfig.pin_sscb_sda = SIOD_GPIO_NUM;
    _cameraConfig.pin_sscb_scl = SIOC_GPIO_NUM;
    _cameraConfig.pin_pwdn = PWDN_GPIO_NUM;
    _cameraConfig.pin_reset = RESET_GPIO_NUM;
    _cameraConfig.xclk_freq_hz = 20000000;
    _cameraConfig.pixel_format = PIXFORMAT_JPEG; //YUV422,GRAYSCALE,RGB565,JPEG
    
    if(psramFound())
    {
        SerialTemp.println("psramFound");
        
        _cameraConfig.frame_size =  FRAMESIZE_UXGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
                                                    //QQVGA-UXGA Do not use sizes above QVGA when not JPEG
        
        _cameraConfig.jpeg_quality = 12; //0-63 lower number means higher quality
        _cameraConfig.fb_count = 2; //if more than one, i2s runs in continuous mode. Use only with JPEG
    }
    else
    {
        SerialTemp.println("psram NOT Found");
        
        _cameraConfig.frame_size = FRAMESIZE_SVGA;
        _cameraConfig.jpeg_quality = 12;
        _cameraConfig.fb_count = 1;
    }
    
    // Init Camera
    esp_err_t err = esp_camera_init(&_cameraConfig);
    if (err != ESP_OK)
    {
        SerialError.printf("Camera init failed with error 0x%x", err);
        return;
    }
    SerialTemp.println("*** Camera Initialized ****");
    
    //!try initializing each time..
    initCameraSensor();
    
#ifdef ENABLE_RTSPSERVER
    {
        IPAddress ip;
        
        //! done in MQTTNetworking.cpp
        WiFi.mode(WIFI_STA);
        WiFi.begin("SunnyWhiteriver", "sunny2021");
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            SerialDebug.print(F("."));
        }
        ip = WiFi.localIP();
        SerialDebug.println(F("WiFi connected"));
        SerialDebug.println("");
        SerialDebug.println(ip);
        SerialDebug.print("Stream Link: rtsp://");
        SerialDebug.print(ip);
        SerialDebug.println(":8554/mjpeg/1");
    }
#endif
    
#ifdef ENABLE_RTSPSERVER
    _rtspServer.begin();
#endif
}


//!! split this up..  Sure seems like the picture is taken right now!!
void initCameraSensor()
{

    sensor_t *s = esp_camera_sensor_get();
    //! initial sensors are flipped vertically and colors are a bit saturated
    s->set_vflip(s, 1);       // flip it back

    s->set_brightness(s, 1);  // up the blightness just a bit
    s->set_saturation(s, -2); // lower the saturation
    
    s->set_hmirror(s, 0); //?? horizontal mirror?

    // drop down frame size for higher initial frame rate
    //  s->set_framesize(s, FRAMESIZE_QVGA);
   // s->set_framesize(s, FRAMESIZE_UXGA);
    
    SerialTemp.println("*** Camera Sensor Initialized ****");
}

#ifdef ENABLE_RTSPSERVER
CStreamer *_rtsp_streamer;
CRtspSession *_rtsp_session;
WiFiClient _rtsp_client; // FIXME, support multiple clients

#endif

//!loop
void loop_CameraModule()
{
#ifdef ESP_M5_CAMERA
    // blink the light on the camera
  //  delay(100);
    //digitalWrite(2, HIGH);
    //delay(100);
 //   digitalWrite(2, LOW);
#endif

#ifdef ENABLE_RTSPSERVER
    uint32_t msecPerFrame = 100;
    static uint32_t lastimage = millis();
    
    // If we have an active _rtsp_client connection, just service that until gone
    // (FIXME - support multiple simultaneous _rtsp_clients)
    if(_rtsp_session)
    {
      //  SerialDebug.println("loop: _rtsp_session");
        // we don't use a timeout here,
        // instead we send only if we have new enough frames
        _rtsp_session->handleRequests(0);

        uint32_t now = millis();
        // handle clock rollover
        if(now > lastimage + msecPerFrame || now < lastimage)
        {
            _rtsp_session->broadcastCurrentFrame(now);
            lastimage = now;
            
            // check if we are overrunning our max frame rate
            now = millis();
            if(now > lastimage + msecPerFrame)
                SerialDebug.printf("warning exceeding max frame rate of %d ms\n", now - lastimage);
        }
        
        if(_rtsp_session->m_stopped) 
        {
            delete _rtsp_session;
            delete _rtsp_streamer;
            _rtsp_session = NULL;
            _rtsp_streamer = NULL;
        }
    }
    else
    {
        _rtsp_client = _rtspServer.accept();
        
        
        if(_rtsp_client) 
        {
            //_rtsp_streamer = new SimStreamer(&_rtsp_client, true);             // our _rtsp_streamer for UDP/TCP based RTP transport
            _rtsp_streamer = new OV2640Streamer(&_rtsp_client, _camera);             // our _rtsp_streamer for UDP/TCP based RTP transport
            
            _rtsp_session = new CRtspSession(&_rtsp_client, _rtsp_streamer); // our threads RTSP _rtsp_session and state
           // SerialDebug.printf("loop2: _rtsp_session: %d\n ", _rtsp_session);
        }
        else
        {
          //  SerialDebug.printf(" ** still no rtsp_client %d\n", _rtspServer);
        }
    }
    
#endif
}

//!@see https://github.com/m5stack/M5Stack-Camera
//!take a picture
void takePicture_CameraModule_internal(boolean publishBinary)
{
    //!try initializing each time..
 //   initCameraSensor();
    
    //!turn on light
    digitalWrite(2, HIGH);
    //! cameras frame buffer
    camera_fb_t *cameraFB = NULL;
    
    //! Take Picture with Camera
    cameraFB = esp_camera_fb_get();
    if(!cameraFB)
    {
        SerialError.println("Camera capture failed");
        return;
    }
    //we could use our EPROM for the next file name..
#ifdef M5_CAPTURE_SCREEN
    
    //for now save in SPIFF
    writeFB_SPIFFModule(cameraFB->buf,              /*!< Pointer to the pixel data */
                        cameraFB->len,                 /*!< Length of the buffer in bytes */
                        (char*)"/CameraPicture.jpg");
#endif
    
    //!turn off light
    digitalWrite(2, LOW);
    
#ifdef USE_MQTT_NETWORKING
    if (publishBinary)
        publishBinaryFile((char*)"usersP/bark/images", cameraFB->buf, cameraFB->len, "jpg");
#endif
    //!close it up
    esp_camera_fb_return(cameraFB);
    
}

//!@see https://github.com/m5stack/M5Stack-Camera
//!take a picture
void takePicture_CameraModule()
{
    //! take 2 pics, so the framebuffer of 2 is flushed
    //! 1.20.24
    takePicture_CameraModule_internal(false);
    delay(50);
//    takePicture_CameraModule_internal(false);
//    delay(50);
//    takePicture_CameraModule_internal(false);
//    delay(50);
    takePicture_CameraModule_internal(true);
    delay(50);

}

//!turn off on-board LED flash
void flashConfig_CameraModule(boolean turnOn)
{
    
    // Turns off the ESP32-CAM white on-board LED (flash) connected to GPIO 4
    pinMode(4, OUTPUT);
    digitalWrite(4, LOW);
    //?? rtc_gpio_hold_en(GPIO_NUM_4);
}

//!sleep the device
void sleep_CameraModule()
{
    SerialDebug.println("Going to sleep now");
    delay(2000);
    esp_deep_sleep_start();
}


//! good example:
//! https://randomnerdtutorials.com/esp32-cam-take-photo-save-microsd-card/



//! BUTTON PROCESSING abstraction
//!short press on buttonA (top button)
void buttonA_ShortPress_CameraModule()
{
    
}
//!long press on buttonA (top button)
void buttonA_LongPress_CameraModule()
{
    
}
#endif  //use camera module
