#include <Arduino.h>
#include <SensirionI2CScd4x.h>
#include <Sensor_SCD41.h>

Sensor_SCD41 scd41; 

uint16_t scd41_co2;
float scd41_temp;
float scd41_hum;

void setup()
{
  Wire.begin();
  Serial.begin(115200);

  while (!Serial) 
    delay(100);

  // инициализация SCD41 в режиме периодического измерения (каждые 5 секунд)
  if (scd41.init(Wire, SCD41_5s_mode, false)) 
    Serial.println("SCD41 - OK");
  else
    Serial.println("SCD41 - failed, check wires!");
}

void loop()
{ 
    // опрашиваем готовность данных из датчика
    if (scd41.isDataReady())
    {
      // считываем данные из датчика
      if (scd41.readDataMeasurement(scd41_co2, scd41_temp, scd41_hum)) 
      {
        Serial.println("CO2:"+String(scd41_co2)); // полученные от датчика значения CO2
        Serial.println("T:"+String(scd41_temp) + " RH:"+String(scd41_hum)); // температуры и относительной влажности
      }
    }
}