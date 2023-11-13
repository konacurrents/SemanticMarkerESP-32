
#include "SMARTButton.h"
#ifdef PROCESS_SMART_BUTTON_JSON


char *_SMARTButton = NULL;
void runSMARTButton(char *SMAddress)
{
    //todo .. create an array .. but for now just run it..
}

//!@see https://arduinojson.org/v6/doc/deserialization/
//!
//!process the JSON message (looking for FEED, etc). Note: topic can be nil, or if not, it's an MQTT topic (so send replies if you want)
boolean processJSONSMARTButton(DynamicJsonDocument smartJSONObject)
{
    SerialTemp.printf("processJSONSMARTButton: ");
    serializeJsonPretty(smartJSONObject, Serial);
    SerialTemp.println("");
    
    StaticJsonDocument<2024>  flows;
    DynamicJsonDocument smartButtonObject(1024);
//    deserializeJson(doc1, ascii);
//    SerialTemp.printf("step1\n");

    const char *dev = smartJSONObject["dev"];
    SerialTemp.printf("dev = %s\n", dev?dev:"NULL");
    
    deserializeJson(smartButtonObject, smartJSONObject["SMARTButton"]);
    
    serializeJsonPretty(smartButtonObject, Serial);
    SerialTemp.println("");

    
    SerialTemp.printf("step2\n");

    //NEW: 3.28.22 {'cmd':COMMANDS}
    // {'sm':<sm>}
    // {'guest':<passws>
    //{ 'set':<object>,'val':<value>}
    const char* name = smartButtonObject["name"];
    const char* uuid = smartButtonObject["uuid"];
    const char* flowNum = smartButtonObject["flowNum"];
    SerialTemp.printf("processJSONSMARTButton: %s, %s, %s\n", name?name:"NULL", uuid?uuid:"NULL", flowNum?flowNum:"NULL");

    deserializeJson(flows, smartButtonObject["flow"]);
    
    SerialTemp.printf("step3\n");

    
    return true;

}

#endif
