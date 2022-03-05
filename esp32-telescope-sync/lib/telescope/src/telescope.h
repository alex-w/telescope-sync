#ifndef TELESCOPE_H
#define TELESCOPE_H

using namespace std;

#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include <algorithm>
#include <helper.h>
#include <linalg.h>

#ifdef ARDUINO
#include <Arduino.h>
#else
#define PI M_PI
#endif

#define MESSAGE_CURRENT_POSITION_LENGTH (24)
#define MESSAGE_CURRENT_POSITION_TYPE (0)
#define MAX_ALIGNMENT_POINTS (32)

class Telescope
{
private:
public:
    typedef struct
    {
        double ra = 0;  // in degrees
        double dec = 0; // in degrees
    } Equatorial;

    typedef struct
    {
        double alt = 0; // in degrees
        double az = 0;  // in degrees
    } Horizontal;

    // Equatorial eqPosition;
    
    Horizontal orientation;
    Horizontal offset;

    Horizontal referencePoints[MAX_ALIGNMENT_POINTS];
    Horizontal mountPoints[MAX_ALIGNMENT_POINTS];
    int32_t alignmentWritePointer = 0;
    int32_t alignmentPoints = 0;

    Telescope(void);
    Telescope(double ra, double dec);

    void setOrientation(double alt, double az);

    bool isCalibrated = false;
    void calibrate(Equatorial reference, double latitude, double localSiderealTimeDegrees);
    void addReferencePoint(Horizontal reference);
    Horizontal getCalibratedOrientation(void);
    Horizontal getCalibratedOrientation(BLA::Matrix<3, 3, BLA::Array<3, 3, double>> M);

    BLA::Matrix<3, 3, BLA::Array<3, 3, double>> transormationMatrices[MAX_ALIGNMENT_POINTS - 2];
    BLA::Matrix<3, 3, BLA::Array<3, 3, double>> getTransformationMatrix(uint32_t triangleOffset);

    double rad(double degrees);
    double deg(double radians);
    double degToHours(double degrees);

    void horizontalToEquatorial(double azimuth, double altitude, double latitude, double localSiderealTimeDegrees, Equatorial *result);
    void horizontalToEquatorial(Horizontal horizontal, double latitude, double localSiderealTimeDegrees, Equatorial *result);
    Equatorial horizontalToEquatorial(Horizontal horizontal, double latitude, double localSiderealTimeDegrees);
    Equatorial horizontalToEquatorial(double azimuth, double altitude, double latitude, double localSiderealTimeDegrees);

    void equatorialToHorizontal(double ra, double dec, double latitude, double localSiderealTimeDegrees, Horizontal *result);
    void equatorialToHorizontal(Equatorial equatorial, double latitude, double localSiderealTimeDegrees, Horizontal *result);
    Horizontal equatorialToHorizontal(Equatorial equatorial, double latitude, double localSiderealTimeDegrees);
    Horizontal equatorialToHorizontal(double ra, double dec, double latitude, double localSiderealTimeDegrees);

    uint32_t packPosition(double ra, double dec, uint64_t timestamp, uint8_t *buffer, size_t bufferSize);
    uint32_t packPosition(Equatorial equatorial, uint64_t timestamp, uint8_t *buffer, size_t bufferSize);
    // uint32_t packPosition(uint8_t *buffer, size_t bufferSize = 24);

    bool unpackPosition(double *ra, double *dec, uint64_t *timestamp, uint8_t *data, size_t dataLength = 20);
    bool unpackPosition(Equatorial *equatorial, uint64_t *timestamp, uint8_t *data, size_t dataLength);
    // bool unpackPosition(uint8_t *data, size_t dataLength = 20);
};
#endif