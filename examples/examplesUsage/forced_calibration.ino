#include <Arduino.h>
#include <SensirionI2CScd4x.h>
#include <Sensor_SCD41.h>

/*****************************************************************************
 * пример использования библиотеки для работы датчика в режиме               *
 * принудительной калибровки                                                  *
 *****************************************************************************/

Sensor_SCD41 scd41; 

uint16_t scd41_co2;
float scd41_temp;
float scd41_hum;
//unsigned long long start_tm;

void setup()
{
  Wire.begin();
  Serial.begin(115200);

  while (!Serial) 
    delay(100);

  // инициализация SCD41 в режиме периодического измерения (каждые 5 секунд)
  if (scd41.init(Wire, SCD41_manual_calibration, true)) // процесс калибровки будет выводиться в консоль Serial 
    Serial.println("SCD41 - OK");
  else
    Serial.println("SCD41 - failed, check wires!");
}

void loop()
{ 
}