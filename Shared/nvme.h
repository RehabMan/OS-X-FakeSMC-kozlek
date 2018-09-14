//
//  nvme.h
//  HWMonitor
//  Created by darkvoid on 28/07/2018.
//
//  Based on www.smartmontools.org/browser/
//
#ifndef nvme_h
#define nvme_h

#define kIONVMeSMARTUserClientTypeID        CFUUIDGetConstantUUIDWithBytes(NULL,                \
                                            0xAA, 0x0F, 0xA6, 0xF9, 0xC2, 0xD6, 0x45, 0x7F,     \
                                            0xB1, 0x0B, 0x59, 0xA1, 0x32, 0x53, 0x29, 0x2F)

#define kIONVMeSMARTInterfaceID             CFUUIDGetConstantUUIDWithBytes(NULL,                \
                                            0xCC, 0xD1, 0xDB, 0x19, 0xFD, 0x9A, 0x4D, 0xAF,     \
                                            0xBF, 0x95, 0x12, 0x45, 0x4B, 0x23, 0x0A, 0xB6)

#define kIOPropertyNVMeSMARTCapableKey      "NVMe SMART Capable"
#define kIONVMeBlockStorageDevice           "IONVMeBlockStorageDevice"


#define kNVMeSMARTTemperatureKey            "Temperature_Celsius"
#define kNVMeSMARTRemainingLifeKey          "SSD_Life_Left"
#define kNVMeSMARTSAvailableSpareKey        "NVMe_Available_Spare"
#define kNVMESMARTDataReadKey               "NVMe_Data_Read"
#define kNVMeSMARTDataWrittenKey            "NVMe_Data_Written"
#define kNVMeSMARTPowerCyclesKey            "NVMe_Power_Cycles"
#define kNVMeSMARTPowerOnHoursKey           "NVMe_PowerOn_Hours"
#define kNVMeSMARTUnsafeShutdownsKey        "NVMe_Unsafe_Shutdowns"
#define kNVMeSMARTIntegrityErrorsKey        "NVMe_Integrity_Errors"
#define kNVMeSMARTErrorLogEntriesKey        "NVMe_Error_Log_Entries"

typedef struct nvme_id_power_state {
    unsigned short  max_power; // centiwatts
    unsigned char   rsvd2;
    unsigned char   flags;
    unsigned int    entry_lat; // microseconds
    unsigned int    exit_lat;  // microseconds
    unsigned char   read_tput;
    unsigned char   read_lat;
    unsigned char   write_tput;
    unsigned char   write_lat;
    unsigned short  idle_power;
    unsigned char   idle_scale;
    unsigned char   rsvd19;
    unsigned short  active_power;
    unsigned char   active_work_scale;
    unsigned char   rsvd23[9];
} nvme_id_power_state;

typedef struct nvme_id_ctrl {
    unsigned short  vid;
    unsigned short  ssvid;
    char            sn[20];
    char            mn[40];
    char            fr[8];
    unsigned char   rab;
    unsigned char   ieee[3];
    unsigned char   cmic;
    unsigned char   mdts;
    unsigned short  cntlid;
    unsigned int    ver;
    unsigned int    rtd3r;
    unsigned int    rtd3e;
    unsigned int    oaes;
    unsigned int    ctratt;
    unsigned char   rsvd100[156];
    unsigned short  oacs;
    unsigned char   acl;
    unsigned char   aerl;
    unsigned char   frmw;
    unsigned char   lpa;
    unsigned char   elpe;
    unsigned char   npss;
    unsigned char   avscc;
    unsigned char   apsta;
    unsigned short  wctemp;
    unsigned short  cctemp;
    unsigned short  mtfa;
    unsigned int    hmpre;
    unsigned int    hmmin;
    unsigned char   tnvmcap[16];
    unsigned char   unvmcap[16];
    unsigned int    rpmbs;
    unsigned short  edstt;
    unsigned char   dsto;
    unsigned char   fwug;
    unsigned short  kas;
    unsigned short  hctma;
    unsigned short  mntmt;
    unsigned short  mxtmt;
    unsigned int    sanicap;
    unsigned char   rsvd332[180];
    unsigned char   sqes;
    unsigned char   cqes;
    unsigned short  maxcmd;
    unsigned int    nn;
    unsigned short  oncs;
    unsigned short  fuses;
    unsigned char   fna;
    unsigned char   vwc;
    unsigned short  awun;
    unsigned short  awupf;
    unsigned char   nvscc;
    unsigned char   rsvd531;
    unsigned short  acwu;
    unsigned char   rsvd534[2];
    unsigned int    sgls;
    unsigned char   rsvd540[228];
    char            subnqn[256];
    unsigned char   rsvd1024[768];
    unsigned int    ioccsz;
    unsigned int    iorcsz;
    unsigned short  icdoff;
    unsigned char   ctrattr;
    unsigned char   msdbd;
    unsigned char   rsvd1804[244];
    nvme_id_power_state  psd[32];
    unsigned char   vs[1024];
} nvme_id_ctrl;

typedef struct nvme_smart_log {
    UInt8  critical_warning;
    UInt8  temperature[2];
    UInt8  avail_spare;
    UInt8  spare_thresh;
    UInt8  percent_used;
    UInt8  rsvd6[26];
    UInt8  data_units_read[16];
    UInt8  data_units_written[16];
    UInt8  host_reads[16];
    UInt8  host_writes[16];
    UInt8  ctrl_busy_time[16];
    UInt32 power_cycles[4];
    UInt32 power_on_hours[4];
    UInt32 unsafe_shutdowns[4];
    UInt32 media_errors[4];
    UInt32 num_err_log_entries[4];
    UInt32 warning_temp_time;
    UInt32 critical_comp_time;
    UInt16 temp_sensor[8];
    UInt32 thm_temp1_trans_count;
    UInt32 thm_temp2_trans_count;
    UInt32 thm_temp1_total_time;
    UInt32 thm_temp2_total_time;
    UInt8  rsvd232[280];
} nvme_smart_log;

// interface structure, obtained using lldb, could be incomplete or wrong
typedef struct IONVMeSMARTInterface
{
    IUNKNOWN_C_GUTS;
    
    UInt16 version;
    UInt16 revision;
    
    // NVMe smart data, returns nvme_smart_log structure
    IOReturn ( *SMARTReadData )( void *  interface,
                                struct nvme_smart_log * NVMeSMARTData );
    
    // NVMe IdentifyData, returns nvme_id_ctrl per namespace
    IOReturn ( *GetIdentifyData )( void *  interface,
                                  struct nvme_id_ctrl * NVMeIdentifyControllerStruct,
                                  unsigned int ns );
    
    // Always getting kIOReturnDeviceError
    IOReturn ( *GetFieldCounters )( void *   interface,
                                   char * FieldCounters );
    // Returns 0
    IOReturn ( *ScheduleBGRefresh )( void *   interface);
    
    // Always returns kIOReturnDeviceError, probably expects pointer to some
    // structure as an argument
    IOReturn ( *GetLogPage )( void *  interface, void * data, unsigned int, unsigned int);
    
    /* GetSystemCounters Looks like a table with an attributes. Sample result:
     
     0x101022200: 0x01 0x00 0x08 0x00 0x00 0x00 0x00 0x00
     0x101022208: 0x00 0x00 0x00 0x00 0x02 0x00 0x08 0x00
     0x101022210: 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
     0x101022218: 0x03 0x00 0x08 0x00 0xf1 0x74 0x26 0x01
     0x101022220: 0x00 0x00 0x00 0x00 0x04 0x00 0x08 0x00
     0x101022228: 0x0a 0x91 0xb1 0x00 0x00 0x00 0x00 0x00
     0x101022230: 0x05 0x00 0x08 0x00 0x24 0x9f 0xfe 0x02
     0x101022238: 0x00 0x00 0x00 0x00 0x06 0x00 0x08 0x00
     0x101022240: 0x9b 0x42 0x38 0x02 0x00 0x00 0x00 0x00
     0x101022248: 0x07 0x00 0x08 0x00 0xdd 0x08 0x00 0x00
     0x101022250: 0x00 0x00 0x00 0x00 0x08 0x00 0x08 0x00
     0x101022258: 0x07 0x00 0x00 0x00 0x00 0x00 0x00 0x00
     0x101022260: 0x09 0x00 0x08 0x00 0x00 0x00 0x00 0x00
     0x101022268: 0x00 0x00 0x00 0x00 0x0a 0x00 0x04 0x00
     .........
     0x101022488: 0x74 0x00 0x08 0x00 0x00 0x00 0x00 0x00
     0x101022490: 0x00 0x00 0x00 0x00 0x75 0x00 0x40 0x02
     0x101022498: 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
     */
    IOReturn ( *GetSystemCounters )( void *  interface, char *, unsigned int *);
    
    
    /* GetAlgorithmCounters returns mostly 0
     0x102004000: 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
     0x102004008: 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
     0x102004010: 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
     0x102004018: 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
     0x102004020: 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
     0x102004028: 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
     0x102004038: 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
     0x102004040: 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
     0x102004048: 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
     0x102004050: 0x00 0x00 0x00 0x00 0x80 0x00 0x00 0x00
     0x102004058: 0x80 0x00 0x00 0x00 0x00 0x00 0x00 0x00
     0x102004060: 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
     0x102004068: 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
     0x102004070: 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
     0x102004078: 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
     0x102004080: 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
     0x102004088: 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
     0x102004090: 0x00 0x01 0x00 0x00 0x00 0x00 0x00 0x00
     0x102004098: 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
     
     */
    IOReturn ( *GetAlgorithmCounters )( void *  interface, char *, unsigned int *);
} IONVMeSMARTInterface;


static inline uint16_t sg_get_unaligned_le16(const void *p)
{
    uint16_t u;
    memcpy(&u, p, 2);
    return u;
}

static inline __uint128_t sg_get_unaligned_le128(const void *p)
{
    __uint128_t u;
    memcpy(&u, p, 16);
    return u;
}

#endif /* nvme_h */
