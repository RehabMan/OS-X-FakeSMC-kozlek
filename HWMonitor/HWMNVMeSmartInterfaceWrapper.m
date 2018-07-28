//
//  HWMNVMeSmartInterfaceWrapper.m
//  HWMonitor
//
//  Created by Natan Zalkin on 16/04/2017.
//  Copyright © 2017 kozlek. All rights reserved.
//

#import "HWMNVMeSmartInterfaceWrapper.h"
#import "NSString+returnCodeDescription.h"

#import "HWMSensor.h"
#import "HWMEngine.h"
#import "HWMConfiguration.h"

#import "Localizer.h"

#import <Growl/Growl.h>

#pragma mark
#pragma mark HWMNVMeSmartInterfaceWrapper

#define RAW_TO_LONG(attribute)  (UInt64)attribute->rawvalue[0] | \
(UInt64)attribute->rawvalue[1] << 8 | \
(UInt64)attribute->rawvalue[2] << 16 | \
(UInt64)attribute->rawvalue[3] << 24 | \
(UInt64)attribute->rawvalue[4] << 32 | \
(UInt64)attribute->rawvalue[5] << 40

#define GetLocalizedAttributeName(key) \
[[NSBundle mainBundle] localizedStringForKey:(key) value:@"" table:@"SmartAttributes"]

static NSMutableDictionary *    gATASmartInterfaceWrapperCache = nil;
static NSMutableDictionary *    gATASmartAttributeOverrideCache = nil;
static NSArray *                gATASmartAttributeOverrideDatabase = nil;

@implementation HWMNVMeSmartInterfaceWrapper

@synthesize pluginInterface = _pluginInterface;
@synthesize smartInterface = _smartInterface;
@synthesize attributes = _attributes;
@synthesize overrides = _overrides;

+(HWMNVMeSmartInterfaceWrapper*)wrapperWithService:(io_service_t)service productName:(NSString*)productName firmware:(NSString*)firmware bsdName:(NSString*)bsdName isRotational:(BOOL)rotational
{
    
    if (!gATASmartInterfaceWrapperCache) {
        gATASmartInterfaceWrapperCache = [[NSMutableDictionary alloc] init];
    }
    
    HWMNVMeSmartInterfaceWrapper *wrapper = [gATASmartInterfaceWrapperCache objectForKey:bsdName];
    
    if (!wrapper) {
        
        IOCFPlugInInterface ** pluginInterface = NULL;
        IONVMeSMARTInterface ** smartInterface = NULL;
        SInt32 score = 0;
        int tryCount = 4;
        
        do {
            
            IOReturn result;
            
            if (kIOReturnSuccess == (result = IOCreatePlugInInterfaceForService(service, kIONVMeSMARTUserClientTypeID, kIOCFPlugInInterfaceID, &pluginInterface, &score))) {
                
                HRESULT hresult;
                
                if (S_OK == (hresult = (*pluginInterface)->QueryInterface(pluginInterface, CFUUIDGetUUIDBytes(kIONVMeSMARTInterfaceID), (LPVOID)&smartInterface))) {
                    
                    wrapper = [[HWMNVMeSmartInterfaceWrapper alloc] init];
                    
                    (*smartInterface)->AddRef(smartInterface);
                    (*pluginInterface)->AddRef(pluginInterface);
                    
                    wrapper.pluginInterface = pluginInterface;
                    wrapper.smartInterface  = smartInterface;
                    wrapper.bsdName = bsdName;
                    wrapper.product = productName;
                    wrapper.firmware = firmware;
                    wrapper.isRotational = rotational;
                    
                    if ([wrapper readSMARTDataAndThresholds]) {
                        [gATASmartInterfaceWrapperCache setObject:wrapper forKey:bsdName];
                        return wrapper;
                    }
                }
                else {
                    NSLog(@"pluginInterface->QueryInterface error: %d for %@", hresult, productName);
                }
            }
            else {
                NSLog(@"IOCreatePlugInInterfaceForService error: %@ for %@", [NSString stringFromReturn:result], productName);
            }
            
            if (smartInterface) {
                (*smartInterface)->Release(smartInterface);
            }
            
            if (pluginInterface) {
                IODestroyPlugInInterface(pluginInterface);
            }
            
            [NSThread sleepForTimeInterval:0.25];
            
        } while (--tryCount);
        
    }
    
    return wrapper;
}

+(HWMNVMeSmartInterfaceWrapper*)getWrapperForBsdName:(NSString*)name
{
    if (name && gATASmartInterfaceWrapperCache) {
        return [gATASmartInterfaceWrapperCache objectForKey:name];
    }
    
    return nil;
}

+(void)destroyAllWrappers
{
    [gATASmartInterfaceWrapperCache removeAllObjects];
    
    gATASmartInterfaceWrapperCache = nil;
}

-(BOOL)readSMARTDataAndThresholds
{
    NSLog(@"reading SMART data for %@", _product);
    
    IOReturn result = kIOReturnError;
    
    nvme_smart_log smartLog;
    nvme_id_ctrl idControl;
    
    bzero(&smartLog, sizeof(smartLog));
    //bzero(&idControl, sizeof(idControl));
    
    if (kIOReturnSuccess != (*_smartInterface)->SMARTReadData(_smartInterface, &smartLog)) {
        NSLog(@"SMARTReadData returned error for: %@ code: %@", _product, [NSString stringFromReturn:result]);
    }

    if (kIOReturnSuccess != (*_smartInterface)->GetIdentifyData(_smartInterface, &idControl, 0)) {
        NSLog(@"GetIdentifyData returned error for: %@ code: %@", _product, [NSString stringFromReturn:result]);
    }

    NSMutableArray * attributes = [[NSMutableArray alloc] init];
    
    NSNumber* spare_avail = [NSNumber numberWithUnsignedShort:smartLog.avail_spare];
    NSNumber* spare_thres = [NSNumber numberWithUnsignedShort:smartLog.spare_thresh];
    
    [attributes addObject:@{@"level": [NSNumber numberWithInteger:spare_avail.longValue <= spare_thres.longValue ? kHWMSensorLevelExceeded : kHWMSensorLevelNormal],
                            @"name": @kNVMeSMARTSAvailableSpareKey,
                            @"title": @"SSD Available Spare",
                            @"critical": @"Percent",
                            @"value": spare_avail,
                            @"worst": spare_avail,
                            @"threshold": spare_thres,
                            @"raw": spare_avail,
                            @"rawFormatted": [NSString stringWithFormat:@"%@%%", spare_avail]
                            }];

    NSNumber* percent_remaining = [NSNumber numberWithUnsignedChar:100 - smartLog.percent_used];

    [attributes addObject:@{@"level": [NSNumber numberWithInteger:percent_remaining.longValue > 0 ? kHWMSensorLevelNormal : kHWMSensorLevelExceeded],
                            @"name": @kNVMeSMARTRemainingLifeKey,
                            @"title": @"SSD Remaining Life",
                            @"critical": @"Percent",
                            @"value": percent_remaining,
                            @"worst": percent_remaining,
                            @"threshold": @0,
                            @"raw": percent_remaining,
                            @"rawFormatted": [NSString stringWithFormat:@"%@%%", percent_remaining],
                            }];
    
    NSNumber* temperature = [NSNumber numberWithUnsignedShort:sg_get_unaligned_le16(smartLog.temperature) - 273];
    NSNumber* temp_warn   = [NSNumber numberWithUnsignedShort:idControl.wctemp - 273];
    //NSNumber* temp_crit   = [NSNumber numberWithUnsignedShort:idControl.cctemp - 273];
    
    [attributes addObject:@{@"level": [NSNumber numberWithInteger:temperature.longValue < temp_warn.longValue ? kHWMSensorLevelNormal : kHWMSensorLevelExceeded],
                            @"name": @kNVMeSMARTTemperatureKey,
                            @"title": @"SSD Temperature",
                            @"critical": @"° Celcius",
                            @"value": temperature,
                            @"worst": temperature,
                            @"threshold": temp_warn,
                            @"raw": temperature,
                            @"rawFormatted": [NSString stringWithFormat:@"%@°", temperature]
                            }];

    NSNumber* data_read = [NSNumber numberWithFloat:((sg_get_unaligned_le128(smartLog.data_units_read) * 512 * 1000) / 10000000000) / (float)100];
    
    [attributes addObject:@{@"level": [NSNumber numberWithInteger:kHWMSensorLevelDisabled],
                            @"name": @kNVMESMARTDataReadKey,
                            @"title": @"Data Read",
                            @"critical": @"Tb",
                            @"value": data_read,
                            @"worst": data_read,
                            @"threshold": @0,
                            @"raw": data_read,
                            @"rawFormatted": [NSString stringWithFormat:@"%@ Tb", data_read]
                            }];

    NSNumber* data_written = [NSNumber numberWithFloat:((sg_get_unaligned_le128(smartLog.data_units_written) * 512 * 1000) / 10000000000) / (float)100];
    
    [attributes addObject:@{@"level": [NSNumber numberWithInteger:kHWMSensorLevelDisabled],
                            @"name": @kNVMeSMARTDataWrittenKey,
                            @"title": @"Data Written",
                            @"critical": @"Tb",
                            @"value": data_written,
                            @"worst": data_written,
                            @"threshold": @0,
                            @"raw": data_written,
                            @"rawFormatted": [NSString stringWithFormat:@"%@ Tb", data_written]
                            }];
    
    NSNumber* power_cycles = [NSNumber numberWithFloat:sg_get_unaligned_le128(smartLog.power_cycles)];
    
    [attributes addObject:@{@"level": [NSNumber numberWithInteger:kHWMSensorLevelDisabled],
                            @"name": @kNVMeSMARTPowerCyclesKey,
                            @"title": @"Power Cycles",
                            @"value": power_cycles,
                            @"worst": power_cycles,
                            @"threshold": @0,
                            @"raw": power_cycles,
                            @"rawFormatted": power_cycles
                            }];
    
    NSNumber* poweron_hours = [NSNumber numberWithFloat:sg_get_unaligned_le128(smartLog.power_on_hours)];
    
    [attributes addObject:@{@"level": [NSNumber numberWithInteger:kHWMSensorLevelDisabled],
                            @"name": @kNVMeSMARTPowerOnHoursKey,
                            @"title": @"Power-on Hours",
                            @"value": poweron_hours,
                            @"worst": poweron_hours,
                            @"threshold": @0,
                            @"raw": poweron_hours,
                            @"rawFormatted": poweron_hours
                            }];
    
    NSNumber* unsafe_shutdowns = [NSNumber numberWithFloat:sg_get_unaligned_le128(smartLog.unsafe_shutdowns)];
    
    [attributes addObject:@{@"level": [NSNumber numberWithInteger:kHWMSensorLevelDisabled],
                            @"name": @kNVMeSMARTUnsafeShutdownsKey,
                            @"title": @"Unsafe Shutdowns",
                            @"value": unsafe_shutdowns,
                            @"worst": unsafe_shutdowns,
                            @"threshold": @0,
                            @"raw": unsafe_shutdowns,
                            @"rawFormatted": unsafe_shutdowns
                            }];

    NSNumber* media_errors = [NSNumber numberWithFloat:sg_get_unaligned_le128(smartLog.media_errors)];
    
    [attributes addObject:@{@"level": [NSNumber numberWithInteger:media_errors.longLongValue > 0 ? kHWMSensorLevelExceeded : kHWMSensorLevelNormal],
                            @"name": @kNVMeSMARTIntegrityErrorsKey,
                            @"title": @"Media Integrity Errors",
                            @"value": media_errors,
                            @"worst": media_errors,
                            @"threshold": @0,
                            @"raw": media_errors,
                            @"rawFormatted": media_errors
                            }];

    NSNumber* num_err_log = [NSNumber numberWithFloat:sg_get_unaligned_le128(smartLog.num_err_log_entries)];
    
    [attributes addObject:@{@"level": [NSNumber numberWithInteger:num_err_log.longLongValue > 0 ? kHWMSensorLevelExceeded : kHWMSensorLevelNormal],
                            @"name": @kNVMeSMARTErrorLogEntriesKey,
                            @"title": @"Error Log Entries",
                            @"value": num_err_log,
                            @"worst": num_err_log,
                            @"threshold": @0,
                            @"raw": num_err_log,
                            @"rawFormatted": num_err_log
                            }];

    _attributes = [attributes copy];

    return result == kIOReturnSuccess;
}

-(void)releaseInterface
{
    if (_smartInterface) {
        (*_smartInterface)->Release(_smartInterface);
    }
    
    if (_pluginInterface) {
        IODestroyPlugInInterface(_pluginInterface);
    }
}

-(void)dealloc
{
    //NSLog(@"Wrapper deallocated for %@", _bsdName);
    [self releaseInterface];
}

@end

