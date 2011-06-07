/*pvData.cpp*/
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <pv/status.h>
#include <pv/epicsException.h>
#include <pv/serializeHelper.h>

namespace epics { namespace pvData {

const char* Status::StatusTypeName[] = { "OK", "WARNING", "ERROR", "FATAL" };
epics::pvData::String Status::m_emptyString;

Status Status::OK;

//PVDATA_REFCOUNT_MONITOR_DEFINE(status);

Status::Status() :
    m_type(STATUSTYPE_OK), m_message(m_emptyString), m_stackDump(m_emptyString)
{
}

Status::Status(StatusType type, String message) :
    m_type(type), m_message(message), m_stackDump(m_emptyString)
{
    if (type == STATUSTYPE_OK)
        throw std::invalid_argument("type == STATUSTYPE_OK");
         
    //PVDATA_REFCOUNT_MONITOR_CONSTRUCT(status);
}

Status::Status(StatusType type, String message, String stackDump) :
    m_type(type), m_message(message), m_stackDump(stackDump)
{
    if (type == STATUSTYPE_OK)
        throw std::invalid_argument("type == STATUSTYPE_OK");

    //PVDATA_REFCOUNT_MONITOR_CONSTRUCT(status);
}
    
Status::~Status() {
    //PVDATA_REFCOUNT_MONITOR_DESTRUCT(status);
}
    
Status::StatusType Status::getType() const
{
    return m_type;    
}
    

epics::pvData::String Status::getMessage() const
{
    return m_message;
}
    
epics::pvData::String Status::getStackDump() const
{
    return m_stackDump;
}
    
bool Status::isOK() const
{
    return (m_type == STATUSTYPE_OK);
}
    
bool Status::isSuccess() const
{
    return (m_type == STATUSTYPE_OK || m_type == STATUSTYPE_WARNING);
}

void Status::serialize(ByteBuffer *buffer, SerializableControl *flusher) const
{
	flusher->ensureBuffer(1);
	if (m_type == STATUSTYPE_OK)
	{
		// special code for okStatus (optimization)
		buffer->putByte((int8)-1);
	}
	else
	{
		buffer->putByte((int8)m_type);
		SerializeHelper::serializeString(m_message, buffer, flusher);
		SerializeHelper::serializeString(m_stackDump, buffer, flusher);
	}
}
    
void Status::deserialize(ByteBuffer *buffer, DeserializableControl *flusher)
{
	flusher->ensureData(1);
	int8 typeCode = buffer->getByte();
	if (typeCode == (int8)-1)
	{
	   // in most of the cases status will be OK, we statistically optimize
	   if (m_type != STATUSTYPE_OK)
	   {
	       m_type = STATUSTYPE_OK;
	       m_message = m_stackDump = m_emptyString;
	   }
	}
	else
	{
	    m_type = (StatusType)typeCode;
		m_message = SerializeHelper::deserializeString(buffer, flusher);
		m_stackDump = SerializeHelper::deserializeString(buffer, flusher);
	}
}

String Status::toString() const
{
    String str;
    toString(&str, 0);
    return str;
}

void Status::toString(StringBuilder buffer, int indentLevel) const
{
    *buffer += "Status [type=";
    *buffer += StatusTypeName[m_type];
    if (!m_message.empty())
    {
        *buffer += ", message=";
        *buffer += m_message;
    }
    if (!m_stackDump.empty())
    {
        *buffer += ", stackDump=";
        *buffer += '\n';
        *buffer += m_stackDump;
    }
    *buffer += ']';
}

}}
