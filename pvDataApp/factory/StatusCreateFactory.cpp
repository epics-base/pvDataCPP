/*StatusCreateFactory.cpp*/
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <lock.h>
#include "factory.h"
#include "byteBuffer.h"
#include "showConstructDestruct.h"
#include "status.h"
#include "serializeHelper.h"

namespace epics { namespace pvData {

//static DebugLevel debugLevel = lowDebug;

static volatile int64 totalConstruct = 0;
static volatile int64 totalDestruct = 0;
static Mutex *globalMutex = 0;

static int64 getTotalConstruct()
{
    Lock xx(globalMutex);
    return totalConstruct;
}

static int64 getTotalDestruct()
{
    Lock xx(globalMutex);
    return totalDestruct;
}

static ConstructDestructCallback *pConstructDestructCallback;

static void init()
{
     static Mutex mutex = Mutex();
     Lock xx(&mutex);
     if(globalMutex==0) {
        globalMutex = new Mutex();
        pConstructDestructCallback = new ConstructDestructCallback(
            String("status"),
            getTotalConstruct,getTotalDestruct,0);
     }
}

class StatusImpl : public Status
{
    public:
    
    StatusImpl(StatusType type, String message, String stackDump) :
        m_type(type), m_message(message), m_stackDump(stackDump)
    {
        Lock xx(globalMutex);
        totalConstruct++;
    }
    
    ~StatusImpl() {
        Lock xx(globalMutex);
        totalDestruct++;
    }
    
    virtual StatusType getType()
    {
        return m_type;    
    }
    

    virtual epics::pvData::String getMessage()
    {
        return m_message;
    }
    
    virtual epics::pvData::String getStackDump()
    {
        return m_stackDump;
    }
    
    virtual bool isOK()
    {
	   return (m_type == STATUSTYPE_OK);
    }
    
    virtual bool isSuccess()
    {
        return (m_type == STATUSTYPE_OK || m_type == STATUSTYPE_WARNING);
    }

    virtual void serialize(ByteBuffer *buffer, SerializableControl *flusher)
    {
		flusher->ensureBuffer(1);
		if (this == getStatusCreate()->getStatusOK())
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
    
    virtual void deserialize(ByteBuffer *pbuffer, DeserializableControl *pflusher)
    {
	   throw new std::runtime_error("use getStatusCreate()->deserialize()");
    }

    virtual void toString(StringBuilder buffer, int indentLevel)
    {
        *buffer += "StatusImpl [type=";
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

    private:
    
    StatusType m_type;
    String m_message;
    String m_stackDump;
    
};




class StatusCreateImpl : public StatusCreate {
    public:
    
    StatusCreateImpl()
    {
        init();
        m_ok = createStatus(STATUSTYPE_OK, "OK", 0);
    }

    virtual Status* getStatusOK() {
        return m_ok;
    }
        	
    virtual Status* createStatus(StatusType type, String message, BaseException* cause) {
        if (cause == 0)
            return new StatusImpl(type, message, "");
        else
        {
            std::string stackDump;
            cause->toString(stackDump);
            return new StatusImpl(type, message, stackDump);
        }
    }
        	
    virtual Status* deserializeStatus(ByteBuffer* buffer, DeserializableControl* control) {
		control->ensureData(1);
		int8 typeCode = buffer->getByte();
		if (typeCode == (int8)-1)
			return m_ok;
		else {
			String message = SerializeHelper::deserializeString(buffer, control);
			String stackDump = SerializeHelper::deserializeString(buffer, control);
			return new StatusImpl((StatusType)typeCode, message, stackDump);
		}
    }

    private:
    
    Status* m_ok;
};




static StatusCreate* statusCreate = 0;

StatusCreate* getStatusCreate() {
    static Mutex mutex = Mutex();
    Lock xx(&mutex);

    if(statusCreate==0) statusCreate = new StatusCreateImpl();
    return statusCreate;
}

}}
