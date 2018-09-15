//
//  SMMSensors.h
//  HWSensors
//
//  Created by darkvoid on 25.07.18.
//
//  Based on Linux dell-smm-hwmon (https://github.com/torvalds/linux/blob/master/drivers/hwmon/dell-smm-hwmon.c)
//
//  The MIT License (MIT)
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy of this software
//  and associated documentation files (the "Software"), to deal in the Software without restriction,
//  including without limitation the rights to use, copy, modify, merge, publish, distribute,
//  sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all copies or
//  substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
//  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
//  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
//  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "FakeSMCPlugin.h"

#include <IOKit/IOTimerEventSource.h>
#include <IOKit/IOLib.h>

#ifndef INT_MIN
// from 10.13 SDK limits.h
#define    INT_MIN        (-2147483647-1)    /* min value for an int */
#endif

class EXPORT SMMSensors : public FakeSMCPlugin
{
    OSDeclareDefaultStructors(SMMSensors)
    
private:
    int                     fanTypes[5] = { INT_MIN, INT_MIN, INT_MIN, INT_MIN, INT_MIN }; // Cache fan types for performance
    uint                    fanMultiplier; // Multiplier for fan speeds (if required by configuration)

    int                     getFanStatus(int fan);
    int                     getFanSpeed(int fan);
    int                     getFanType(int fan);
    int                     getFanNominalSpeed(int fan, int speed);
    int                     setFan(int fan, int speed);
    
    int                     getTempType(int sensor);
    int                     getTemp(int sensor);
    
    int                     getPowerStatus(void);
    
    bool                    getDellSignature();
    
    const char *            getFanTypeName(int fanType);
    const char *            getTempTypeName(int sensorType);

    int                     _getTemp(int sensor);
protected:
    virtual bool            willReadSensorValue(FakeSMCSensor *sensor, float *outValue);
    
public:
    virtual bool            start(IOService *provider);
    virtual void            stop(IOService* provider);
    virtual void            free(void);
};
