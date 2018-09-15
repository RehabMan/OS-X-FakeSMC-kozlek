//
//  SMMSensors.cpp
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

#include "SMMSensors.h"
#include "smm.h"

#include <IOKit/IODeviceTreeSupport.h>
#include <IOKit/IORegistryEntry.h>

//REVIEW: avoids problem with Xcode 5.1.0 where -dead_strip eliminates these required symbols
#include <libkern/OSKextLib.h>
void* _org_rehabman_dontstrip[] =
{
    (void*)&OSKextGetCurrentIdentifier,
    (void*)&OSKextGetCurrentLoadTag,
    (void*)&OSKextGetCurrentVersionString,
};

#define super FakeSMCPlugin
OSDefineMetaClassAndStructors(SMMSensors, FakeSMCPlugin)

/*
 * Call the System Management Mode BIOS. Code provided by Jonathan Buzzard.
 */
static int smm_cmd(struct smm_regs* regs)
{
    int rc;
    unsigned int eax = regs->eax;

    asm volatile("pushq %%rax\n\t"
                 "movl 0(%%rax),%%edx\n\t"
                 "pushq %%rdx\n\t"
                 "movl 4(%%rax),%%ebx\n\t"
                 "movl 8(%%rax),%%ecx\n\t"
                 "movl 12(%%rax),%%edx\n\t"
                 "movl 16(%%rax),%%esi\n\t"
                 "movl 20(%%rax),%%edi\n\t"
                 "popq %%rax\n\t"
                 "out %%al,$0xb2\n\t"
                 "out %%al,$0x84\n\t"
                 "xchgq %%rax,(%%rsp)\n\t"
                 "movl %%ebx,4(%%rax)\n\t"
                 "movl %%ecx,8(%%rax)\n\t"
                 "movl %%edx,12(%%rax)\n\t"
                 "movl %%esi,16(%%rax)\n\t"
                 "movl %%edi,20(%%rax)\n\t"
                 "popq %%rdx\n\t"
                 "movl %%edx,0(%%rax)\n\t"
                 "pushfq\n\t"
                 "popq %%rax\n\t"
                 "andl $1,%%eax\n"
                 : "=a"(rc)
                 :    "a"(regs)
                 :    "%ebx", "%ecx", "%edx", "%esi", "%edi", "memory");
    
    if (rc != 0 || (regs->eax & 0xffff) == 0xffff || regs->eax == eax)
        rc = -1;

    return rc;
}

// Read fan status
int SMMSensors::getFanStatus(int fan)
{
    struct smm_regs regs = { .eax = kSMM_GET_FAN, };
    
    regs.ebx = fan & 0xff;
    
    return smm_cmd(&regs) ? : regs.eax & 0xff;
}

// Read the fan speed in RPM.
int SMMSensors::getFanSpeed(int fan)
{
    struct smm_regs regs = { .eax = kSMM_GET_SPEED, };
    
    regs.ebx = fan & 0xff;
    return smm_cmd(&regs) ? : (regs.eax & 0xffff) * fanMultiplier;
}

// Read fan type
int SMMSensors::getFanType(int fan)
{
    /* kSMM_GET_FAN_TYPE SMM call is expensive, so cache values */
    if (fanTypes[fan] == INT_MIN) {
        struct smm_regs regs = { .eax = kSMM_GET_FAN_TYPE, };
        
        regs.ebx = fan & 0xff;
        
        fanTypes[fan] = smm_cmd(&regs) ? : regs.eax & 0xff;
    }

    return fanTypes[fan];
}

// Read the fan nominal rpm for specific fan speed.
int SMMSensors::getFanNominalSpeed(int fan, int speed)
{
    struct smm_regs regs = { .eax = kSMM_GET_NOM_SPEED, };

    regs.ebx = (fan & 0xff) | (speed << 8);
    return smm_cmd(&regs) ? : (regs.eax & 0xffff) * fanMultiplier;
}

// Set the fan speed (off, low, high). Returns the new fan status.
int SMMSensors::setFan(int fan, int speed)
{
    struct smm_regs regs = { .eax = kSMM_SET_FAN, };
    
    speed = (speed < 0) ? 0 : speed;
    regs.ebx = (fan & 0xff) | (speed << 8);
    
    return smm_cmd(&regs) ? : getFanStatus(fan);
}

int SMMSensors::getTempType(int sensor)
{
    struct smm_regs regs = { .eax = kSMM_GET_TEMP_TYPE, };
    
    regs.ebx = sensor & 0xff;
    return smm_cmd(&regs) ? : regs.eax & 0xff;
}

// Read temperature (internal)
int SMMSensors::_getTemp(int sensor)
{
    struct smm_regs regs = {
        .eax = kSMM_GET_TEMP,
        .ebx = sensor & 0xff,
    };

    return smm_cmd(&regs) ? : regs.eax & 0xff;
}

int SMMSensors::getTemp(int sensor)
{
    int temp = _getTemp(sensor);
    
    /*
     * Sometimes the temperature sensor returns 0x99, which is out of range.
     * In this case we retry (once) before returning an error.
     # 1003655137 00000058 00005a4b
     # 1003655138 00000099 00003a80 <--- 0x99 = 153 degrees
     # 1003655139 00000054 00005c52
     */
    if (temp == 0x99) {
        IOSleep(100);
        temp = _getTemp(sensor);
    }
    /*
     * Return 0 for all invalid temperatures.
     *
     * Known instances are the 0x99 value as seen above as well as
     * 0xc1 (193), which may be returned when trying to read the GPU
     * temperature if the system supports a GPU and it is currently
     * turned off.
     */
    if (temp > SMM_MAX_TEMP)
        return 0;
    
    return temp;
}

// Read the power status.
int SMMSensors::getPowerStatus(void)
{
    struct smm_regs regs = { .eax = kSMM_GET_POWER_STATUS, };
    int rc;
    
    rc = smm_cmd(&regs);
    if (rc < 0)
        return rc;
    
    return (regs.eax & 0xff) == kSMM_POWER_AC ? kSMM_POWER_AC : kSMM_POWER_BATTERY;
}

bool SMMSensors::getDellSignature()
{
    struct smm_regs regs = { .eax = kSMM_GET_DELL_SIG, };
    int rc;

    rc = smm_cmd(&regs);
    if (rc < 0)
        return false;

    return (regs.eax == SMM_DELL_SIG_2 && regs.edx == SMM_DELL_SIG_1);
}

const char * SMMSensors::getFanTypeName(int fanType)
{
    switch (fanType)
    {
        case kSMM_FAN_PROCESSOR:
            return "CPU fan";
        case kSMM_FAN_MOTHERBOARD:
            return "Motherboard fan";
        case kSMM_FAN_GPU:
            return "GPU fan";
        case kSMM_FAN_PSU:
            return "PSU fan";
        case kSMM_FAN_CHIPSET:
            return "Chipset fan";
        default:
            return "Misc. fan";
    }
}

const char * SMMSensors::getTempTypeName(int sensorType)
{
    switch (sensorType)
    {
        case kSMM_TEMP_CPU:
            return "CPU temperature";
        case kSMM_TEMP_GPU:
            return "GPU temperature";
        case kSMM_TEMP_MEMORY:
            return "Memory temperature";
        case kSMM_TEMP_MISC:
            return "Misc. temperature";
        case kSMM_TEMP_AMBIENT:
            return "Ambient temperature";
        default:
            return "Other temperature";
    }
}

bool SMMSensors::willReadSensorValue(FakeSMCSensor *sensor, float *outValue)
{
    switch (sensor->getGroup()) {
        case kFakeSMCTemperatureSensor:
            *outValue = getTemp(sensor->getIndex());
            break;
        case kFakeSMCTachometerSensor:
            *outValue = getFanSpeed(sensor->getIndex());
            break;
        default:
            return false;
    }
                                    
    return true;
}

bool SMMSensors::start(IOService *provider)
{
    if (!super::start(provider))
        return false;
    
    fanMultiplier = 1;
    
    int registered_sensors = 0;
    
    // Configure
    if (OSDictionary *configuration = getConfigurationNode())
    {
        OSBoolean* disable = OSDynamicCast(OSBoolean, configuration->getObject("DisableDevice"));
        if (disable && disable->isTrue()) {
            return false;
        }
        
        OSBoolean* ignoreSignature = OSDynamicCast(OSBoolean, configuration->getObject("ForceLoad"));

        // Validate Dell SMM signature
        if (!ignoreSignature || ignoreSignature->isFalse()) {
            if (!getDellSignature()) {
                HWSensorsErrorLog("Unable to validate Dell SMM signature, not loading SMMSensors\n");
                return false;
            }
        }

        OSNumber* numberFanMultiplier = OSDynamicCast(OSNumber, configuration->getObject("FanMultiplier"));
        if (numberFanMultiplier) {
            fanMultiplier = numberFanMultiplier->unsigned32BitValue();
            HWSensorsDebugLog("Tachometer fan multiplier \"%d\"", fanMultiplier);
        }

        /* Add platform configured fans */
        OSDictionary* tachometers = OSDynamicCast(OSDictionary, configuration->getObject("Tachometers"));

        if (tachometers) {
            OSCollectionIterator *iterator = OSCollectionIterator::withCollection(tachometers);
            
            while (OSString *key = OSDynamicCast(OSString, iterator->getNextObject())) {
                
                OSDictionary *tachometer = OSDynamicCast(OSDictionary, tachometers->getObject(key));

                if (tachometer) {
                    // Skip over disabled items
                    OSBoolean* disabled = OSDynamicCast(OSBoolean, tachometer->getObject("Disabled"));

                    if (disabled && disabled->isTrue())
                        continue;

                    OSNumber* tachometerIndex = OSDynamicCast(OSNumber, tachometer->getObject("Index"));

                    if (!tachometerIndex) {
                        HWSensorsErrorLog("No SMM index configured for tachometer sensor \"%s\".", key->getCStringNoCopy());
                        continue;
                    }

                    int fanType = getFanType(tachometerIndex->unsigned32BitValue());
                    
                    if (fanType != -1) {
                        FakeSMCSensor * sensor = addTachometer(tachometerIndex->unsigned32BitValue(), key->getCStringNoCopy(), FAN_RPM);

                        if (sensor != NULL) {
                            registered_sensors++;
                            HWSensorsDebugLog("Registered tachometer sensor \"%s\" for SMM fan index \"%d\", type \"%s\".", key->getCStringNoCopy(), tachometerIndex->unsigned32BitValue(), getFanTypeName(fanType));
                        } else {
                            HWSensorsWarningLog("Failed to register tachometer sensor \"%s\" for SMM fan index \"%d\"", key->getCStringNoCopy(), tachometerIndex->unsigned32BitValue());
                        }
                    } else {
                        HWSensorsErrorLog("Failed to validate sensor \"%s\" for SMM fan index \"%d\"", key->getCStringNoCopy(), tachometerIndex->unsigned32BitValue());
                    }
                }
            }
        }
        
        /* Add platform configured temperature sensors */
        OSDictionary* temperatures = OSDynamicCast(OSDictionary, configuration->getObject("Temperatures"));
        
        if (temperatures) {
            OSCollectionIterator *iterator = OSCollectionIterator::withCollection(temperatures);
            
            while (OSString *key = OSDynamicCast(OSString, iterator->getNextObject())) {
                
                OSDictionary *temperature = OSDynamicCast(OSDictionary, temperatures->getObject(key));
                
                if (temperature) {
                    // Skip over disabled items
                    OSBoolean* disabled = OSDynamicCast(OSBoolean, temperature->getObject("Disabled"));
                    
                    if (disabled && disabled->isTrue())
                        continue;
                    
                    OSNumber* temperatureIndex = OSDynamicCast(OSNumber, temperature->getObject("Index"));
                    
                    if (!temperatureIndex) {
                        HWSensorsErrorLog("No SMM index configured for temperature sensor \"%s\".", key->getCStringNoCopy());
                        continue;
                    }

                    int tempType = getTempType(temperatureIndex->unsigned32BitValue());
                    
                    if (tempType != -1) {

                        FakeSMCSensor* sensor = addSensorUsingAbbreviation(key->getCStringNoCopy(), kFakeSMCCategoryTemperature, kFakeSMCTemperatureSensor, temperatureIndex->unsigned32BitValue());
                        
                        if (sensor != NULL) {
                            registered_sensors++;
                            HWSensorsDebugLog("Registered temperature sensor \"%s\" for SMM temperature sensor index \"%d\", type \"%s\".", key->getCStringNoCopy(), temperatureIndex->unsigned32BitValue(), getTempTypeName(tempType));
                        } else {
                            HWSensorsWarningLog("Failed to register tachometer sensor \"%s\" for SMM temperature sensor index \"%d\"", key->getCStringNoCopy(), temperatureIndex->unsigned32BitValue());
                        }
                    } else {
                        HWSensorsErrorLog("Failed to validate sensor \"%s\" for SMM temperature sensor index \"%d\"", key->getCStringNoCopy(), temperatureIndex->unsigned32BitValue());
                    }
                }
            }
        }
    }

    if (registered_sensors > 0)
      HWSensorsInfoLog("%d sensor%s added", registered_sensors, registered_sensors > 1 ? "s" : "");
        
    // Register service
    registerService();
    
    HWSensorsInfoLog("started");
    
    return true;
}

void SMMSensors::stop(IOService *provider)
{
    super::stop(provider);
}

void SMMSensors::free()
{
    super::free();
}
