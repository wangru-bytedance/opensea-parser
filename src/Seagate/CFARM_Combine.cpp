//
// CFarm_Combine.cpp   Implementation of class CFARMLog
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2023 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CFarm_Combine.cpp  
//
// \brief Defines the function calls and structures for parsing Seagate combine FARM logs SAS and SATA

#include "CFARM_Combine.h"
#include "CFARM_Log.h"
#include <algorithm>
#include <cstring>

using namespace opensea_parser;

namespace
{
	const size_t COMBINED_HEADER_SIZE = 256;
	const size_t COMBINED_ENTRY_SIZE = 32;

	// The enclosing header, directory entry or frame is checked before use.
	uint64_t combined_LE(const uint8_t* data, size_t offset, size_t size)
	{
		uint64_t value = 0;
		for (size_t i = 0; i < size; ++i)
		{
			value |= static_cast<uint64_t>(data[offset + i]) << (8 * i);
		}
		return value;
	}

	std::string combined_String(const uint8_t* data, size_t offset, size_t size)
	{
		std::string value(reinterpret_cast<const char*>(data + offset), size);
		const size_t terminator = value.find('\0');
		if (terminator != std::string::npos)
		{
			value.resize(terminator);
		}
		while (!value.empty() && value.back() == ' ')
		{
			value.pop_back();
		}
		return value;
	}
}
//-----------------------------------------------------------------------------
//
//! \fn CFarm_Combine::CFarm_Combine()
//
//! \brief
//!   Description: Class constructor
//
//  Entry:
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CFarm_Combine::CFarm_Combine()
	:CFarmCommon()
	, bufferData()
	, m_LogSize(0)
	, m_status(eReturnValues::IN_PROGRESS)
	, m_isScsi(false)
	, m_combine_isScsi(false)
	, m_isComboLog(false)
	, m_showStatus(false)
	, m_showStatic(false)
	, m_version(0)
	, m_maxLogSize(0)
	, m_entrySize(COMBINED_ENTRY_SIZE)
{

}
//-----------------------------------------------------------------------------
//
//! \fn CFarm_Combine::CFarm_Combine()
//
//! \brief
//!   Description: Class constructor
//
//  Entry:
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CFarm_Combine::CFarm_Combine(bool showStatus)
	:CFarmCommon()
	, bufferData()
	, m_LogSize(0)
	, m_status(eReturnValues::IN_PROGRESS)
	, m_isScsi(false)
	, m_combine_isScsi(false)
	, m_isComboLog(false)
	, m_showStatus(showStatus)
	, m_showStatic(false)
	, m_version(0)
	, m_maxLogSize(0)
	, m_entrySize(COMBINED_ENTRY_SIZE)
{

}
//-----------------------------------------------------------------------------
//
//! \fn CFarm_Combine::CFarm_Combine()
//
//! \brief
//!   Description: Class constructor
//
//  Entry:
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CFarm_Combine::CFarm_Combine(uint8_t* buffer, size_t bufferSize, bool showStatus, bool showStatic)
	:CFarmCommon()
	, bufferData(buffer)
	, m_LogSize(bufferSize)
	, m_status(eReturnValues::IN_PROGRESS)
	, m_isScsi(false)
	, m_combine_isScsi(false)
	, m_isComboLog(false)
	, m_showStatus(showStatus)
	, m_showStatic(showStatic)
	, m_version(0)
	, m_maxLogSize(0)
	, m_entrySize(COMBINED_ENTRY_SIZE)
{
	is_Combo_Log();
	combine_Device_Scsi();
}
//-----------------------------------------------------------------------------
//
//! \fn CFarm_Combine::CFarm_Combine()
//
//! \brief
//!   Description: Class Deconstructor
//
//  Entry:
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CFarm_Combine::~CFarm_Combine()
{
	// bufferData is borrowed from the caller.
}
//-----------------------------------------------------------------------------
//
//! \fn setCombine()
//
//! \brief
//!   Description:  setup the combine class by settting the buffer, logsize, checking if combo file, checking if sas
//
//  Entry:
//
//  Exit:
//!   \return bool = True or False
//
//---------------------------------------------------------------------------
void CFarm_Combine::setCombine(uint8_t* buffer, size_t bufferSize)
{
	setBuffer(buffer); 
	setLogSize(bufferSize); 
	is_Combo_Log();
	combine_Device_Scsi();
	
}
//-----------------------------------------------------------------------------
//
//! \fn combine_Device_Scsi()
//
//! \brief
//!   Description:  check the first part of the buffer to see if it's a scsi log or sata \n
//!                 look at offset 0x18 for information
//
//  Entry:
//
//  Exit:
//!   \return bool = True or False
//
//---------------------------------------------------------------------------
void CFarm_Combine::combine_Device_Scsi()
{
	is_Device_Scsi();
	m_combine_isScsi = bufferData != M_NULLPTR && m_LogSize >= 28 &&
		combined_String(bufferData, 24, 4) == "SAS";
}
//-----------------------------------------------------------------------------
//
//! \fn is_Device_Scsi()
//
//! \brief
//!   Description:  check the first part of the buffer to see if it's a scsi log or sata log
//
//  Entry:
//
//  Exit:
//!   \return bool = True or False
//
//---------------------------------------------------------------------------
void CFarm_Combine::is_Device_Scsi()
{
	m_isScsi = bufferData != M_NULLPTR && m_LogSize >= 2 &&
		CFarmCommon::is_Device_Scsi(bufferData[0], bufferData[1]);
}
//-----------------------------------------------------------------------------
//
//! \fn is_Combo_Log()
//
//! \brief
//!   Description:  check the log to see if it is a combo log or not
//
//  Entry:
//
//  Exit:
//!   \return bool = True or False
//
//---------------------------------------------------------------------------
void CFarm_Combine::is_Combo_Log()
{
	m_isComboLog = bufferData != M_NULLPTR && m_LogSize >= 16 &&
		std::memcmp(bufferData, "STX_FARM_COMBINE", 16) == 0;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Log_Size()
//
//! \brief
//!   Description: Get the log size from the buffer data. Starting offset + 32 - 39.
//
//  Entry:
//! \param offset - location with in the buffer, to get the log size.
//
//  Exit:
//!   \return uint64_t currentLogSize
//
//---------------------------------------------------------------------------
uint64_t CFarm_Combine::get_Log_Size(size_t offset)
{
	if (bufferData == M_NULLPTR || offset > m_LogSize || m_LogSize - offset < 40)
	{
		return 0;
	}
	const uint64_t signature = combined_LE(bufferData, offset, 8) & UINT64_C(0x00FFFFFFFFFFFFFF);
	if (signature == FARMEMPTYSIGNATURE || signature == FARMPADDINGSIGNATURE)
	{
		return ATALOGSIZE;
	}
	return combined_LE(bufferData, offset + 32, 8) & UINT64_C(0x00FFFFFFFFFFFFFF);
}
//-----------------------------------------------------------------------------
//
//! \fn get_FARM_Type()
//
//! \brief
//!   Description:  From the farm log Reason the FARM log was capture 
//
//  Entry:
//! \param headerNode - string for the information to be placed
//! \param flag - the uint8 value from the buffer
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarm_Combine::get_FARM_Type(std::string* reason, uint64_t dataType)
{
	if (dataType == FARM_CURRENT)
	{
		*reason = "Current FARM Log";
	}
	else if (dataType == FARM_TIME)
	{
		*reason = "Time Series FARM Log";
	}
	else if (dataType == FARM_LONG_SAVE)
	{
		*reason = "Long Term Saved FARM Log";
	}
	else if (dataType == FARM_SAVE)
	{
		*reason = "Host Saved FARM Disc Log";
	}
	else if (dataType == FARM_FACTORY)
	{
		*reason = "Factory FARM Log";
	}
	else if (dataType == FARM_STICKY)
	{
		*reason = "Sticky FARM Log";
	}
	else if (dataType == FARM_WLDTR)
	{
		*reason = "FARM WorkLoad Trace Log";
	}
	else
	{
		*reason = "unknown";
	}
}
//-----------------------------------------------------------------------------
//
//! \fn is_Subpage_Neeeded()
//
//! \brief
//!   Description: Check the dataType to see if we need to create a subpage for the Json data
//
//  Entry:
//! \param dataType - pointer to the dataType in sComboDataSet structure
//
//
//  Exit:
//!   \return bool - true if we need a subpage in the JSON
//
//---------------------------------------------------------------------------
bool CFarm_Combine::is_Subpage_Neeeded(uint64_t dataType)
{
	if (dataType == FARM_TIME)						// FARM TimeSeries Frame
	{
		return true;
	}
	if (dataType == FARM_STICKY)					// FARM Sticky Frame
	{
		return true;
	}
	if (dataType == FARM_LONG_SAVE)				    // Farm Long Saved Frame
	{
		return true;
	}
	return false;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Sticky_Frame_Type()
//
//! \brief
//!   Description: get the sticky frame names 
//
//  Entry:
//! \param reason - pointer to the string 
//! \param frame - the frame number  (note we have to add 3 to get the right names)
//
//
//  Exit:
//!   \return none
//
//---------------------------------------------------------------------------
void CFarm_Combine::get_Sticky_Frame_Type(std::string* reason, uint8_t frame)
{
	switch (frame+3)
	{
	case GLIST_DISC_ENTRIES_FRAME:
		*reason = "1000 g-list disc entries frame";
		break;
	case FIRST_UNRECOVERED_READ_ERROR_FRAME:
		*reason = "1st unrecovered read error";
		break;
	case TENTH_UNRECOVERED_READ_ERROR_FRAME:
		*reason = "10th unrecovered read error";
		break;
	case FIRST_FATAL_CTO_FRAME:
		*reason = "1st fatal command time out";
		break;
	case BEFORE_CFW_UPDATE_FRAME:
		*reason = "last frame prior to most recent CFW or SFW update";
		break;
	case TEMP_EXCEDED_FRAME:
		*reason = "temperature exceeds 70 degress celsius";
		break;
	default:
		*reason = "Unknown Frame Type";
		break;
	}
}
//-----------------------------------------------------------------------------
//
//! \fn get_Header_Info()
//
//! \brief
//!   Description:  Get the header information from the combined FARM log
//
//  Entry:
//! \param offset - offset of for the buffer 
//! \param headerInfo - pointer to the idenify data structure
//
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarm_Combine::get_Header_Info(sStringIdentifyData* headerInfo)
{
	// Match the writer's complete string widths and little-endian version words.
	m_version = combined_LE(bufferData, 16, 8);
	headerInfo->deviceInterface = combined_String(bufferData, 24, 4);
	headerInfo->modelNumber = combined_String(bufferData, 32, 40);
	headerInfo->serialNumber = combined_String(bufferData, 80, 20);
	headerInfo->firmwareRev = combined_String(bufferData, 104, 8);
}
//-----------------------------------------------------------------------------
//
//! \fn get_Data_Set()
//
//! \brief
//!   Description:  get the data set from the combined FARM log. Fills in the \n
//!  vdataSetInfo vector
//
//  Entry:
//! \param DataSetNumber - the number of data sets to look for
//
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarm_Combine::get_Data_Set(uint16_t DataSetNumber)
{
#define DATAOFFSET  0x100
	vdataSetInfo.clear();
	vdataSetInfo.reserve(DataSetNumber);
	size_t offset = DATAOFFSET;
	for (uint16_t i = 0; i < DataSetNumber; i++)
	{
		sComboDataSet set;
		for (size_t byte = 0; byte < 8; ++byte)
		{
			set.dataSetType = (set.dataSetType << 8) | bufferData[offset + byte];
		}
		set.location = static_cast<uint32_t>(combined_LE(bufferData, offset + 8, 4));
		set.dataSize = static_cast<uint32_t>(combined_LE(bufferData, offset + 12, 4));
		set.startTime = combined_LE(bufferData, offset + 16, 8);
		set.endTime = combined_LE(bufferData, offset + 24, 8);
		vdataSetInfo.push_back(set);
		offset += m_entrySize;
	}
}
//-----------------------------------------------------------------------------
//
//! \fn parse_FARM_Logs()
//
//! \brief
//!   Description:  get the data set from the combined FARM log. Fills in the \n
//!  vdataSetInfo vector
//
//  Entry:
//! \param offset - what the starting offset is
//
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarm_Combine::parse_FARM_Logs(size_t offset, size_t logSize, uint64_t dataType, JSONNODE* farmJson)
{
	m_status = eReturnValues::SUCCESS;
	if (offset > m_LogSize || logSize > m_LogSize - offset)
	{
		m_status = eReturnValues::INVALID_LENGTH;
		return;
	}
	const size_t end = offset + logSize;
	const bool subFrame = is_Subpage_Neeeded(dataType);
	while (offset < end)
	{
		const size_t remaining = end - offset;
		size_t page = 0;
		if (m_combine_isScsi)
		{
			if (remaining < 4)
			{
				m_status = eReturnValues::INVALID_LENGTH;
				return;
			}
			if (!CFarmCommon::is_Device_Scsi(bufferData[offset], bufferData[offset + 1]))
			{
				m_status = eReturnValues::VALIDATION_FAILURE;
				return;
			}
			page = (static_cast<size_t>(bufferData[offset + 2]) << 8) + bufferData[offset + 3] + 4;
			if (page < 8 + sizeof(sFarmHeader) || page > remaining)
			{
				m_status = eReturnValues::INVALID_LENGTH;
				return;
			}
		}
		else
		{
			if (remaining < sizeof(sFarmHeader))
			{
				m_status = eReturnValues::INVALID_LENGTH;
				return;
			}
			const uint64_t signature = combined_LE(bufferData, offset, 8) & UINT64_C(0x00FFFFFFFFFFFFFF);
			if (signature == FARMEMPTYSIGNATURE || signature == FARMPADDINGSIGNATURE)
			{
				page = ATALOGSIZE;
			}
			else
			{
				if (signature != FARMSIGNATURE && signature != FACTORYCOPY)
				{
					m_status = eReturnValues::VALIDATION_FAILURE;
					return;
				}
				const uint64_t frameSize = get_Log_Size(offset);
				const uint64_t pageSize = combined_LE(bufferData, offset + 40, 8) & UINT64_C(0x00FFFFFFFFFFFFFF);
				const uint64_t copies = combined_LE(bufferData, offset + 56, 8) & UINT64_C(0x00FFFFFFFFFFFFFF);
				const size_t minPageSize = std::max({sizeof(sFarmHeader), sizeof(sDriveInfo),
					sizeof(sWorkLoadStat), sizeof(sErrorStat), sizeof(sEnvironementStat),
					sizeof(sAtaReliabilityStat)});
				// Five pages per copy follow the header. Divide before multiplying.
				if (frameSize > remaining || pageSize < minPageSize || frameSize / pageSize < 6 ||
					copies >= (frameSize / pageSize - 1) / 5)
				{
					m_status = eReturnValues::INVALID_LENGTH;
					return;
				}
				page = static_cast<size_t>(frameSize);
			}
		}
		if (page > remaining || (!subFrame && page != remaining))
		{
			m_status = eReturnValues::INVALID_LENGTH;
			return;
		}
		JSONNODE* farmInfo = (subFrame || m_combine_isScsi) ? json_new(JSON_NODE) : farmJson;
		if (m_combine_isScsi)
		{
			std::ostringstream name;
			name << "Subpage 0x" << std::hex << std::uppercase << std::setw(2)
				<< std::setfill('0') << static_cast<unsigned int>(bufferData[offset + 1]);
			json_set_name(farmInfo, name.str().c_str());
		}
		// Current field decoders retain BER fixes and status/static options.
		CFARMLog log(bufferData + offset, page, m_showStatus, m_showStatic);
		m_status = log.parse_Device_Farm_Log(farmInfo);
		if (farmInfo != farmJson)
		{
			json_push_back(farmJson, farmInfo);
		}
		if (m_status != eReturnValues::SUCCESS)
		{
			return;
		}
		offset += page;
	}
}

//-----------------------------------------------------------------------------
//
//! \fn print_Combine_Log_Header()
//
//! \brief
//!   Description:  print out the header information sn, firmware rev, interface, model and version number
//
//  Entry:
//! \param sStringIdentifyData - structue from the identifydata for the drive
//! \param labelJson - json node to add the data to
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarm_Combine::print_Combine_Log_Header(sStringIdentifyData* headerInfo, JSONNODE* header)
{
	JSONNODE* label = json_new(JSON_NODE);
	json_set_name(label, "FARM Combined Labels");
	json_push_back(label, json_new_a("serial_number", headerInfo->serialNumber.c_str()));
	json_push_back(label, json_new_a("firmware_rev", headerInfo->firmwareRev.c_str()));
	json_push_back(label, json_new_a("Drive Interface", headerInfo->deviceInterface.c_str()));
	json_push_back(label, json_new_a("Model Number", headerInfo->modelNumber.c_str()));
	std::ostringstream version;
	version << ((m_version >> 48) & 0xFFFF) << "." << ((m_version >> 32) & 0xFFFF)
		<< "." << ((m_version >> 16) & 0xFFFF);
	json_push_back(label, json_new_a("FARM Combined version", version.str().c_str()));
	json_push_back(header, label);
}
//-----------------------------------------------------------------------------
//
//! \fn print_Header_Debug()
//
//! \brief
//!   Description:  print out the header information on where everything is located
//
//  Entry:
//! \param labelJson - json node to add the data to
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarm_Combine::print_Header_Debug( JSONNODE* labelJson)
{
#if defined _DEBUG
	std::ostringstream temp;
	std::string dataStr = "Debug Combo Data Set";
	JSONNODE* label = json_new(JSON_ARRAY);
	json_set_name(label, dataStr.c_str());

	for (std::vector<sComboDataSet>::iterator dataItr = vdataSetInfo.begin(); dataItr != vdataSetInfo.end(); ++dataItr)
	{
		JSONNODE* dblabel = json_new(JSON_NODE);
		temp.str(""); temp.clear();
		temp << "0x" << std::hex << std::nouppercase << dataItr->dataSetType;
		json_push_back(dblabel, json_new_a("Type", temp.str().c_str()));
		temp.str(""); temp.clear();
		temp << "0x" << std::hex << std::nouppercase << dataItr->dataSize;
		json_push_back(dblabel, json_new_a("Size", temp.str().c_str()));
		temp.str(""); temp.clear();
		temp << "0x" << std::hex << std::nouppercase << dataItr->startTime;
		json_push_back(dblabel, json_new_a("start time", temp.str().c_str()));
		temp.str(""); temp.clear();
		temp << "0x" << std::hex << std::nouppercase << dataItr->endTime;
		json_push_back(dblabel, json_new_a("end time", temp.str().c_str()));
		temp.str(""); temp.clear();
		temp << "0x" << std::hex << std::nouppercase << dataItr->location;
		json_push_back(dblabel, json_new_a("location", temp.str().c_str()));
		json_push_back(label, dblabel);
	}

	json_push_back(labelJson, label);
#else
	(void)labelJson;
#endif
}
//-----------------------------------------------------------------------------
//
//! \fn combo_Parsing()
//
//! \brief
//!   Description:  parse out the Farm Log into a vector called Farm Frame.
//
//  Entry:
//! \param pData - pointer to the buffer
//
//  Exit:
//!   \return SUCCESS or FAILURE
//
//---------------------------------------------------------------------------
eReturnValues CFarm_Combine::combo_Parsing(JSONNODE* masterJson)
{
	if (masterJson == M_NULLPTR || bufferData == M_NULLPTR)
	{
		return m_status = eReturnValues::BAD_PARAMETER;
	}
	if (m_LogSize < COMBINED_HEADER_SIZE)
	{
		return m_status = eReturnValues::INVALID_LENGTH;
	}
	if (!m_isComboLog)
	{
		return m_status = eReturnValues::VALIDATION_FAILURE;
	}
	if (combined_LE(bufferData, 22, 2) != 1)
	{
		return m_status = eReturnValues::NOT_SUPPORTED;
	}
	const size_t headerSize = static_cast<size_t>(combined_LE(bufferData, 116, 2));
	const uint16_t count = static_cast<uint16_t>(combined_LE(bufferData, 252, 2));
	m_entrySize = static_cast<size_t>(combined_LE(bufferData, 254, 2));
	if (headerSize < COMBINED_HEADER_SIZE || headerSize > m_LogSize ||
		m_entrySize < COMBINED_ENTRY_SIZE || count == 0 ||
		count > (headerSize - COMBINED_HEADER_SIZE) / m_entrySize)
	{
		return m_status = eReturnValues::INVALID_LENGTH;
	}
	const std::string interface = combined_String(bufferData, 24, 4);
	if (interface != "SATA" && interface != "SAS")
	{
		return m_status = eReturnValues::NOT_SUPPORTED;
	}
	get_Data_Set(count);
	for (size_t i = 0; i < vdataSetInfo.size(); ++i)
	{
		const sComboDataSet& dataSet = vdataSetInfo[i];
		if (dataSet.location < headerSize || dataSet.location > m_LogSize ||
			dataSet.dataSize == 0 || dataSet.dataSize > m_LogSize - dataSet.location)
		{
			return m_status = eReturnValues::INVALID_LENGTH;
		}
		for (size_t previous = 0; previous < i; ++previous)
		{
			const sComboDataSet& other = vdataSetInfo[previous];
			if (dataSet.dataSetType == other.dataSetType ||
				(static_cast<uint64_t>(dataSet.location) < static_cast<uint64_t>(other.location) + other.dataSize &&
				 static_cast<uint64_t>(other.location) < static_cast<uint64_t>(dataSet.location) + dataSet.dataSize))
			{
				return m_status = eReturnValues::VALIDATION_FAILURE;
			}
		}
	}
	m_maxLogSize = vdataSetInfo.front().dataSize;
	sStringIdentifyData headerInfo;
	get_Header_Info(&headerInfo);
	print_Combine_Log_Header(&headerInfo, masterJson);
	print_Header_Debug(masterJson);

	JSONNODE* directory = json_new(JSON_ARRAY);
	json_set_name(directory, "FARM Combined Data Sets");
	json_push_back(masterJson, directory);
	eReturnValues result = eReturnValues::SUCCESS;
	for (size_t i = 0; i < vdataSetInfo.size(); ++i)
	{
		const sComboDataSet& dataSet = vdataSetInfo[i];
		const std::string tag = combined_String(bufferData, COMBINED_HEADER_SIZE + i * m_entrySize, 8);
		std::string typeName;
		get_FARM_Type(&typeName, dataSet.dataSetType);
		JSONNODE* info = json_new(JSON_NODE);
		json_push_back(info, json_new_a("Type", tag.c_str()));
		set_json_64bit(info, "Offset", dataSet.location, false);
		set_json_64bit(info, "Size", dataSet.dataSize, false);
		set_json_64bit(info, "Start Time (ms since Unix epoch)", dataSet.startTime, false);
		set_json_64bit(info, "End Time (ms since Unix epoch)", dataSet.endTime, false);
		json_push_back(directory, info);

		JSONNODE* data = json_new(is_Subpage_Neeeded(dataSet.dataSetType) ||
			dataSet.dataSetType == FARM_WLDTR ? JSON_ARRAY : JSON_NODE);
		json_set_name(data, typeName == "unknown" ? tag.c_str() : typeName.c_str());
		json_push_back(masterJson, data);
		eReturnValues status;
		if (dataSet.dataSetType == FARM_WLDTR)
		{
			CFARMWLM wlm(bufferData + dataSet.location, dataSet.dataSize, data);
			status = wlm.get_WLM_Status();
		}
		else if (typeName == "unknown")
		{
			json_push_back(data, json_new_a("Not Supported", "Unknown combined FARM dataset type"));
			status = eReturnValues::NOT_SUPPORTED;
		}
		else
		{
			parse_FARM_Logs(dataSet.location, dataSet.dataSize, dataSet.dataSetType, data);
			status = m_status;
		}
		json_push_back(info, json_new_a("Parsing Status", status == eReturnValues::SUCCESS ? "Success" :
			(status == eReturnValues::NOT_SUPPORTED ? "Not supported" : "Failed")));
		if (status != eReturnValues::SUCCESS &&
			(result == eReturnValues::SUCCESS || result == eReturnValues::NOT_SUPPORTED))
		{
			result = status;
		}
	}
	return m_status = result;
}
