#ifndef _SENSOR_SCD41_H_
#define _SENSOR_SCD41_H_

#include <Arduino.h>
#include <SensirionI2CScd4x.h>

/*********************************************************************/
/* оберточная библиотека - враппер над библиотекой SensirionI2CScd4x */
/*********************************************************************/

/**
 * режимы работы датчика SCD41
 * @note задаются в качестве параметра функции init()
 */

/** режимы функционирования датчика */
enum Sensor_SCD41_Mode
{
    SCD41_5s_with_ASC_mode,  // режим периодических измерений (одно измерение за 5с.) с периодической автокалибровкой
    SCD41_30s_with_ASC_mode, // режим периодических измерений (одно измерение за 30с.) с периодической автокалибровкой
    SCD41_5s_mode,           // режим периодических измерений (одно измерение за 5с.)
    SCD41_30s_mode,          // режим периодических измерений (одно измерение за 30с.)
    SCD41_manual_calibration,// режим принудительной калибровки
    SCD41_single_mode,       // режим одиночного измерения по запросу
};

class Sensor_SCD41
{
public:
    Sensor_SCD41()
    {
        serial0 = serial1 = serial2 =
            frc_correction =
                isReady =
                    dbg = false;
    }
    virtual ~Sensor_SCD41(){};

    /** функция инициализации датчика SCD41
     * @param i2cBus [in] объект шины I2C используемый для коммуникации
     * @param mode [in] режим работы SCD41
     * @param debug [in] признак отладочного режима (вывод в консоль отладочных сообщений)
     * @return признак успешности проведения инициализации
     */
    bool init(TwoWire &i2cBus, Sensor_SCD41_Mode mode = SCD41_5s_with_ASC_mode, bool debug = false);

    /** функция возвращающая признак готовности данных измерения для последеющего считывания
     * @return признак готовности данных измерения для последеющего считывания
     */
    bool isDataReady();

    /** функция возвращающая данные последнего измерения или NACK если данных нет
     * @param co2 [out] измеренные данные CO2
     * @param temperature [out] измеренные данные температуры, градусы Цельсия
     * @param humidity [out] измеренные данные относительной влажности, проценты
     * @return признак успешности выполнения функции
     */
    bool readDataMeasurement(uint16_t &co2, float &temperature, float &humidity);

    /**
     * функция возвращающая данные последнего измерения или NACK если данных нет
     * @return признак успешности выполнения функции
     */
    bool singleMeasurement();

protected:
    void printUint16Hex(uint16_t value);
    void printSerialNumber(uint16_t serial0, uint16_t serial1, uint16_t serial2);

private:
    SensirionI2CScd4x scd4x;
    uint16_t error;
    char errorMessage[256];
    uint16_t serial0; // serial number SCD41, part 0
    uint16_t serial1; // serial number SCD41, part 1
    uint16_t serial2; // serial number SCD41, part 2
    bool isReady;
    uint16_t frc_correction;
    bool dbg;
    Sensor_SCD41_Mode md;
};

#endif // _SENSOR_SCD41_H_