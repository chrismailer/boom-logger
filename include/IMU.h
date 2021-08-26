
#include <SparkFunLSM9DS1.h>
#include <Wire.h>

class IMU: public LSM9DS1 {
    public :
    // custom calibrate function. Modified version of the one in LSM9DS1 library
    void customCalibrate(bool autoCalc) {
        uint8_t samples = 0;
        int ii;
        int32_t aBiasRawTemp[3] = {0, 0, 0};
        
        // Turn on FIFO and set threshold to 32 samples
        enableFIFO(true);
        setFIFO(FIFO_THS, 0x1F);
        while (samples < 0x1F) {
            samples = (xgReadByte(FIFO_SRC) & 0x3F); // Read number of stored samples
        }
        for(ii = 0; ii < samples ; ii++) {	// Read the gyro data stored in the FIFO
            readAccel();
            aBiasRawTemp[0] += ax;
            aBiasRawTemp[1] += ay;
            aBiasRawTemp[2] += az;
        }  
        for (ii = 0; ii < 3; ii++) {
            aBiasRaw[ii] = aBiasRawTemp[ii] / samples;
            aBias[ii] = calcAccel(aBiasRaw[ii]);
        }
        enableFIFO(false);
        setFIFO(FIFO_OFF, 0x00);
        if (autoCalc) _autoCalc = true;
    }


    // configure settings for the IMU
    // https://github.com/sparkfun/SparkFun_LSM9DS1_Arduino_Library/blob/master/examples/LSM9DS1_Settings/LSM9DS1_Settings.ino
    void configureSettings() {
        // enable or disable sensors
        settings.accel.enabled = true;
        settings.gyro.enabled = false;
        settings.mag.enabled = false;
        settings.temp.enabled = true;
        // configure accelerometer
        settings.accel.scale = 4; // 4g's
        settings.accel.sampleRate = 6; // 952 Hz
        settings.accel.bandwidth = 2; // 0 = 408 Hz, 1 = 211 Hz, 2 = 105 Hz, 3 = 50 Hz
        // settings.accel.highResEnable = false;
        // settings.accel.highResBandwidth = 0; // 0 = ODR/50, 1 = ODR/100, 2 = ODR/9, 3 = ODR/400
        initAccel(); // writes settings to registers
    }


    // runs through startup procedure
    // configures IMU settings and calibrates the accelerometer
    void start() {
        Wire.begin();
        Wire.setClock(400000); // 400kHz I2C
        if (!begin()) {
            Serial.println("Failed to initialize IMU");
            while (1);
        }
        configureSettings();
        customCalibrate(true);
    }
};
