/*pvData.cpp*/
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/**
 *  @author mrk
 */
#define epicsExportSharedSymbols
#include <pv/epicsException.h>
#include <pv/serializeHelper.h>
#include <pv/status.h>

using std::string;

namespace epics { namespace pvData {

const char* Status::StatusTypeName[] = { "OK", "WARNING", "ERROR", "FATAL" };
string Status::m_emptyStringtring;

Status Status::Ok;

//PVDATA_REFCOUNT_MONITOR_DEFINE(status);

Status::Status() :
    m_statusType(STATUSTYPE_OK)
{
}

Status::Status(StatusType type, string const & message) :
    m_statusType(type), m_message(message)
{
    if (type == STATUSTYPE_OK)
        throw std::invalid_argument("type == STATUSTYPE_OK");
         
    //PVDATA_REFCOUNT_MONITOR_CONSTRUCT(status);
}

Status::Status(StatusType type, string const & message, string const & stackDump) :
    m_statusType(type), m_message(message), m_stackDump(stackDump)
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
    return m_statusType;    
}
    

string Status::getMessage() const
{
    return m_message;
}
    
string Status::getStackDump() const
{
    return m_stackDump;
}
    
bool Status::isOK() const
{
    return (m_statusType == STATUSTYPE_OK);
}
    
bool Status::isSuccess() const
{
    return (m_statusType == STATUSTYPE_OK || m_statusType == STATUSTYPE_WARNING);
}

void Status::serialize(ByteBuffer *buffer, SerializableControl *flusher) const
{
	flusher->ensureBuffer(1);
	if (m_statusType == STATUSTYPE_OK)
	{
		// special code for okStatus (optimization)
		buffer->putByte((int8)-1);
	}
	else
	{
		buffer->putByte((int8)m_statusType);
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
	   if (m_statusType != STATUSTYPE_OK)
	   {
	       m_statusType = STATUSTYPE_OK;
	       m_message = m_stackDump = m_emptyStringtring;
	   }
	}
	else
	{
	    m_statusType = (StatusType)typeCode;
		m_message = SerializeHelper::deserializeString(buffer, flusher);
		m_stackDump = SerializeHelper::deserializeString(buffer, flusher);
	}
}

std::ostream& operator<<(std::ostream& o, const Status& status)
{
    o << "Status [type=" << Status::StatusTypeName[status.m_statusType];
    if (!status.m_message.empty())
        o << ", message=" << status.m_message;
    if (!status.m_stackDump.empty())
        o << ", stackDump=" << std::endl << status.m_stackDump;
    o << ']';
    return o;
}

std::ostream& operator<<(std::ostream& o, const Status::StatusType& statusType)
{
    o << Status::StatusTypeName[statusType];
    return o;
}

}}
