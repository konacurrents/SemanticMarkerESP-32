#include <iostream>
#include <string.h>
#include <stdio.h>
#include "PIRSensorClass.h"

PIRSensorClass::PIRSensorClass(char *config) : SensorClassType(config)
{

   printf("PIRSensorClass init %s\n", config);
}

//!@see https://www.w3schools.com/cpp/cpp_class_methods.asp
void PIRSensorClass::loop()
{
   printf("PIRSensorClass::loop()\n");
}

void PIRSensorClass::setup()
{
   printf("PIRSensorClass::setup()\n");
}
