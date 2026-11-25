//
// CFARM_WLM_Log.cpp
//
// Do NOT modify or remove this copyright and confidentiality notice.
//
// Copyright 2012 - 2023 Seagate Technology LLC.
//
// The code contained herein is CONFIDENTIAL to Seagate Technology LLC
// and may be covered under one or more Non-Disclosure Agreements.
// All or portions are also trade secret.
// Any use, modification, duplication, derivation, distribution or disclosure
// of this code, for any reason, not expressly authorized is prohibited.
// All other rights are expressly reserved by Seagate Technology LLC.
//
// *****************************************************************************

#include "CFARM_WLM_Log.h"   
#include <algorithm>
#include <cstring>

using namespace opensea_parser;
//-----------------------------------------------------------------------------
//
//! \fn   CFARMWLM()
//
//! \brief
//!   Description: Default Class constructor for the CFARMWLM
//
//  Entry:
//
//  Exit:
//!  \return NONE
//
//---------------------------------------------------------------------------
CFARMWLM::CFARMWLM()
    :pData(NULL)
    , m_name("WLM Log")
    , m_logSize(0)
    , m_version(0)
    , m_WLMstatus(eReturnValues::IN_PROGRESS)
    , m_DataHeader()
    , m_DataHeaderPTR(NULL)
{

}
//-----------------------------------------------------------------------------
//
//! \fn   CFARMWLM()
//
//! \brief
//!   Description:  Class constructor for the CWLMLog use for cdf and other logs \n
//!     where the log is part of another log
//
//  Entry:
//! \param bufferData - pointer to the buffer data for parsing
//! \param masterData - pointer to the json data
//! \param interfaceType - is this drive sas or sata
//
//  Exit:
//!  \return NONE
//
//---------------------------------------------------------------------------
CFARMWLM::CFARMWLM( uint8_t *bufferData, uint32_t dataSize, JSONNODE *masterData)
    :pData(bufferData)
    , m_name("WLM Log")
    , m_logSize(dataSize)
    , m_version(0)
    , m_WLMstatus(eReturnValues::IN_PROGRESS)
    , m_DataHeader()
    , m_DataHeaderPTR(NULL)
{
    if (bufferData != NULL && masterData != NULL)
    {
        m_DataHeaderPTR = &m_DataHeader;
        if (validate_WLM())
        {
            try
            {
                m_WLMstatus = parse_WLM_Summary(masterData);
            }
            catch (...)
            {
                m_WLMstatus = eReturnValues::PARSING_EXCEPTION_FAILURE;
            }

        }
        else
        {
            print_Validation_Error(masterData);
            m_WLMstatus = eReturnValues::VALIDATION_FAILURE;
        }
    }
    else
    {
        if (masterData != NULL)
        {
            print_Empty_WLM_Log(masterData);
        }
        m_WLMstatus = eReturnValues::BAD_PARAMETER;
    }
    pData = NULL; // Borrowed buffer; ownership remains with the caller.
   // _CrtDumpMemoryLeaks();
}

//-----------------------------------------------------------------------------
//
//! \fn   ~CFARMWLM()
//
//! \brief
//!   Description:  Class deconstructor for for the CWLMLog
//
//  Entry:
//
//  Exit:
//!  \return NONE
//
//---------------------------------------------------------------------------
CFARMWLM::~CFARMWLM()
{
    if (!vFrame.empty())
    {
        vFrame.clear();
        vFrame.swap(vBlankFrame);
    }
    pData = NULL;
    //  _CrtDumpMemoryLeaks();
}
//-----------------------------------------------------------------------------
//
//   is_FARM()
//
//! \brief
//!   Description:  Check the signature to see if it is an SSD drive vs HD
//
//  Entry:
//! \param signature - 32 bit holding the signature of the log
//
//  Exit:
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
bool CFARMWLM::is_FARM(uint32_t signature)
{
    if (signature == FARMWLM)
    {
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
//
//   validate_WLM()
//
//! \brief
//!   Description:  Check the header and footer singatures for validation
//
//  Entry:
//
//  Exit:
//!   \return bool 
//
//---------------------------------------------------------------------------
bool CFARMWLM::validate_WLM()
{
    // Validate every slot before decoding any commands.
    if (pData == NULL || m_logSize == 0 || m_logSize % TRACESIZE != 0)
    {
        m_validationError = "Input length is not a nonzero multiple of the legacy 0x80000-byte frame slot";
        return false;
    }
    for (size_t offset = 0; offset < m_logSize; offset += TRACESIZE)
    {
        m_validationOffset = offset;
        if (!get_WLM_Header_Data(offset))
        {
            m_validationError = "Invalid legacy WLM frame size or truncated header";
            return false;
        }
        if (!is_FARM(m_DataHeader.signature))
        {
            m_validationError = "Legacy WLM header signature 3C 48 54 57 is absent at the frame start";
            return false;
        }
        if (m_DataHeader.rev != WLM3 && m_DataHeader.rev != WLM6)
        {
            m_validationError = "Legacy WLM revision is neither 3 nor 6";
            return false;
        }
        if (!check_For_Footer_Signature(offset + TRACESIZE - 8))
        {
            m_validationError = "Legacy WLM footer is absent at frame offset 0x7FFF8";
            return false;
        }
    }
    return true;
}

void CFARMWLM::print_Validation_Error(JSONNODE* wlm)
{
    JSONNODE* info = json_new(JSON_NODE);
    json_set_name(info, "WLM Validation");
    json_push_back(info, json_new_a("Validation Error", m_validationError.c_str()));
    set_json_64bit(info, "Frame Offset", m_validationOffset, false);
    set_json_64bit(info, "Input Size", m_logSize, false);
    std::ostringstream prefix;
    for (size_t offset = 0; offset < std::min<size_t>(m_logSize, 80); ++offset)
    {
        if (offset != 0)
        {
            prefix << ' ';
        }
        prefix << std::hex << std::uppercase << std::setfill('0') << std::setw(2)
               << static_cast<unsigned int>(pData[offset]);
    }
    json_push_back(info, json_new_a("Header Prefix (hex)", prefix.str().c_str()));
    if (m_logSize >= 20 && std::memcmp(pData + 16, "HLWN", 4) == 0)
    {
        json_push_back(info, json_new_a("Header Candidate", "HLWN at offset 0x10; field layout is not established"));
    }
    size_t nonzero = 0;
    size_t lastNonzero = 0;
    size_t footerMatches = 0;
    for (size_t offset = 0; offset < m_logSize; ++offset)
    {
        if (pData[offset] != 0)
        {
            ++nonzero;
            lastNonzero = offset;
        }
        if (check_For_Footer_Signature(offset))
        {
            ++footerMatches;
        }
    }
    set_json_64bit(info, "Nonzero Byte Count", nonzero, false);
    if (nonzero != 0)
    {
        set_json_64bit(info, "Last Nonzero Byte Offset", lastNonzero, false);
    }
    set_json_64bit(info, "Legacy Footer Signature Matches", footerMatches, false);
    json_push_back(wlm, info);
}
//-----------------------------------------------------------------------------
//
//   check_For_Footer_Signature()
//
//! \brief
//!   Description:  Check for the footer signature
//
//  Entry:
//! \param: offset  - the offset in the buffer where we will check for the signature
//
//  Exit:
//!   \return bool 
//
//---------------------------------------------------------------------------
bool CFARMWLM::check_For_Footer_Signature(size_t offset)
{
    if (pData == NULL || offset > m_logSize || m_logSize - offset < 4)
    {
        return false;
    }
    uint32_t footer = M_BytesTo4ByteValue(pData[offset], pData[offset + 1], pData[offset + 2], pData[offset + 3]);
    if (footer == FARMFOOTWLM || footer == FARMFOOTWLMSWAP)
    {
        return true;
    }
    return false;
}
//-----------------------------------------------------------------------------
//
//   get_WLM_Header_Data()
//
//! \brief
//!   Description:  get WLM version 6 Header information 
//
//  Entry:
//! \param  offset: the begining offset for getting the data 
//
//  Exit:
//!   \return bool 
//
//---------------------------------------------------------------------------
bool CFARMWLM::get_WLM_Header_Data(size_t offset)
{
    if (pData == NULL || offset > m_logSize || m_logSize - offset < sizeof(sWLMDataHeader))
    {
        return false;
    }
    // get the signature of the WLM trace
    m_DataHeader.signature = M_BytesTo4ByteValue(pData[offset], pData[offset + 1], pData[offset + 2], pData[offset + 3]);
    offset += sizeof(m_DataHeader.signature);
    m_DataHeader.farmeNumber = M_BytesTo2ByteValue(pData[offset + 1], pData[offset]);
    offset += sizeof(m_DataHeader.farmeNumber);
    m_DataHeader.rev = M_BytesTo2ByteValue(pData[offset + 1], pData[offset]);
    offset += sizeof(m_DataHeader.rev);

    m_DataHeader.startTimestamp = M_BytesTo8ByteValue(pData[offset + 7], pData[offset + 6], pData[offset + 5], pData[offset + 4], pData[offset + 3], pData[offset + 2], pData[offset + 1], pData[offset]);
    offset += sizeof(m_DataHeader.startTimestamp);
    m_DataHeader.endTimestamp = M_BytesTo8ByteValue(pData[offset + 7], pData[offset + 6], pData[offset + 5], pData[offset + 4], pData[offset + 3], pData[offset + 2], pData[offset + 1], pData[offset]);
    offset += sizeof(m_DataHeader.endTimestamp);

    m_DataHeader.preFrameOffset = M_BytesTo4ByteValue(pData[offset + 3], pData[offset + 2], pData[offset + 1], pData[offset]);
    offset += sizeof(m_DataHeader.preFrameOffset);
    m_DataHeader.cycleCount = M_BytesTo2ByteValue(pData[offset + 1], pData[offset]);
    offset += sizeof(m_DataHeader.cycleCount) + sizeof(m_DataHeader.pad1);

    m_DataHeader.frameSize = M_BytesTo4ByteValue(pData[offset + 3], pData[offset + 2], pData[offset + 1], pData[offset]);
    if (m_DataHeader.frameSize > TRACESIZE - 8 || m_DataHeader.frameSize < sizeof(sWLMDataHeader))
    {
        return false;
    }
    offset += sizeof(m_DataHeader.frameSize);
    m_DataHeader.duration = M_BytesTo4ByteValue(pData[offset + 3], pData[offset + 2], pData[offset + 1], pData[offset]);
    offset += sizeof(m_DataHeader.duration);
    m_DataHeader.readOps = M_BytesTo4ByteValue(pData[offset + 3], pData[offset + 2], pData[offset + 1], pData[offset]);
    offset += sizeof(m_DataHeader.readOps);
    m_DataHeader.writeOps = M_BytesTo4ByteValue(pData[offset + 3], pData[offset + 2], pData[offset + 1], pData[offset]);
    offset += sizeof(m_DataHeader.writeOps);
    m_DataHeader.markers = M_BytesTo4ByteValue(pData[offset + 3], pData[offset + 2], pData[offset + 1], pData[offset]);
    //offset += sizeof(m_DataHeader.markers);
    return true;


    
}
//-----------------------------------------------------------------------------
//
//   get_transferSize()
//
//! \brief
//!   Description:  get the size of the transfer
//
//  Entry:
//! \param tlength - the tlen from the codeByte
//
//  Exit:
//!   \return uint8_t 
//
//---------------------------------------------------------------------------
uint16_t CFARMWLM::get_transferSize(uint8_t tLen, size_t* offset)
{
    uint16_t len = 0;
    if (tLen == 0)
    {
        len = 1;
    }
    else if (tLen == 1)
    {
        len = 2;
    }
    else if (tLen == 2)
    {
        len = 4;
    }
    else if (tLen == 3)
    {
        len = 8;
    }
    else if (tLen == 4)
    {
        len = 16;
    }
    else if (tLen == 5)
    {
        len = 32;
    }
    else if (tLen == 6)
    {
        len = 128;
    }
    else if (tLen == 7)
    {
        len = 256;
    }
    else if (tLen == 9)
    {
        len = pData[*offset];
        *offset += 1;
    }
    else if (tLen == 0xA)
    {
        len = M_BytesTo2ByteValue(pData[*offset +1],pData[*offset]);
        *offset += 2;
    }
    else
    {
        len = 0;
    }
    return len;
}
//-----------------------------------------------------------------------------
//
//   print_Empty_WLM_Log
//
//! \brief
//!   Description:  Check the signature to see if it is an SSD drive vs HD
//
//  Entry:
//! \param signature - 32 bit holding the signature of the log
//
//  Exit:
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CFARMWLM::print_Empty_WLM_Log(JSONNODE *wlm)
{
    std::string emptyStr;
    emptyStr = "None";

    if (eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
    {
        printf("\tPower on Hours:                     %s \n", "-1");
        printf("\tTotal Number of Other Commands:           %s \n", emptyStr.c_str());
        printf("\tTotal Number of LBAs Read:                %s \n", emptyStr.c_str());
        printf("\tTotal Number of LBAs Written:             %s \n", emptyStr.c_str());
        printf("\tTotal Number of Write Commands:           %s \n", emptyStr.c_str());
        printf("\tTotal Number of Read Commands:            %s \n", emptyStr.c_str());
        printf("\tUnrecoverable Read Errors:                %s \n", emptyStr.c_str());
        printf("\tUnrecoverable Write Errors:               %s \n", emptyStr.c_str());
        printf("\tTotal Number of Random Read Cmds:         %s \n", emptyStr.c_str());
        printf("\tTotal Number of Random Write Cmds:        %s \n", emptyStr.c_str());
        printf("\t# of Task Management Operations:          %s \n", emptyStr.c_str());
    }

    JSONNODE* wlmJson = json_new(JSON_NODE);
    json_set_name(wlmJson, "WLS Summary");

    json_push_back(wlmJson, json_new_a("Power on Hours", emptyStr.c_str()));
    json_push_back(wlmJson, json_new_a("Total Number of Other Commands", emptyStr.c_str()));
    json_push_back(wlmJson, json_new_a("Total Number of LBAs Reads", emptyStr.c_str()));
    json_push_back(wlmJson, json_new_a("Total Number of LBAs Written", emptyStr.c_str()));
    json_push_back(wlmJson, json_new_a("Total Number of Write Commands", emptyStr.c_str()));
    json_push_back(wlmJson, json_new_a("Total Number of Read Commands", emptyStr.c_str()));
    json_push_back(wlmJson, json_new_a("Unrecoverable Read Errors", emptyStr.c_str()));
    json_push_back(wlmJson, json_new_a("Unrecoverable Write Errors", emptyStr.c_str()));
    json_push_back(wlmJson, json_new_a("Total Number of Random Read Cmds", emptyStr.c_str()));
    json_push_back(wlmJson, json_new_a("Total Number of Random Write Cmds", emptyStr.c_str()));
    json_push_back(wlmJson, json_new_a("# of Task Management Operations", emptyStr.c_str()));
    json_push_back(wlmJson, json_new_a("Idle Time", emptyStr.c_str()));
    json_push_back(wlm, wlmJson);

    return eReturnValues::SUCCESS;
}
//-----------------------------------------------------------------------------
//
//   CWLMLog::print_Summary_Log()
//
//! \brief
//!   Description:  Print out the summary data from the log
//
//  Entry:
//! \param wlm - pointer to the json data
//
//  Exit:
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CFARMWLM::print_Summary_Log(size_t* offset, JSONNODE *wlm)
{
#define LBAOFFSET 0x3c
#define HEADERSIZE 8
    eReturnValues retStatus = eReturnValues::SUCCESS;
    if (get_WLM_Header_Data(*offset ))
    {

        if (eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
        {
            printf("Workload Trace Header\n");
            printf("\tFrame Number:                             %" PRIu16" \n", m_DataHeader.farmeNumber);
            printf("\tStart Timestamp:                          %" PRIu64" \n", m_DataHeader.startTimestamp);
            printf("\tEnd Timestamp:                            %" PRIu64" \n", m_DataHeader.endTimestamp);
            printf("\tRead Operations:                          %" PRIu32" \n", m_DataHeader.readOps);
            printf("\tWrite Operations:                         %" PRIu32" \n", m_DataHeader.writeOps);
            printf("\tFrame Size:                               %" PRIu32" \n", m_DataHeader.frameSize);
            printf("\tCycle Count:                              %" PRIu16" \n", m_DataHeader.cycleCount);
            printf("\tOne Second Markers:                       %" PRIu32" \n", m_DataHeader.markers);

        }
   
        JSONNODE* wlmJson = json_new(JSON_NODE);
        std::ostringstream temp;
        temp << "Workload Frame Number " << std::dec << m_DataHeader.farmeNumber;
        json_set_name(wlmJson, temp.str().c_str());
        opensea_parser::set_json_64bit(wlmJson, "Workload Frame Number", m_DataHeader.farmeNumber, false);
        temp.str(""); temp.clear();
        temp << std::dec << m_DataHeader.rev;
        json_push_back(wlmJson, json_new_a("version", temp.str().c_str()));
        opensea_parser::set_json_64bit(wlmJson, "Start Timestamp", m_DataHeader.startTimestamp, false);
        opensea_parser::set_json_64bit(wlmJson, "End Timestamp", m_DataHeader.endTimestamp, false);
        opensea_parser::set_json_64bit(wlmJson, "Read Operations", m_DataHeader.readOps, false);
        opensea_parser::set_json_64bit(wlmJson, "Write Operations", m_DataHeader.writeOps, false);
        opensea_parser::set_json_64bit(wlmJson, "Frame Size", m_DataHeader.frameSize, false);
        opensea_parser::set_json_64bit(wlmJson, "Cycle Count", m_DataHeader.cycleCount, false);
        opensea_parser::set_json_64bit(wlmJson, "One Second Markers", m_DataHeader.markers, false);

        *offset += LBAOFFSET;
        if (!get_Trace_Data(offset, wlmJson))
        {
            retStatus = eReturnValues::PARSE_FAILURE;
        }

        json_push_back(wlm, wlmJson);
    }
    else
    {
        retStatus = eReturnValues::PARSE_FAILURE;
    }
    return retStatus;
}
//-----------------------------------------------------------------------------
//
//  get_Trace_Data()
//
//! \brief
//!   Description:  
//
//  Entry:
//! \param ptr - 
//
//  Exit:
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
bool CFARMWLM::get_Trace_Data(size_t* offset, JSONNODE* wlmJson)
{
    uint64_t lba = 0;
    uint16_t tranSize = 0;
    uint16_t savedTranSize = 0;
    size_t frameOffset = 0x3c;

    
    JSONNODE* tData = json_new(JSON_ARRAY);
    json_set_name(tData, ("Trace Data"));

    while (frameOffset < m_DataHeader.frameSize)
    {
        size_t startOffset = *offset;
        uint8_t encodeByte = pData[*offset];
        uint8_t tLength = M_GETBITRANGE(encodeByte, 7, 4);
        uint8_t LDes = M_GETBITRANGE(encodeByte, 2, 0);
        if (eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
        {
            printf("LDes  0x %" PRIx8" \n", LDes);
            printf("offset %zu \n", *offset);
            printf("encodeByte  0x %" PRIx8" \n", encodeByte);
            printf("tLength  0x %" PRIx8" \n", tLength);
        }
        *offset += 1;

        // Check the whole encoded command before any variable-length reads.
        const size_t lengthBytes = tLength == 9 ? 1 : (tLength == 0xA ? 2 : 0);
        const size_t lbaBytes = LDes == 0 ? (tLength == 0xF ? 0 : 5) :
            (LDes == 4 ? 0 : (LDes & 3));
        const size_t needed = 1 + lengthBytes + lbaBytes;
        if (needed > m_DataHeader.frameSize - frameOffset ||
            startOffset > m_logSize || needed > m_logSize - startOffset ||
            tLength == 0xB || tLength == 0xC ||
            (tLength == 0xF && encodeByte != 0xF0))
        {
            json_push_back(wlmJson, tData);
            return false;
        }

        // if tLength is 0x08 the transfer size is the same as the previous operation, so don't change it
        if (tLength != 0x08)
        {
            savedTranSize = tranSize;
            tranSize = get_transferSize(tLength, offset);
        }

        if (LDes == 0)
        {
            if (tLength == 0xe)
            {
                lba = M_BytesTo8ByteValue(0, 0, 0, pData[*offset + 4], pData[*offset + 3], pData[*offset + 2], pData[*offset + 1], pData[*offset]);
                *offset += 5;
            }
            else if (tLength == 0xd)
            {
                lba = M_BytesTo8ByteValue(0, 0, 0, pData[*offset + 4], pData[*offset + 3], pData[*offset + 2], pData[*offset + 1], pData[*offset]);
                *offset += 5;
            }
            else if (tLength == 8)
            {
                //lba += tranSize;
                lba = M_BytesTo8ByteValue(0, 0, 0, pData[*offset + 4], pData[*offset + 3], pData[*offset + 2], pData[*offset + 1], pData[*offset]);
                *offset += 5;
            }
            else if (tLength == 0x0f)
            {
                lba = 0;
            }
            else
            {
                lba = M_BytesTo8ByteValue(0, 0, 0, pData[*offset + 4], pData[*offset + 3], pData[*offset + 2], pData[*offset + 1], pData[*offset]);
                *offset += 5;
            }
        }
        else if (LDes == 1)
        {
            lba += M_BytesTo8ByteValue(0, 0, 0, 0, 0, 0, 0, pData[*offset]);
            *offset += 1;
        }
        else if (LDes == 2)
        {
            lba += M_BytesTo8ByteValue(0, 0, 0, 0, 0, 0, pData[*offset + 1], pData[*offset]);
            *offset += 2;
        }
        else if (LDes == 3)
        {
            lba += M_BytesTo8ByteValue(0, 0, 0, 0, 0, pData[*offset + 2], pData[*offset + 1], pData[*offset]);
            *offset += 3;
        }
        else if (LDes == 4)
        {
            lba += tranSize;
            //*offset += 1; // ???
        }
        else if (LDes == 5)
        {
            lba -= M_BytesTo8ByteValue(0, 0, 0, 0, 0, 0, 0, pData[*offset]);
            *offset += 1;
        }
        else if (LDes == 6)
        {
            lba -= M_BytesTo8ByteValue(0, 0, 0, 0, 0, 0, pData[*offset + 1], pData[*offset]);
            *offset += 2;
        }
        else if (LDes == 7)
        {
            lba -= M_BytesTo8ByteValue(0, 0, 0, 0, 0, pData[*offset + 2], pData[*offset + 1], pData[*offset]);
            *offset += 3;
        }
        else
        {
            // push what we have and then return false
            printf(">>>>  Lost \n");
            json_push_back(wlmJson, tData);
            return false;
        }
        if (eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
        {
            printf("LBA  0x %" PRIx64" \n", lba);
            printf("Transfer length %" PRIu16" \n", tranSize);
        }
        JSONNODE* tCmd = json_new(JSON_NODE);
        json_set_name(tCmd, ("Trace Command"));
        std::ostringstream temp;
        if (encodeByte & BIT3)
        {
            temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(10) << lba;
            json_push_back(tCmd, json_new_a("Command", "Write"));
            json_push_back(tCmd, json_new_i("Transfer size", tranSize));
            json_push_back(tCmd, json_new_a("LBA", temp.str().c_str()));
        }
        else
        {
            if (encodeByte == 0xf0)
            {
                json_push_back(tCmd, json_new_a("Command", "One Second Marker"));
            }
            else
            {
                if (tLength == 0x0D)
                {
                    temp <<  "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(10) << lba;
                    json_push_back(tCmd, json_new_a("Command", "Streaming Start"));
                    json_push_back(tCmd, json_new_a("LBA", temp.str().c_str()));
                    tranSize = savedTranSize;
                }
                else if (tLength == 0x0E)
                {
                    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(10) << lba;
                    json_push_back(tCmd, json_new_a("Command", "Streaming End"));
                    json_push_back(tCmd, json_new_a("LBA", temp.str().c_str()));
                    tranSize = savedTranSize;
                }
                else
                {
                    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(10) << lba;
                    json_push_back(tCmd, json_new_a("Command", "Read"));
                    json_push_back(tCmd, json_new_i("Transfer size", tranSize));
                    json_push_back(tCmd, json_new_a("LBA", temp.str().c_str()));
                }
            }
        }
        json_push_back(tData,tCmd);
        frameOffset += *offset - startOffset;
        if (check_For_Footer_Signature(*offset))
        {
            break;
        }
    }
    json_push_back(wlmJson, tData);
    return true;
}
//-----------------------------------------------------------------------------
//
//  parse_WLM_Summary()
//
//! \brief
//!   Description:  
//
//  Entry:
//! \param ptr - 
//
//  Exit:
//!   \return SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CFARMWLM::parse_WLM_Summary(JSONNODE *masterData)
{
    // _CrtDumpMemoryLeaks();
    eReturnValues retStatus = eReturnValues::SUCCESS;
    size_t offset = 0;
    size_t traceLine = 0;
   
    offset = 0;
    while (offset  < m_logSize)
    {
        retStatus = print_Summary_Log(&offset, masterData);
        if (retStatus != eReturnValues::SUCCESS)
        {
            return retStatus;
        }
        traceLine += TRACESIZE;
        offset = traceLine;
    }
 
    return retStatus;
}
