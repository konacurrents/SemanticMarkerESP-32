#include "CameraModule.h"
#ifdef USE_CAMERA_MODULE

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

//! setup
void setup_CameraModule()
{
    
    //! config of camera
    camera_config_t cameraConfig;
    
    //disable brownout detector
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
    
    cameraConfig.ledc_channel = LEDC_CHANNEL_0;
    cameraConfig.ledc_timer = LEDC_TIMER_0;
    cameraConfig.pin_d0 = Y2_GPIO_NUM;
    cameraConfig.pin_d1 = Y3_GPIO_NUM;
    cameraConfig.pin_d2 = Y4_GPIO_NUM;
    cameraConfig.pin_d3 = Y5_GPIO_NUM;
    cameraConfig.pin_d4 = Y6_GPIO_NUM;
    cameraConfig.pin_d5 = Y7_GPIO_NUM;
    cameraConfig.pin_d6 = Y8_GPIO_NUM;
    cameraConfig.pin_d7 = Y9_GPIO_NUM;
    cameraConfig.pin_xclk = XCLK_GPIO_NUM;
    cameraConfig.pin_pclk = PCLK_GPIO_NUM;
    cameraConfig.pin_vsync = VSYNC_GPIO_NUM;
    cameraConfig.pin_href = HREF_GPIO_NUM;
    cameraConfig.pin_sscb_sda = SIOD_GPIO_NUM;
    cameraConfig.pin_sscb_scl = SIOC_GPIO_NUM;
    cameraConfig.pin_pwdn = PWDN_GPIO_NUM;
    cameraConfig.pin_reset = RESET_GPIO_NUM;
    cameraConfig.xclk_freq_hz = 20000000;
    cameraConfig.pixel_format = PIXFORMAT_JPEG; //YUV422,GRAYSCALE,RGB565,JPEG
    
    if(psramFound())
    {
        SerialTemp.println("psramFound");
        cameraConfig.frame_size =  FRAMESIZE_UXGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
                                                   //QQVGA-UXGA Do not use sizes above QVGA when not JPEG
        
        cameraConfig.jpeg_quality = 12; //0-63 lower number means higher quality
        cameraConfig.fb_count = 2; //if more than one, i2s runs in continuous mode. Use only with JPEG
    }
    else
    {
        SerialTemp.println("psram NOT Found");
        
        cameraConfig.frame_size = FRAMESIZE_SVGA;
        cameraConfig.jpeg_quality = 12;
        cameraConfig.fb_count = 1;
    }
    
    // Init Camera
    esp_err_t err = esp_camera_init(&cameraConfig);
    if (err != ESP_OK)
    {
        SerialError.printf("Camera init failed with error 0x%x", err);
        return;
    }
    SerialTemp.println("*** Camera Initialized ****");
    
    //!try initializing each time..
    initCameraSensor();
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
    s->set_framesize(s, FRAMESIZE_UXGA);
    
    SerialTemp.println("*** Camera Sensor Initialized ****");
}

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
}

//!@see https://github.com/m5stack/M5Stack-Camera
//!take a picture
void takePicture_CameraModule()
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
    
#ifdef USE_MQTT_NETWORKING
    publishBinaryFile((char*)"usersP/bark/images", cameraFB->buf, cameraFB->len);
#endif
    //!close it up
    esp_camera_fb_return(cameraFB);
    
    //!turn off light
    digitalWrite(2, LOW);
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

#endif  //use camera module
