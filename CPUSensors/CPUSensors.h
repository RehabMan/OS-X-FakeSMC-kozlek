/*
 *  HWSensors.h
 *  CPUSensorsPlugin
 *
 *  Based on code by mercurysquad, superhai (C) 2008
 *  Based on code from Open Hardware Monitor project by Michael Möller (C) 2011
 *
 *  Created by kozlek on 30/09/10.
 *  Copyright 2010 Natan Zalkin <natan.zalkin@me.com>. All rights reserved.
 *
 */

/*
 
 Version: MPL 1.1/GPL 2.0/LGPL 2.1
 
 The contents of this file are subject to the Mozilla Public License Version
 1.1 (the "License"); you may not use this file except in compliance with
 the License. You may obtain a copy of the License at
 
 http://www.mozilla.org/MPL/
 
 Software distributed under the License is distributed on an "AS IS" basis,
 WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 for the specific language governing rights and limitations under the License.
 
 The Original Code is the Open Hardware Monitor code.
 
 The Initial Developer of the Original Code is 
 Michael Möller <m.moeller@gmx.ch>.
 Portions created by the Initial Developer are Copyright (C) 2011
 the Initial Developer. All Rights Reserved.
 
 Contributor(s):
 
 Alternatively, the contents of this file may be used under the terms of
 either the GNU General Public License Version 2 or later (the "GPL"), or
 the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 in which case the provisions of the GPL or the LGPL are applicable instead
 of those above. If you wish to allow use of your version of this file only
 under the terms of either the GPL or the LGPL, and not to allow others to
 use your version of this file under the terms of the MPL, indicate your
 decision by deleting the provisions above and replace them with the notice
 and other provisions required by the GPL or the LGPL. If you do not delete
 the provisions above, a recipient may use your version of this file under
 the terms of any one of the MPL, the GPL or the LGPL.
 
 */

#include "FakeSMCPlugin.h"

#include <IOKit/IOTimerEventSource.h>
#include <IOKit/IOLib.h>

#include "cpuid.h"


#define MSR_IA32_THERM_STS                  0x019C
#define MSR_IA32_TEMP_TARGET                0x01A2
#define MSR_IA32_PERF_STATUS                0x0198

#define MSR_RAPL_POWER_UNIT                 0x606
#define MSR_PKG_ENERY_STATUS                0x611
#define MSR_DRAM_ENERGY_STATUS              0x619
#define MSR_PP0_ENERY_STATUS                0x639
#define MSR_PP1_ENERY_STATUS                0x641

#ifndef MSR_IA32_APERF
#define MSR_IA32_APERF                      0x0E8
#endif
#ifndef MSR_IA32_MPERF
#define MSR_IA32_MPERF                      0x0E7
#endif

#define MSR_IA32_TIME_STAMP_COUNTER         0x10

#define kCPUSensorsMaxCpus                  32

extern "C" int cpu_number(void);
extern "C" void mp_rendezvous_no_intrs(void (*action_func)(void *), void * arg);

struct CPUSensorsCounters {
    UInt16  event_flags;

    UInt8   thermal_status[kCPUSensorsMaxCpus];
    UInt8   thermal_status_package;

    UInt16  perf_status[kCPUSensorsMaxCpus];

    bool    update_perf_counters;

    UInt64  aperf_before[kCPUSensorsMaxCpus];
    UInt64  aperf_after[kCPUSensorsMaxCpus];
    UInt64  mperf_before[kCPUSensorsMaxCpus];
    UInt64  mperf_after[kCPUSensorsMaxCpus];

    UInt64  utc_before[kCPUSensorsMaxCpus];
    UInt64  utc_after[kCPUSensorsMaxCpus];
    UInt64  urc_before[kCPUSensorsMaxCpus];
    UInt64  urc_after[kCPUSensorsMaxCpus];

    UInt64  energy_before[4];
    UInt64  energy_after[4];
};

class EXPORT CPUSensors : public FakeSMCPlugin
{
    OSDeclareDefaultStructors(CPUSensors)    
    
private:
    CPUSensorsCounters      counters;

    OSData*                 platform;
    UInt64                  busClock;
    UInt8                   baseMultiplier;
    float                   energyUnits;
    UInt8                   coreCount;

    float                   multiplier[kCPUSensorsMaxCpus];
    float                   voltage[kCPUSensorsMaxCpus];
    float                   ratio[kCPUSensorsMaxCpus];
    float                   turbo[kCPUSensorsMaxCpus];
    float                   energy[kCPUSensorsMaxCpus];


    IOTimerEventSource*     timerEventSource;
    IOReturn                timerEventAction(void);
    double                  timerEventLastTime;
    double                  timerEventDeltaTime;
    bool                    timerEventScheduled;

    void                    calculateMultiplier(UInt32 index);
    void                    calculateVoltage(UInt32 index);
    void                    calculateTimedCounters();
    
    virtual FakeSMCSensor   *addSensor(const char *key, const char *type, UInt8 size, UInt32 group, UInt32 index, float reference = 0.0f, float gain = 0.0f, float offset = 0.0f);
    
    
protected:
    virtual bool            willReadSensorValue(FakeSMCSensor *sensor, float *outValue);
    
public:
    virtual bool			start(IOService *provider);
    virtual IOReturn        setPowerState(unsigned long powerState, IOService *device);
    virtual void            stop(IOService* provider);
    virtual void			free(void);
};
