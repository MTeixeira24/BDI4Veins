/*
 * LightJasonBuffer.cpp
 *
 *  Created on: 26/08/2018
 *      Author: miguel
 */

#include "LightJasonBuffer.h"

LightJasonBuffer::LightJasonBuffer() : buf() {
    buf_index = 0;
}

LightJasonBuffer::LightJasonBuffer(std::string _buf) : buf(_buf) {
    buf_index = 0;
}

bool LightJasonBuffer::eof() const
{
    return buf_index == buf.length();
}

bool LightJasonBuffer::isBigEndian()
{
    short a = 0x0102;
    unsigned char* p_a = reinterpret_cast<unsigned char*>(&a);
    return (p_a[0] == 0x01);
}

LightJasonBuffer::~LightJasonBuffer() {
}

