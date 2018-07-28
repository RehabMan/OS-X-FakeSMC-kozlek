//
//  HWMNVMeSmartInterfaceWrapper.h
//  HWMonitor
//
//  Created by darkvoid on 28/07/2018.
//  Copyright © 2017 kozlek. All rights reserved.
//

// The parts of this code is based on http://smartmontools.sourceforge.net

/*
 * Home page of code is: http://smartmontools.sourceforge.net
 *
 * Copyright (C) 2002-10 Bruce Allen <smartmontools-support@lists.sourceforge.net>
 * Copyright (C) 2008-10 Christian Franke <smartmontools-support@lists.sourceforge.net>
 * Copyright (C) 1999-2000 Michael Cornwell <cornwell@acm.org>
 * Copyright (C) 2000 Andre Hedrick <andre@linux-ide.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * You should have received a copy of the GNU General Public License
 * (for example COPYING); if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * This code was originally developed as a Senior Thesis by Michael Cornwell
 * at the Concurrent Systems Laboratory (now part of the Storage Systems
 * Research Center), Jack Baskin School of Engineering, University of
 * California, Santa Cruz. http://ssrc.soe.ucsc.edu/
 *
 */

#import <Foundation/Foundation.h>
#include <IOKit/storage/ata/ATASMARTLib.h>

#include "nvme.h"

@interface HWMNVMeSmartInterfaceWrapper : NSObject
{
    NSArray *_attributes;
    
    NSDictionary *_overrides;
    
    NSString *_product;
    NSString *_firmware;
    NSString *_bsdName;
    BOOL _rotational;
}

@property (nonatomic, assign) IOCFPlugInInterface** pluginInterface;
@property (nonatomic, assign) IONVMeSMARTInterface** smartInterface;
@property (nonatomic, strong) NSString * bsdName;
@property (nonatomic, strong) NSString * product;
@property (nonatomic, strong) NSString * firmware;
@property (assign) BOOL isRotational;

@property (readonly) NSArray * attributes;
@property (readonly) NSDictionary * overrides;

+(HWMNVMeSmartInterfaceWrapper*)wrapperWithService:(io_service_t)service productName:(NSString*)productName firmware:(NSString*)firmware bsdName:(NSString*)bsdName isRotational:(BOOL)rotational;
+(HWMNVMeSmartInterfaceWrapper*)getWrapperForBsdName:(NSString*)name;
+(void)destroyAllWrappers;

@end
