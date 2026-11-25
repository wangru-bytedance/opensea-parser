//
// Ata_Farm_Types.h   Structures of ATA FARM specific structures. 
//
// Do NOT modify or remove this copyright and license
//
//Copyright (c) 2014 - 2026 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************
//
// \file Ata_Farm_Types.h  
#pragma once

#include <inttypes.h>
#include "Opensea_Parser_Helper.h"
#include "Farm_Types.h"

#pragma pack(push, 1)
typedef struct _sDriveInfo

{
    uint64_t        pageNumber;                                 //!< page Number 1
    uint64_t        copyNumber;                                 //!< copy number
    uint64_t        serialNumber;                               //!< Serial Number
    uint64_t        serialNumber2;                              //!< Serial Number 2
    uint64_t        worldWideName;                              //!< world wide name 
    uint64_t        worldWideName2;                             //!< world wide name 2
    uint64_t        deviceInterface;                            //!< device interface
    uint64_t        deviceCapacity;                             //!< 48-bit Device Capacity
    uint64_t        psecSize;                                   //!< Physical Sector Size in Bytes
    uint64_t        lsecSize;                                   //!< Logical Sector Size in Bytes
    uint64_t        deviceBufferSize;                           //!< Device Buffer Size in Bytes
    uint64_t        heads;                                      //!< Number of Heads
    uint64_t        factor;                                     //!< Device Form Factor (ID Word 168)
    uint64_t        rotationRate;                               //!< Rotational Rate of Device (ID Word 217)
    uint64_t        firmware;                                   //!< firmware 
    uint64_t        firmwareRev;                                //!< firmware Rev
    uint64_t        security;                                   //!< ATA Security State (ID Word 128)
    uint64_t        featuresSupported;                          //!< ATA Features Supported (ID Word 78)
    uint64_t        featuresEnabled;                            //!< ATA Features Enabled (ID Word 79)
    uint64_t        poh;                                        //!< Power-on Hours
    uint64_t        spoh;                                       //!< Spindle Power-on Hours
    uint64_t        headFlightHoursAct0;                        //!< Head Flight Hours, actuator 0
    uint64_t        headLoadEventsAct0;                         //!< Head Load Events, actuator 0
    uint64_t        powerCycleCount;                            //!< Power Cycle Count
    uint64_t        resetCount;                                 //!< Hardware Reset Count
    uint64_t        spinUpTime;                                 //!< SMART Spin-Up time in milliseconds
    uint64_t        NVC_StatusATPowerOn;                        //!< NVC Status on Power-on
    uint64_t        timeAvailable;                              //!< Time Available to Save User Data to Media Over Last Power Cycle (in 100us)
    uint64_t        timeStamp1;                                 //!< Timestamp of most recent SMART Summary Frame in Power-On Hours Milliseconds
    uint64_t        timeStamp2;                                 //!< Timestamp of last SMART Summary Frame in Power-On Hours Milliseconds
    uint64_t        timeToReady;                                //!< time to ready of the last power cycle
    uint64_t        timeHeld;                                   //!< time drive is held in staggered spin during the last power on sequence
    uint64_t        modelNumber[10];                            //!< lower 32 Model Number (started support in 2.14 )
    uint64_t        driveRecordingType;                         //!< 0 for SMR and 1 for CMR (started support in 2.15 )
    uint64_t        depopped;                                   //!< has the drive been depopped  - 1 = depopped and 0 = not depopped(started support in 2.15 )
    uint64_t        maxNumberForReasign;                        //!< Max Number of Available Sectors for Reassignment - Value in disc sectors(started in 3.3 )
    uint64_t        dateOfAssembly;                             //!< Date of Assembly in ASCII "YYWW" where YY is the year and WW is the week(started in 4.2)
    uint64_t        depopulationHeadMask;                       //!< Depopulation Head Mask(started in 4.7)
    uint64_t        headFlightHoursAct1;                        //!< Head Flight Hours, Actuator1
    uint64_t        headLoadEventsAct1;                         //!< Head Load Events, Actuator 1
    uint64_t        HAMRProtectStatus;                          //!< HAMR Data Protect Status: 1 = Data Protect, 0 = No Data Protect
    uint64_t        regenHeadMask;                              //!< Regen Head Mask: bitmap where 1 = bad head, 0 = good head
    uint64_t        POHMostRecentSave;                          //!< Power-on Hours of the most recent FARM Time series frame save
    uint64_t        POHSecondMostRecentSave;                    //!< Power-on Hours of the second most recent FARM Time series frame save
    uint64_t        SeqActiveZone;                              //!< Sequential Or Before Required for active zone configuration
    uint64_t        SeqWriteActiveZone;                         //!< Sequential Write Required for active zone configuration
    uint64_t        numLBA;                                     //!< Number of LBAs ( HSMR SWR capacity )
    int64_t         gpes[MAX_HEAD_COUNT];                       //!< Get Physical Element Status (GPES) by head

    _sDriveInfo() : pageNumber(0), copyNumber(0), serialNumber(0), serialNumber2(0), worldWideName(0), worldWideName2(0),
        deviceInterface(0), deviceCapacity(0), psecSize(0), lsecSize(0), deviceBufferSize(0), heads(0), factor(0),
        rotationRate(0), firmware(0), firmwareRev(0), security(0), featuresSupported(0), featuresEnabled(0),
        poh(0), spoh(0), headFlightHoursAct0(0), headLoadEventsAct0(0), powerCycleCount(0), resetCount(0),
        spinUpTime(0), NVC_StatusATPowerOn(0), timeAvailable(0), timeStamp1(0), timeStamp2(0), timeToReady(0), timeHeld(0),
#if defined __cplusplus && __cplusplus >= 201103L
        modelNumber{ 0 },
#endif
        driveRecordingType(0), depopped(0), maxNumberForReasign(0), dateOfAssembly(0), depopulationHeadMask(0), headFlightHoursAct1(0),
        headLoadEventsAct1(0), HAMRProtectStatus(0), regenHeadMask(0), POHMostRecentSave(0), POHSecondMostRecentSave(0),
        SeqActiveZone(0), SeqWriteActiveZone(0), numLBA(0)
#if defined __cplusplus && __cplusplus >= 201103L
        ,gpes{ 0 } 
#endif
        {};
}sDriveInfo;

typedef struct _sErrorStat
{
    uint64_t         pageNumber;                                //!< Page Number = 3
    uint64_t         copyNumber;                                //!< Copy Number
    uint64_t         totalReadECC;                              //!< Number of Unrecoverable Read Errors
    uint64_t         totalWriteECC;                             //!< Number of Unrecoverable Write Errors
    uint64_t         totalReallocations;                        //!< Number of Reallocated Sectors
    uint64_t         totalReadRecoveryAttepts;                  //!< Number of Read Recovery Attempts
    uint64_t         totalMechanicalFails;                      //!< Number of Mechanical Start Failures
    uint64_t         totalReallocatedCanidates;                 //!< Number of Reallocated Candidate Sectors2
    uint64_t         totalASREvents;                            //!< Number of ASR Events3
    uint64_t         totalCRCErrors;                            //!< Number of Interface CRC Errors3
    uint64_t         attrSpinRetryCount;                        //!< Spin Retry Count 
    uint64_t         normalSpinRetryCount;                      //!< Spin Retry Count 
    uint64_t         worstSpinRretryCount;                      //!< Spin Retry Count 
    uint64_t         attrIOEDCErrors;                           //!< Number of IOEDC Errors 
    uint64_t         attrCTOCount;                              //!< CTO Count Total 
    uint64_t         overfiveSecCTO;                            //!< CTO Count Over 5s 
    uint64_t         oversevenSecCTO;                           //!< CTO Count Over 7.5s 
    uint64_t         totalFlashLED;                             //!< Total Flash LED (Assert) Events
    uint64_t         indexFlashLED;                             //!< index of the last Flash LED of the array
    uint64_t         uncorrectables;                            //!< uncorrecatables errors (sata only)
    uint64_t         reserved1;                                 //!< reserved
    uint64_t         flashLEDArray[FLASH_EVENTS];               //!< Info on the last 8 Flash LED events Wrapping array. (added 2.7)
    uint64_t         readWriteRetry[FLASH_EVENTS];              //!< Info on the last 8 read/write Retry events (added 2.7)
    uint64_t         reserved2[2];                              //!< reserved [2] (added 3.0)
    uint64_t         reallocatedSectors[REALLOCATIONEVENTS];    //!< Reallocated Secotors by cause(added 3.0)
    uint64_t         timestampForLED[FLASH_EVENTS];             //!< Universal Timestamp (us) of last 8 Flash LED (assert) Events, wrapping array (added 4.7)
    uint64_t         powerCycleOfLED[FLASH_EVENTS];             //<! Power Cycle of the last 8 Flash LED (assert) Events, wrapping array (added 4.7)
    uint64_t         cumLifeTimeECCReadDueErrorRecovery;        //<! Cumulative Lifetime Unrecoverable Read errors due to Error Recovery Control 
    uint64_t         cumLifeUnRecoveralbeReadByhead[MAX_HEAD_COUNT];        //<! Cumulative Lifetime Unrecoverable Read Repeating by head
    uint64_t         cumLiveUnRecoveralbeReadUnique[MAX_HEAD_COUNT];        //<! Cumulative Lifetime Unrecoverable Read Unique by head
//version 4.21
    uint64_t         reallocSectorsAct1;                        //!< Number of Reallocated sectors, Actuator 1
    uint64_t         reallocCandidatesAct1;                     //!< Number of Reallocation Candidate Sectors, Actuator 1
    uint64_t         totalFlashLEDEvents;                       //!< Total Flash LED (Assert) Events, Actuator 1 
    uint64_t         lastIDXFLEDInfoAct1;                       //!< Index of last entry in FLED Info array below, in case the array wraps, Actuator 1
    uint64_t         last8FLEDEventsAct1[FLASH_EVENTS];         //!< Info on the last 8 Flash LED (assert) Events, wrapping array, Actuator 1
    uint64_t         last8ReadWriteRetryEvts[FLASH_EVENTS];     //!< Info on the last 8 Read/Write Retry events, wrapping array, Actuator 1
    uint64_t         reallocSectorsByCauseAct1[REALLOCATIONEVENTS]; //!< Reallocated sectors by cause, Actuator 1
    uint64_t         last8FLEDEvtsAct1[FLASH_EVENTS];           //!< Universal Timestamp (us) of last 8 Flash LED (assert) Events, wrapping array, Actuator 1
    uint64_t         last8FLEDEvtsPowerCycleAct1[FLASH_EVENTS]; //!< Power Cycle of the last 8 Flash LED (assert) Events, wrapping array, Actuator 1
//version 4.41
    uint64_t         pfaAttribute1;                             //!< SATA PFA Attributes 1
    uint64_t         pfaAttribute2;                             //!< SATA PFA Attributes 2
    uint64_t         lastReallocatedSectorsAtc0;                //!< Number of reallocated sectors since the last FARM Time series Frame save
    int64_t          betweenReallocatedSectorsAct0;             //!< Number of reallocated sectors between FARM time series Frame N and N-1
    uint64_t         lastCandidateSectorsAct0;                  //!< Number of reallocation candidate sectors since the last FARM Time series Frame save
    uint64_t         betweenCandidateSectorsAct0;               //!< Number of reallocation candidate sectors between FARM time series Frame N and N-1
    uint64_t         lastReallocatedSectorsAct1;                //!< Number of reallocated sectors since the last FARM Time series Frame save, Actuator 1
    uint64_t         betweenReallocatedSectorsAct1;             //!< Number of reallocated sectors between FARM time series Frame N and N-1, Actuator 1
    uint64_t         lastCandidateSectorsAct1;                  //!< Number of reallocation candidate sectors since the last FARM Time series Frame save, Actuator 1
    uint64_t         betweenCandidateSectorsAct1;               //!< Number of reallocation candidate sectors between FARM time series Frame N and N-1, Actuator 1
    uint64_t         lastUniqueURE[MAX_HEAD_COUNT];             //!< Number of Unique Unrecoverable sectors since the last FARM Time series Frame, by head
    uint64_t         betweenUniqueURE[MAX_HEAD_COUNT];          //!< Number of Unique Unrecoverable sectors between FARM time series Frame N and N-1, by head

    _sErrorStat() : pageNumber(0), copyNumber(0), totalReadECC(0), totalWriteECC(0), totalReallocations(0), totalReadRecoveryAttepts(0),
        totalMechanicalFails(0), totalReallocatedCanidates(0), totalASREvents(0), totalCRCErrors(0), attrSpinRetryCount(0),
        normalSpinRetryCount(0), worstSpinRretryCount(0), attrIOEDCErrors(0), attrCTOCount(0), overfiveSecCTO(0),
        oversevenSecCTO(0), totalFlashLED(0), indexFlashLED(0), uncorrectables(0), reserved1(0),
#if defined __cplusplus && __cplusplus >= 201103L
        flashLEDArray{0}, readWriteRetry{ 0 }, reserved2{ 0 }, reallocatedSectors{ 0 }, timestampForLED{ 0 }, powerCycleOfLED{ 0 },
#endif
        cumLifeTimeECCReadDueErrorRecovery(0),
#if defined __cplusplus && __cplusplus >= 201103L
        cumLifeUnRecoveralbeReadByhead{ 0 }, cumLiveUnRecoveralbeReadUnique{ 0 },
#endif
        reallocSectorsAct1(0), reallocCandidatesAct1(0), totalFlashLEDEvents(0), lastIDXFLEDInfoAct1(0)
#if defined __cplusplus && __cplusplus >= 201103L
        ,last8FLEDEventsAct1{ 0 }, last8ReadWriteRetryEvts{ 0 }, reallocSectorsByCauseAct1{0}, last8FLEDEvtsAct1{ 0 }, last8FLEDEvtsPowerCycleAct1{ 0 }
#endif 
        ,pfaAttribute1(0), pfaAttribute2(0), lastReallocatedSectorsAtc0(0), betweenReallocatedSectorsAct0(0), lastCandidateSectorsAct0(0),
        betweenCandidateSectorsAct0(0), lastReallocatedSectorsAct1(0), betweenReallocatedSectorsAct1(0), lastCandidateSectorsAct1(0), 
        betweenCandidateSectorsAct1(0)
#if defined __cplusplus && __cplusplus >= 201103L
        , lastUniqueURE{ 0 }, betweenUniqueURE{ 0 } 
#endif
            {};
}sErrorStat;

typedef struct _sEnvironementStat
{
    uint64_t         pageNumber;                                //!< Page Number = 4
    uint64_t         copyNumber;                                //!< Copy Number
    uint64_t         curentTemp;                                //!< Current Temperature in Celsius
    uint64_t         highestTemp;                               //!< Highest Temperature in Celsius
    uint64_t         lowestTemp;                                //!< Lowest Temperature
    uint64_t         averageTemp;                               //!< Average Short Term Temperature
    uint64_t         averageLongTemp;                           //!< Average Long Term Temperature
    uint64_t         highestShortTemp;                          //!< Highest Average Short Term Temperature
    uint64_t         lowestShortTemp;                           //!< Lowest Average Short Term Temperature
    uint64_t         highestLongTemp;                           //!< Highest Average Long Term Temperature
    uint64_t         lowestLongTemp;                            //!< Lowest Average Long Term Temperature
    uint64_t         overTempTime;                              //!< Time In Over Temperature
    uint64_t         underTempTime;                             //!< Time In Under Temperature
    uint64_t         maxTemp;                                   //!< Specified Max Operating Temperature
    uint64_t         minTemp;                                   //!< Specified Min Operating Temperature
    uint64_t         shockEvents;                               //!< Over-Limit Shock Events Count
    uint64_t         hfWriteCounts;                             //!< High Fly Write Count
    uint64_t         humidity;                                  //!< Current Relative Humidity (in units of .1%)
    uint64_t         humidityRatio;                             //!< Humidity Mixed Ratio multiplied by 8 (divide by 8 to get actual value)
    uint64_t         currentMotorPower;                         //!< Current Motor Power, value from most recent SMART Summary Frame
    uint64_t         current12v;                                //!< Current 12V input (added 3.7)
    uint64_t         min12v;                                    //!< Minimum 12V input from last 3 SMART Summary Frames (3.7)
    uint64_t         max12v;                                    //!< Maximum 12V input from last 3 SMART Summary Frames (3.7)
    uint64_t         current5v;                                 //!< Current 5V input (3.7)
    uint64_t         min5v;                                     //!< Minimum 5V input from last 3 SMART Summary Frames (3.7)
    uint64_t         max5v;                                     //!< Maximum 5V input from last 3 SMART Summary Frames (3.7)
    uint64_t         powerAvg12v;                               //!< 12V Power Average(mw) - Average of the three summary frames (4.3)
    uint64_t         powerMin12v;                               //!< 12V Power Min(mw) - Lowest of last 3 SMART summary frames (4.3)
    uint64_t         powerMax12v;                               //!< 12V Power Max(mw) - Highest of last 3 SMART summary frames (4.3)
    uint64_t         powerAvg5v;                                //!< 5V Power Average(mw) - Average of the three summary frames (4.3)
    uint64_t         powerMin5v;                                //!< 5V Power Min(mw) - Lowest of last 3 SMART summary frames (4.3)
    uint64_t         powerMax5v;                                //!< 5V Power Max(mw) - Highest of last 3 SMART summary frames (4.3)
    uint64_t         currLFVibeAct0;                            //!< Current Low Frequency Vibe Score - Actuator 0
    uint64_t         currMFVibeAct0;                            //!< Current Mid Frequency Vibe Score - Actuator 0
    uint64_t         currHFVibeAct0;                            //!< Current High Frequency Vibe Score - Actuator 0
    uint64_t         worstLFVibeAct0;                           //!< Worst Low Frequency Vibe Score - Actuator 0
    uint64_t         worstMFVibeAct0;                           //!< Worst Mid Frequency Vibe Score - Actuator 0
    uint64_t         worstHFVibeAct0;                           //!< Worst High Frequency Vibe Score - Actuator 0
    uint64_t         currLFVibeAct1;                            //!< Current Low Frequency Vibe Score - Acutator 1
    uint64_t         currMFVibeAct1;                            //!< Current Mid Frequency Vibe Score - Actuator 1
    uint64_t         currHFVibeAct1;                            //!< Current High Frequency Vibe Score - Actuator 1
    uint64_t         worstLFVibeAct1;                           //!< Worst Low Frequency Vibe Score - Actuator 1
    uint64_t         worstMFVibeAct1;                           //!< Worst Mid Frequency Vibe Score - Actuator 1
    uint64_t         worstHFVibeAct1;                           //!< Worst High Frequency Vibe Score - Actuator 1


    _sEnvironementStat() : pageNumber(0), copyNumber(0), curentTemp(0), highestTemp(0), lowestTemp(0), averageTemp(0), averageLongTemp(0),
        highestShortTemp(0), lowestShortTemp(0), highestLongTemp(0), lowestLongTemp(0), overTempTime(0), underTempTime(0), maxTemp(0),
        minTemp(0), shockEvents(0), hfWriteCounts(0), humidity(0), humidityRatio(0), currentMotorPower(0), current12v(0), min12v(0), max12v(0),
        current5v(0), min5v(0), max5v(0), powerAvg12v(0), powerMin12v(0), powerMax12v(0), powerAvg5v(0), powerMin5v(0), powerMax5v(0),
        currLFVibeAct0(0), currMFVibeAct0(0), currHFVibeAct0(0), worstLFVibeAct0(0), worstMFVibeAct0(0), worstHFVibeAct0(0),
        currLFVibeAct1(0), currMFVibeAct1(0), currHFVibeAct1(0), worstLFVibeAct1(0), worstMFVibeAct1(0), worstHFVibeAct1(0) {};

}sEnvironementStat;

typedef struct _sHeadInfo
{
    uint32_t        decimalPartofFloat;                         //!< data after the decimal
    int16_t         wholePartofFloat;                           //!< the whole number
    uint8_t         reseved;
    uint8_t         status;                                     //!< status bit is set here
    _sHeadInfo() : decimalPartofFloat(0), wholePartofFloat(0), reseved(0), status(0) {};
}sHeadInfo;

typedef struct _sflyHeight
{
    int64_t        outer;
    int64_t        inner;
    int64_t        middle;
    
    _sflyHeight() :outer(0), inner(0), middle(0) {};
}sflyHeight;

typedef struct _H2SAT
{
    int64_t        zone0;
    int64_t        zone1;
    int64_t        zone2;

    _H2SAT() : zone0(0), zone1(0), zone2(0) {};
}H2SAT;

typedef struct _sAtaReliabilityStat
{
    uint64_t        pageNumber;                                     //!< Page Number = 5
    uint64_t        copyNumber;                                     //!< Copy Number
    int64_t         lastIDDTest;                                    //!< Timestamp of last IDD test
    int64_t         cmdLastIDDTest;                                 //!< Sub-command of last IDD test
    uint64_t        discSlip[MAX_HEAD_COUNT];                       //!< [24] Disc Slip in micro-inches by Head
    uint64_t        bitErrorRate[MAX_HEAD_COUNT];                   //!< [24] Bit Error Rate of Zone 0 by Drive Head
    int64_t         gListReclamed;                                  //!< Number of G-List Reclamations
    int64_t         servoStatus;                                    //!< Servo Status (follows standard DST error code definitions)
    int64_t         altsBeforeIDD;                                  //!< Number of Alt List Entries Before IDD Scan
    int64_t         altsAfterIDD;                                   //!< Number of Alt List Entries After IDD Scan
    int64_t         gListBeforIDD;                                  //!< Number of Resident G-List Entries Before IDD Scan
    int64_t         gListAfterIDD;                                  //!< Number of Resident G-List Entries After IDD Scan
    int64_t         scrubsBeforeIDD;                                //!< Number of Scrub List Entries Before IDD Scan
    int64_t         scrubsAfterIDD;                                 //!< Number of Scrub List Entries After IDD Scan
    int64_t         numberDOSScans;                                 //!< Number of DOS Scans Performed
    int64_t         numberLBACorrect;                               //!< Number of LBAs Corrected by ISP
    int64_t         numberValidParitySec;                           //!< Number of Valid Parity Sectors
    int64_t         dosWriteCount[MAX_HEAD_COUNT];                  //!< [24] DOS Write Refresh Count7
    int64_t         numberRAWops;                                   //!< Number of RAW Operations
    int64_t         DVGASkipWriteDetect[MAX_HEAD_COUNT];            //!< [24] DVGA Skip Write Detect by Head
    int64_t         RVGASkipWriteDetect[MAX_HEAD_COUNT];            //!< [24] RVGA Skip Write Detect by Head
    int64_t         FVGASkipWriteDetect[MAX_HEAD_COUNT];            //!< [24] FVGA Skip Write Detect by Head
    int64_t         skipWriteDetectThresExceeded[MAX_HEAD_COUNT];   //!< [24] Skip Write Detect Threshold Exceeded Count by Head
    int64_t         attrErrorRateRaw;                               //!< Error Rate 
    int64_t         attrErrorRateNormal;                            //!< Error Rate 
    int64_t         attrErrorRateWorst;                             //!< Error Rate 
    int64_t         attrSeekErrorRateRaw;                           //!< Seek Error Rate 
    int64_t         attrSeekErrorRateNormal;                        //!< Seek Error Rate 
    int64_t         attrSeekErrorRateWorst;                         //!< Seek Error Rate 
    int64_t         attrUnloadEventsRaw;                            //!< High Priority Unload Events 
    uint64_t        microActuatorLockOUt;                           //!< Micro Actuator Lock-out, head mask accumulated over last 3 Summary Frames
    int64_t         sineACFF[MAX_HEAD_COUNT];                       //!< [24] ACFF Sine 1X, value from most recent SMART Summary Frame by Head
    int64_t         cosineACFF[MAX_HEAD_COUNT];                     //!< [24] ACFF Cosine 1X, value from most recent SMART Summary Frame by Head
    int64_t         PZTCalibration[MAX_HEAD_COUNT];                 //!< [24] PZT Calibration, value from most recent SMART Summary Frame by Head
    int64_t         MRHeadResistance[MAX_HEAD_COUNT];               //!< [24] MR Head Resistance from most recent SMART Summary Frame by Head
    int64_t         numberOfTMD[MAX_HEAD_COUNT];                    //!< [24] Number of TMD over last 3 SMART Summary Frames by Head
    int64_t         velocityObserver[MAX_HEAD_COUNT];               //!< [24] Velocity Observer over last 3 SMART Summary Frames by Head
    int64_t         numberOfVelocityObserver[MAX_HEAD_COUNT];       //!< [24] Number of Velocity Observer over last 3 SMART Summary Frames by Head
    H2SAT           currentH2SAT[MAX_HEAD_COUNT];                   //!< [24] Current H2SAT trimmed mean bits in error by Head, by Test Zone 
    H2SAT           currentH2SATIterations[MAX_HEAD_COUNT];         //!< [24] Qword[24][3] Current H2SAT iterations to converge by Head, by Test Zone 
    int64_t         currentH2SATPercentage[MAX_HEAD_COUNT];         //!< [24] Qword[24] Current H2SAT percentage of codewords at iteration level by Head, averaged
    int64_t         currentH2SATamplitude[MAX_HEAD_COUNT];          //!< [24] Qword[24] Current H2SAT amplitude by Head, averaged across Test Zones 
    int64_t         currentH2SATasymmetry[MAX_HEAD_COUNT];          //!< [24] Qword[24] Current H2SAT asymmetry by Head, averaged across Test Zones
    sflyHeight      flyHeightClearance[MAX_HEAD_COUNT];             //!< [24][3] Applied fly height clearance delta per head in thousandths of one Angstrom.
    int64_t         diskSlipRecalPerformed;                         //!< Number of disc slip recalibrations performed
    int64_t         gList[MAX_HEAD_COUNT];                          //!< [24] Number of Reallocated Sectors per head
    int64_t         pendingEntries[MAX_HEAD_COUNT];                 //!< [24] Number of Reallocation Candidate Sectors per head
    int64_t         heliumPresureTrip;                              //!< Helium Pressure Threshold Tripped ( 1- trip, 0 - no trip)
    int64_t         oughtDOS[MAX_HEAD_COUNT];                       //!< [24] DOS Ought to scan count per head
    int64_t         needDOS[MAX_HEAD_COUNT];                        //!< [24] DOS needs to scanns count per head
    int64_t         writeDOSFault[MAX_HEAD_COUNT];                  //!< [24] DOS  write Fault scans per head
    int64_t         writePOH[MAX_HEAD_COUNT];                       //!< [24] write POS in sec value from most recent SMART Frame by head
    int64_t         RVAbsoluteMean;                                 //!< RV Absolute Mean, value from the most recent SMART Frame
    int64_t         maxRVAbsluteMean;                               //!< Max RV Absolute Mean, value from the most recent SMART Summary Frame
    int64_t         idleTime;                                       //!< idle Time, Value from most recent SMART Summary Frame
    int64_t         countDOSWrite[MAX_HEAD_COUNT];                  //!< [24] DOS Write Count Threshold per head
    int64_t         secondMRHeadResistance[MAX_HEAD_COUNT];         //!< [24] Second Head, MR Head Resistance from most recent SMART Summary Frame by Head (added in 2.11)
    int64_t         FAFHMeasurementStatus[MAX_HEAD_COUNT];          //!< [24] FAFH Measurement Status, bitwise OR across all diameters per head (added in 4.5)
    int64_t         FAFHRelativeAmptitude[MAX_HEAD_COUNT];          //!< [24] FAFH HF/LF Relative Amplitude in tenths, maximum value across all 3 zones per head (added in 4.5)
    sflyHeight      FAFHBitErrorRate[MAX_HEAD_COUNT];               //!< [24][3] FAFH Bit Error Rate, write then read BER on reserved tracks (added in 4.5)
    sflyHeight      FAFHLowFrequency[MAX_HEAD_COUNT];               //<! [24][3] FAFH Low Frequency Passive Clearance in ADC counts (added in 4.5)
    sflyHeight      FAFHHighFrequency[MAX_HEAD_COUNT];              //<! [24][3] FAFH High Frequency Passive Clearance in ADC counts (added in 4.5)
    uint64_t        numberLBACorrectedByParitySector;               //<! Number of LBAs Corrected by Parity Sector Actuator 0
    //4.21
    uint64_t        SuperParityCovPercent;                          //!< Primary Super Parity Coverage Percentae, Actuator 0  added 4.21
    uint64_t        numberOfLFAIterations[MAX_HEAD_COUNT];          //!< Number of total Laser Field Adjust iterations performed per head
    sflyHeight      laserOperCurrent[MAX_HEAD_COUNT];               //!< Laser Operating Current by head Diameter 0: Outer Diameter 1: Inner Diameter 2: Middle
    sflyHeight      postLFABER[MAX_HEAD_COUNT];                     //!< Post LFA Optimal BER by head Diameter 0: Outer Diameter 1: Inner Diameter 2: Middle
    uint64_t        lastIDDTimeAct1;                                //!< Timestamp of last IDD test in Hours(POH), Actuator 1
    uint64_t        cmdLastIDDTestAct1;                             //!< Sub Command of last IDD Test, Actuator 1
    uint64_t        reallocSectorReclamAct1;                        //!< Number of Reallocated Sector Reclamations, Actuator 1
    uint64_t        servoStatusAct1;                                //!< Servo Status (follows standard DST error code definitions), Actuator 1
    uint64_t        slippedSectorsBefIDDAct1;                       //!< Number of Slipped Sectors Before IDD Scan, Actuator 1
    uint64_t        slippedSectorsAftIDDAct1;                       //!< Number of Slipped Sectors After IDD Scan, Actuator 1
    uint64_t        resReallocSectorsBefIDDAct1;                    //!< Number of Resident Reallocated Sectors Before IDD Scan, Actuator 1
    uint64_t        resReallocSectorsAftIDDAct1;                    //!< Number of Resident Reallocated Sectors After IDD Scan, Actuator 1
    uint64_t        scrubbedSectorsBefIDDAct1;                      //!< Number of Successfully Scrubbed Sectors Before IDD Scan, Actuator 1
    uint64_t        scrubbedSectorsAftIDDAct1;                      //!< Number of Successfully Scrubbed Sectors After IDD Scan, Actuator 1
    uint64_t        DOSScansAct1;                                   //!< Number of DOS Scans Performed, Actuator 1
    uint64_t        correctedLBAsAct1;                              //!< Number of LBAs Corrected by ISP, Acuator 1
    uint64_t        validParitySectAct1;                            //!< Number of Valid Parity Sectors, Actuator 1
    uint64_t        rvAbsMeanAct1;                                  //!< RV Absolute Mean, value from most recent SMART Summary Frame in rad/s^2, Actuator 1
    uint64_t        rvAbsMeanMaxAct1;                               //!< Max RV Absolute Mean, value from most recent SMART Summary Frame in rad/s^2, Actuator 1
    uint64_t        idleTimeAct1;                                   //!< Idle Time, value from most recent SMART Summary Frame in seconds, Actuator 1
    uint64_t        numberLBACorrectedByParitySectorAct1;           //!< Number of LBAs Corrected by Parity Sector, Actuator 1
    uint64_t        superParityCovPercentAct1;                      //!< Primary Super Parity Coverage Percentage, Actuator 1
    uint64_t        numOfReaderWriterOffset[MAX_HEAD_COUNT];        //!< Qword[24]	Number of Reader Writer offset Iterations by head
    H2SAT           microJogOffset[MAX_HEAD_COUNT];                 //!< Qword[24][3]	Micro Jog Offset by head
    sflyHeight      preLFABitErrorRate[MAX_HEAD_COUNT];             //!< Qword[24][3]	Pre LFA Bit Error Rate
    sflyHeight      zeroPercentShiftErrorRate[MAX_HEAD_COUNT];      //!< Qword[24][3]	Zero Percent Shift Bit Error Rate
    uint64_t        superParityCoveragePercentageAct0;              //!< Primary Super Parity Coverage Percentage SMR/SWR, Actuator 0
    uint64_t        superParityCoveragePercentageAct1;              //!< Primary Super Parity Coverage Percentage SMR/SWR, Actuator 1
    //4.41
    uint64_t        lifetimeWrites[MAX_HEAD_COUNT];                 //!< Lifetime Terabytes Written per head

    _sAtaReliabilityStat() : pageNumber(0), copyNumber(0), lastIDDTest(0), cmdLastIDDTest(0),
#if defined __cplusplus && __cplusplus >= 201103L
        discSlip{ 0 }, bitErrorRate{ 0 },
#endif
        gListReclamed(0), servoStatus(0), altsBeforeIDD(0), altsAfterIDD(0), gListBeforIDD(0), gListAfterIDD(0), scrubsBeforeIDD(0), scrubsAfterIDD(0), numberDOSScans(0),
        numberLBACorrect(0), numberValidParitySec(0),
#if defined __cplusplus && __cplusplus >= 201103L
        dosWriteCount{ 0 },
#endif
        numberRAWops(0),
#if defined __cplusplus && __cplusplus >= 201103L
        DVGASkipWriteDetect{ 0 }, RVGASkipWriteDetect{ 0 }, FVGASkipWriteDetect{ 0 }, skipWriteDetectThresExceeded{ 0 },
#endif
        attrErrorRateRaw(0), attrErrorRateNormal(0), attrErrorRateWorst(0), attrSeekErrorRateRaw(0), attrSeekErrorRateNormal(0), \
        attrSeekErrorRateWorst(0), attrUnloadEventsRaw(0), microActuatorLockOUt(0),
#if defined __cplusplus && __cplusplus >= 201103L   
        sineACFF{ 0 }, cosineACFF{ 0 }, PZTCalibration{ 0 }, MRHeadResistance{ 0 }, numberOfTMD{ 0 },
        velocityObserver{ 0 }, numberOfVelocityObserver{ 0 }, currentH2SAT{ }, currentH2SATIterations{ }, currentH2SATPercentage{ 0 },
        currentH2SATamplitude{ 0 }, currentH2SATasymmetry{ 0 }, flyHeightClearance{ },
#endif
        diskSlipRecalPerformed(0),
#if defined __cplusplus && __cplusplus >= 201103L  
        gList{ 0 }, pendingEntries{ 0 },
#endif
        heliumPresureTrip(0),
#if defined __cplusplus && __cplusplus >= 201103L 
        oughtDOS{ 0 }, needDOS{ 0 }, writeDOSFault{ 0 }, writePOH{ 0 },
#endif
        RVAbsoluteMean(0), maxRVAbsluteMean(0), idleTime(0),
#if defined __cplusplus && __cplusplus >= 201103L

        countDOSWrite{ 0 }, secondMRHeadResistance{ 0 }, FAFHMeasurementStatus{ 0 }, FAFHRelativeAmptitude{ 0 },
        FAFHBitErrorRate{ }, FAFHLowFrequency{ }, FAFHHighFrequency{ },
#endif
        numberLBACorrectedByParitySector(0), SuperParityCovPercent(0),
#if defined __cplusplus && __cplusplus >= 201103L
        numberOfLFAIterations{ 0 }, laserOperCurrent{ }, postLFABER{ },
#endif    
        lastIDDTimeAct1(0), cmdLastIDDTestAct1(0), reallocSectorReclamAct1(0), servoStatusAct1(0), slippedSectorsBefIDDAct1(0), slippedSectorsAftIDDAct1(0),
        resReallocSectorsBefIDDAct1(0), resReallocSectorsAftIDDAct1(0), scrubbedSectorsBefIDDAct1(0), scrubbedSectorsAftIDDAct1(0),
        DOSScansAct1(0), correctedLBAsAct1(0), validParitySectAct1(0), rvAbsMeanAct1(0),
        rvAbsMeanMaxAct1(0), idleTimeAct1(0), numberLBACorrectedByParitySectorAct1(0), superParityCovPercentAct1(0),
#if defined __cplusplus && __cplusplus >= 201103L
        numOfReaderWriterOffset{ 0 }, microJogOffset{ }, preLFABitErrorRate{ }, zeroPercentShiftErrorRate{ },
#endif
        superParityCoveragePercentageAct0(0), superParityCoveragePercentageAct1(0)
#if defined __cplusplus && __cplusplus >= 201103L
        , lifetimeWrites{ 0 }
#endif
        {};
}sAtaReliabilityStat;

typedef struct _sFarmFrame
{
    sDriveInfo              driveInfo;                          //<! drive information page
    sStringIdentifyData     identStringInfo;                    //<! all the string information from the drive information page
    sWorkLoadStat           workLoadPage;                       //<! work load data page
    sErrorStat              errorPage;                          //<! error data page
    sEnvironementStat       environmentPage;                    //<! environment information page 
    sAtaReliabilityStat     reliPage;                           //<! reliability information page
}sFarmFrame;
#pragma pack(pop)
