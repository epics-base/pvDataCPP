/*
 * serializeHelper.cpp
 *
 *  Created on: Oct 22, 2010
 *      Author: Miha vitorovic
 */

#include <algorithm>
#include <pvType.h>

#include "epicsException.h"
#include "byteBuffer.h"
#include "serializeHelper.h"


using namespace std;

namespace epics {
    namespace pvData {

        void SerializeHelper::writeSize(int s, ByteBuffer* buffer,
                SerializableControl* flusher) {
            flusher->ensureBuffer(sizeof(int64)+1);
            SerializeHelper::writeSize(s, buffer);
        }

        void SerializeHelper::writeSize(int s, ByteBuffer* buffer) {
            if(s==-1) // null
                buffer->putByte(-1);
            else if(s<254)
                buffer->putByte(s);
            else
                buffer->putByte(-2)->putInt(s); // (byte)-2 + size
        }

        int SerializeHelper::readSize(ByteBuffer* buffer,
                DeserializableControl* control) {
            control->ensureData(1);
            int8 b = buffer->getByte();
            if(b==-1)
                return -1;
            else if(b==-2) {
                control->ensureData(sizeof(int32));
                int32 s = buffer->getInt();
                if(s<0) throw EpicsException("negative array size");
                return s;
            }
            else
                return (int)(b<0 ? b+256 : b);
        }

        void SerializeHelper::serializeString(const String& value,
                ByteBuffer* buffer, SerializableControl* flusher) {
            int len = value.length();
            SerializeHelper::writeSize(len, buffer, flusher);
            int i = 0;
            while(true) {
                int maxToWrite = min(len-i, buffer->getRemaining());
                buffer->put(value.data(), i, maxToWrite); // UTF-8
                i += maxToWrite;
                if(i<len)
                    flusher->flushSerializeBuffer();
                else
                    break;
            }
        }

        void SerializeHelper::serializeSubstring(const String& value,
                int offset, int count, ByteBuffer* buffer,
                SerializableControl* flusher) {
            if(offset<0)
                offset = 0;
            else if(offset>(int)value.length()) offset = value.length();

            if(offset+count>(int)value.length()) count = value.length()-offset;

            SerializeHelper::writeSize(count, buffer, flusher);
            int i = 0;
            while(true) {
                int maxToWrite = min(count-i, buffer->getRemaining());
                buffer->put(value.data(), offset+i, maxToWrite); // UTF-8
                i += maxToWrite;
                if(i<count)
                    flusher->flushSerializeBuffer();
                else
                    break;
            }
        }

        String SerializeHelper::deserializeString(ByteBuffer* buffer,
                DeserializableControl* control) {

            int size = SerializeHelper::readSize(buffer, control);
            if(size>=0) {
                char* retBuffer = new char[size]; // get the return buffer
                try {
                    int i = 0;
                    while(true) {
                        int toRead = min(size-i, buffer->getRemaining());
                        buffer->get(retBuffer, i, toRead);
                        i += toRead;
                        if(i<size)
                            control->ensureData(1);
                        else
                            break;
                    }
                    String s = String(retBuffer, size);
                    delete retBuffer;
                    return s;
                } catch(...) {
                    delete retBuffer; // remove the buffer
                    throw;
                }
            }
            else
                return String("");
        }

    }
}
