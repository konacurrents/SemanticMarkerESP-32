#include "AudioModule.h"
#include "../../Defines.h"
#ifdef USE_AUDIO_MODULE

#include <driver/i2s.h>

#define PIN_CLK     0
#define PIN_DATA    34
#define READ_LEN    (2 * 256)
#define GAIN_FACTOR 3
uint8_t _BUFFER[READ_LEN] = {0};

int16_t *_adcBuffer = NULL;

#ifdef CALLBACK
void _callbackFunction;

//! call the callback..
void callCallback(int volume)
{
    if (_callbackFunction)
    {
        void (*callbackFunction)(int) = _callbackFunction;
        (*callbackFunction)(volume);
    }
}
#endif

//! the loud threshhold (after some testing)
int _loudThreshhold = 4000; //8000;
int _lastTimeMillis;

//!check signal
void showSignal_AudioModule()
{
  
    
    int y;
    int max = 0;
    for (int n = 0; n < 160; n++) {
        y = _adcBuffer[n] * GAIN_FACTOR;
        if (y > max)
            max = y;
    }
    
    int currentTimeMillis = millis();
    int timeDiff = currentTimeMillis - _lastTimeMillis;
    
    if (max > _loudThreshhold)
    {
 
        _lastTimeMillis = currentTimeMillis;

        SerialDebug.printf("** HIGH = %d - diffTime=%d\n", max, timeDiff);
    }
//    else
//        SerialDebug.printf("low  = %d\n", max);

}

//!check mic sound
void checkMicSound_AudioModule()
{
    size_t bytesread;
    i2s_read(I2S_NUM_0, (char *)_BUFFER, READ_LEN, &bytesread,
             (100 / portTICK_RATE_MS));
    _adcBuffer = (int16_t *)_BUFFER;
    showSignal_AudioModule();
    //vTaskDelay(100 / portTICK_RATE_MS);
}

//!initialize
void i2sInit_AudioModule()
{
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM),
        .sample_rate = 44100,
        .bits_per_sample =
        I2S_BITS_PER_SAMPLE_16BIT,  // is fixed at 12bit, stereo, MSB
            .channel_format = I2S_CHANNEL_FMT_ALL_RIGHT,
#if ESP_IDF_VERSION > ESP_IDF_VERSION_VAL(4, 1, 0)
            .communication_format = I2S_COMM_FORMAT_STAND_I2S,
#else
            .communication_format = I2S_COMM_FORMAT_I2S,
#endif
            .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
            .dma_buf_count    = 2,
            .dma_buf_len      = 128,
    };
    
    i2s_pin_config_t pin_config;
    
#if (ESP_IDF_VERSION > ESP_IDF_VERSION_VAL(4, 3, 0))
    pin_config.mck_io_num = I2S_PIN_NO_CHANGE;
#endif
    
    pin_config.bck_io_num   = I2S_PIN_NO_CHANGE;
    pin_config.ws_io_num    = PIN_CLK;
    pin_config.data_out_num = I2S_PIN_NO_CHANGE;
    pin_config.data_in_num  = PIN_DATA;
    
    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
    i2s_set_clk(I2S_NUM_0, 44100, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);
}

int _delayStartMillis_AudioModule;
int _delaySeconds_AudioModule;
//!init the timer
void startTimer_AudioModule()
{
    _delayStartMillis_AudioModule = millis();   // start delay
    _delaySeconds_AudioModule = 1 * portTICK_RATE_MS;
}

//!see if the times up
boolean timesUp_AudioModule()
{
    return true;
    
    boolean timesUp = false;
    int currentTimeMillis = millis();
    //!substract the seconds from the set delay
    int currentCounterSeconds = _delaySeconds_AudioModule - (currentTimeMillis - _delayStartMillis_AudioModule)/1000;
    
    SerialLots.printf("delay = %d, counter = %d, delaySeconds = %d\n", _delayStartMillis_AudioModule, currentCounterSeconds, _delaySeconds_AudioModule);
    if (currentCounterSeconds <= 0)
    {
        SerialLots.printf("delayFinished_AudioModule\n");
        
        startTimer_AudioModule();
        timesUp = true;
    }
    return timesUp;
}

//!setup .. 
//void setup_AudioModule(void (*loudCallback)(int))
void setup_AudioModule()
{
    //_callbackFunction = loudCallback;
    //!initialize
    i2sInit_AudioModule();
    
    //!start timer
    startTimer_AudioModule();
}

//!loop if times up then check the mic sound
void loop_AudioModule()
{
    if (timesUp_AudioModule())
    {
        checkMicSound_AudioModule();
        
        //try checking mic again..
//        delay(100 / portTICK_RATE_MS);
//        checkMicSound_AudioModule();

    }
}

#endif

