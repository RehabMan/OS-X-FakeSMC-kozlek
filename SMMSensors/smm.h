//
//  smm.h
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

#ifndef smm_h
#define smm_h

// Dell SMM Commands
enum
{
    kSMM_GET_FN_STATUS      = 0x0025,
    kSMM_GET_POWER_STATUS   = 0x0069,
    kSMM_SET_FAN            = 0x01a3,
    kSMM_GET_FAN            = 0x00a3,
    kSMM_GET_SPEED          = 0x02a3,
    kSMM_GET_FAN_TYPE       = 0x03a3,
    kSMM_GET_NOM_SPEED      = 0x04a3,
    kSMM_GET_TOLERANCE      = 0x05a3,
    kSMM_GET_TEMP           = 0x10a3,
    kSMM_GET_TEMP_TYPE      = 0x11a3,
    kSMM_GET_POWER_TYPE     = 0x22a3,
    //kSMM_GET_POWER_STATUS   = 0x24a3,
    kSMM_GET_DOCK_STATE     = 0x40a3,
    kSMM_GET_DELL_SIG       = 0xfea3,
    kSMM_GET_DELL_SIG_2     = 0xffa3
};

#define SMM_DELL_SIG_1      0x44454C4C // "DELL"
#define SMM_DELL_SIG_2      0x44494147 // "DIAG"

#define SMM_MAX_TEMP        127

// kSMM_GET_FAN_TYPE result codes
// Note dock fans have 0x10 set
//  (type & 0x10) ? type &= 0x0F : type;
enum
{
    kSMM_FAN_PROCESSOR,
    kSMM_FAN_MOTHERBOARD,
    kSMM_FAN_GPU,
    kSMM_FAN_PSU,
    kSMM_FAN_CHIPSET,
    kSMM_FAN_OTHER
};

// kSMM_GET_TEMP_TYPE result codes
enum
{
    kSMM_TEMP_CPU,
    kSMM_TEMP_GPU,
    kSMM_TEMP_MEMORY,
    kSMM_TEMP_MISC,
    kSMM_TEMP_AMBIENT,
    kSMM_TEMP_OTHER
};

// kSMM_GET_FN_STATUS result codes
enum
{
    kSMM_FN_NONE    = 0x00,
    kSMM_FN_UP      = 0x01,
    kSMM_FN_DOWN    = 0x02,
    kSMM_FN_MUTE    = 0x04,
    kSMM_FN_MASK    = 0x07,
    kSMM_FN_SHIFT   = 0x08
};

// kSMM_GET_POWER result codes
enum
{
    kSMM_POWER_AC      = 0x05,
    kSMM_POWER_BATTERY = 0x01
};

struct smm_regs {
    unsigned int eax;
    unsigned int ebx __attribute__ ((packed));
    unsigned int ecx __attribute__ ((packed));
    unsigned int edx __attribute__ ((packed));
    unsigned int esi __attribute__ ((packed));
    unsigned int edi __attribute__ ((packed));
};

#endif /* smm_h */
