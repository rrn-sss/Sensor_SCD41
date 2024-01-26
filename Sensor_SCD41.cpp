#include "sensor_scd41.h"


#define CO2_400PPM (400) // 400 ppm - концентрация CO2 в уличных условиях

bool Sensor_SCD41::init(TwoWire& i2cBus, Sensor_SCD41_Mode mode, bool debug)
{
    dbg = debug;
    md  = mode;

    scd4x.begin(i2cBus);

    // stop potentially previously started measurement
    error = scd4x.stopPeriodicMeasurement();
    if (error && dbg ) {
        Serial.println("Error trying to execute stopPeriodicMeasurement(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    }

    error = scd4x.getSerialNumber(serial0, serial1, serial2);
    if(dbg)
    {
        if (error) 
        {
            Serial.println("Error trying to execute getSerialNumber(): ");
            errorToString(error, errorMessage, 256);
            Serial.println(errorMessage);
        } else 
            printSerialNumber(serial0, serial1, serial2);
    }

    // настройка режима 
    switch( mode )
    {
        case SCD41_5s_mode: 
        case SCD41_5s_with_ASC_mode:
        case SCD41_30s_mode: 
        case SCD41_30s_with_ASC_mode:
            // включение/отключение автокалибровки
            error = scd4x.setAutomaticSelfCalibration((SCD41_5s_mode == mode || SCD41_30s_mode == mode) ? 0 : 1); 
            if (error && dbg) 
            {
                Serial.println("Error trying to execute setAutomaticSelfCalibration()");
                errorToString(error, errorMessage, 256);
                Serial.println(errorMessage);                
            }
            else
            {
                // старт измерений
                if( SCD41_30s_mode == mode || SCD41_30s_with_ASC_mode == mode )
                    error = scd4x.startLowPowerPeriodicMeasurement();
                else 
                    error = scd4x.startPeriodicMeasurement();
                if (error && dbg) 
                {
                    if( SCD41_30s_mode == mode || SCD41_30s_with_ASC_mode == mode )
                        Serial.println("Error trying to execute startLowPowerPeriodicMeasurement()");
                    else 
                        Serial.println("Error trying to execute startPeriodicMeasurement()");
                    errorToString(error, errorMessage, 256);
                    Serial.println(errorMessage);
                }
            }
            break;

        case SCD41_manual_calibration:
            error = scd4x.startPeriodicMeasurement();
            if( error && dbg )
            {
               Serial.println("Error starting forced recalibration"); 
            }
            else
            {
                if (dbg)
                    Serial.println("Starting forced recalibration. Wait 3 minutes.");
                unsigned long start_recalibration_ms = millis(); delay(1);
                while( millis() - start_recalibration_ms < 180000 )
                {
                    uint16_t co2 = 0;
                    float temperature = 0.0f;
                    float humidity = 0.0f;
                    if( isDataReady() )
                    {
                        error = readDataMeasurement(co2, temperature, humidity);
                        if(!error && dbg)
                            Serial.println("Recalibration: CO2=" + String(co2));
                    }
                    delay(10000);
                }
                scd4x.stopPeriodicMeasurement();
                delay(500);
                error = scd4x.performForcedRecalibration(CO2_400PPM, frc_correction);
                if (0xFFFF == error && dbg)
                    Serial.println("Error trying to execute performForcedRecalibration()");
                else
                    Serial.println("Forced recalibration finished succesefully.");

                while(1)
                    delay(1000);
            }
            break;
        case SCD41_single_mode:
            scd4x.powerDown();
            if (dbg)
                    Serial.println("Single measurement mode on demand");
            break;
        default:
            if (dbg)
                Serial.println("Unknown mode");
            break;
    }    

    return error ? false : true;
}

bool Sensor_SCD41::isDataReady()
{
    bool isReady = false;

    error = scd4x.getDataReadyFlag(isReady);
    if (error && dbg)
    {
        Serial.print("Error trying to execute getDataReadyFlag(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    }

    return isReady;
}

bool Sensor_SCD41::readDataMeasurement(uint16_t &co2, float &temperature, float &humidity)
{
    error = 0;

    if( 0 == scd4x.getDataReadyFlag(isReady) && isReady )
    {
        error = scd4x.readMeasurement(co2, temperature, humidity);
        if(dbg)
        {
            if (error) 
            {
                Serial.print("Error trying to execute readMeasurement(): ");
                errorToString(error, errorMessage, 256);
                Serial.println(errorMessage);
            } else if (co2 == 0) 
                    Serial.println("Invalid sample detected, skipping.");
                else
            {
                Serial.print("Co2:");
                Serial.print(co2);
                Serial.print("\t");
                Serial.print("Temperature:");
                Serial.print(temperature);
                Serial.print("\t");
                Serial.print("Humidity:");
                Serial.println(humidity);
            }
        }

        if( co2 < CO2_400PPM)
            co2 = CO2_400PPM;

        if( SCD41_single_mode == md)
            scd4x.powerDown();
    }

    return (error || !isReady) ? false : true;
}

bool Sensor_SCD41::singleMeasurement()
{
  if ( SCD41_single_mode != md)
    return false;

  scd4x.wakeUp();

  return !scd4x.measureSingleShot() ? true : false;
}

void Sensor_SCD41::printUint16Hex(uint16_t value)
{
    Serial.print(value < 4096 ? "0" : "");
    Serial.print(value < 256 ? "0" : "");
    Serial.print(value < 16 ? "0" : "");
    Serial.print(value, HEX);
}

void Sensor_SCD41::printSerialNumber(uint16_t serial0, uint16_t serial1, uint16_t serial2) 
{
    Serial.print("Serial: 0x");
    printUint16Hex(serial0);
    printUint16Hex(serial1);
    printUint16Hex(serial2);
    Serial.println();
}