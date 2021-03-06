/*
 * LightJasonBuffer.h
 *
 *  Created on: 26/08/2018
 *      Author: miguel
 */

#ifndef MANAGERS_MANAGER_LIGHTJASONBUFFER_H_
#define MANAGERS_MANAGER_LIGHTJASONBUFFER_H_

#include <cstdint>

#include <cstddef>
#include <string>

#include "veins/base/utils/MiXiMDefs.h"

namespace Jason{

class LightJasonBuffer {
private:
    std::string buf;
    size_t buf_index;
public:
    LightJasonBuffer();
    LightJasonBuffer(std::string);
    virtual ~LightJasonBuffer();
    bool eof() const;
    bool isBigEndian();
    void reserve(uint32_t);
    void clear();
    bool isEmpty(){return buf.empty();}
    uint32_t length(){return buf.length();}


    template <typename T>
    T read()
    {
        T buf_to_return;
        unsigned char* p_buf_to_return = reinterpret_cast<unsigned char*>(&buf_to_return);

        if (isBigEndian()) {
            for (size_t i = 0; i < sizeof(buf_to_return); ++i) {
                if (eof()) throw cRuntimeError("Attempted to read past end of byte buffer");
                p_buf_to_return[i] = buf[buf_index++];
            }
        }
        else {
            for (size_t i = 0; i < sizeof(buf_to_return); ++i) {
                if (eof()) throw cRuntimeError("Attempted to read past end of byte buffer");;
                p_buf_to_return[sizeof(buf_to_return) - 1 - i] = buf[buf_index++];
            }
        }

        return buf_to_return;
    }


    template <typename T>
    void write(T inv)
    {
        unsigned char* p_buf_to_send = reinterpret_cast<unsigned char*>(&inv);

        if (isBigEndian()) {
            for (size_t i = 0; i < sizeof(inv); ++i) {
                buf += p_buf_to_send[i];
            }
        }
        else {
            for (size_t i = 0; i < sizeof(inv); ++i) {
                buf += p_buf_to_send[sizeof(inv) - 1 - i];
            }
        }
    }

    template <typename T>
    T read(T& out)
    {
        out = read<T>();
        return out;
    }

    template <typename T>
    LightJasonBuffer& operator>>(T& out)
    {
        out = read<T>();
        return *this;
    }

    template <typename T>
    LightJasonBuffer& operator<<(const T& inv)
    {
        write(inv);
        return *this;
    }

    std::string getBuffer(){
        return buf;
    }


};
template <>
void LightJasonBuffer::write(std::string inv);
template <>
std::string LightJasonBuffer::read();
}

#endif /* MANAGERS_MANAGER_LIGHTJASONBUFFER_H_ */
