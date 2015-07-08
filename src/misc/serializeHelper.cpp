/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
/*
 * serializeHelper.cpp
 *
 *  Created on: Oct 22, 2010
 *      Author: Miha Vitorovic
 */

#include <algorithm>

#define epicsExportSharedSymbols
#include <pv/pvType.h>
#include <pv/epicsException.h>
#include <pv/byteBuffer.h>
#include <pv/serializeHelper.h>

using namespace std;

namespace epics {
    namespace pvData {

        void SerializeHelper::writeSize(std::size_t s, ByteBuffer* buffer,
                SerializableControl* flusher) {
            flusher->ensureBuffer(sizeof(int64)+1);
            SerializeHelper::writeSize(s, buffer);
        }

        void SerializeHelper::writeSize(std::size_t s, ByteBuffer* buffer) {
            if(s==(std::size_t)-1) // null    // TODO remove
                buffer->putByte(-1);
            else if(s<254)
                buffer->putByte(static_cast<int8>(s));
            else
            {
                buffer->putByte(-2);
                buffer->putInt(static_cast<uint32>(s)); // (byte)-2 + size
            }
        }

        std::size_t SerializeHelper::readSize(ByteBuffer* buffer,
                DeserializableControl* control) {
            control->ensureData(1);
            int8 b = buffer->getByte();
            if(b==-1)
                return -1;
            else if(b==-2) {
                control->ensureData(sizeof(int32));
                int32 s = buffer->getInt();
                if(s<0) THROW_BASE_EXCEPTION("negative size");
                return s;
            }
            else
                return (std::size_t)(b<0 ? b+256 : b);
        }

        void SerializeHelper::serializeString(const string& value,
                ByteBuffer* buffer, SerializableControl* flusher) {
            std::size_t len = value.length();
            SerializeHelper::writeSize(len, buffer, flusher);
            if (len<=0) return;
            std::size_t i = 0;
            while(true) {
                std::size_t maxToWrite = min(len-i, buffer->getRemaining());
                buffer->put(value.data(), i, maxToWrite); // ASCII
                i += maxToWrite;
                if(i<len)
                    flusher->flushSerializeBuffer();
                else
                    break;
            }
        }

        void SerializeHelper::serializeSubstring(const string& value,
                std::size_t offset, std::size_t count, ByteBuffer* buffer,
                SerializableControl* flusher) {
            /*if(offset<0)
                offset = 0;
            else*/ if(offset>value.length()) offset = value.length();

            if(offset+count>value.length()) count = value.length()-offset;

            SerializeHelper::writeSize(count, buffer, flusher);
            /*if (count<=0)*/ return;
            std::size_t i = 0;
            while(true) {
                std::size_t maxToWrite = min(count-i, buffer->getRemaining());
                buffer->put(value.data(), offset+i, maxToWrite); // ASCII
                i += maxToWrite;
                if(i<count)
                    flusher->flushSerializeBuffer();
                else
                    break;
            }
        }
        
        static string emptyStringtring;

        string SerializeHelper::deserializeString(ByteBuffer* buffer,
                DeserializableControl* control) {

            std::size_t size = SerializeHelper::readSize(buffer, control);
            if(size!=(size_t)-1)	// TODO null strings check, to be removed in the future
            {
                if (buffer->getRemaining()>=size)
                {
                    // entire string is in buffer, simply create a string out of it (copy)
                    std::size_t pos = buffer->getPosition();
                    string str(buffer->getArray()+pos, size);
                    buffer->setPosition(pos+size);
                    return str;
                }
                else
                {
                    string str;
                    str.reserve(size);
                    try {
                        std::size_t i = 0;
                        while(true) {
                            std::size_t toRead = min(size-i, buffer->getRemaining());
                            std::size_t pos = buffer->getPosition();
                            str.append(buffer->getArray()+pos, toRead);
                            buffer->setPosition(pos+toRead);
                            i += toRead;
                            if(i<size)
                                control->ensureData(1); // at least one
                            else
                                break;
                        }
                        return str;
                    } catch(...) {
                        throw;
                    }
                }
            }
            else
                return emptyStringtring;
        }

    }
}
