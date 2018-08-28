/*
 * LightJasonBuffer.cpp
 *
 *  Created on: 26/08/2018
 *      Author: miguel
 */

#include "LightJasonBuffer.h"

namespace Jason{

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

template <>
void LightJasonBuffer::write(std::string inv)
{
    uint32_t length = inv.length();
    write<uint32_t>(length);
    for (size_t i = 0; i < length; ++i) write<char>(inv[i]);
}

template <>
std::string LightJasonBuffer::read()
{
    uint32_t length = read<uint32_t>();
    if (length == 0) return std::string();
    char obuf[length + 1];

    for (size_t i = 0; i < length; ++i) read<char>(obuf[i]);
    obuf[length] = 0;

    return std::string(obuf, length);
}

LightJasonBuffer::~LightJasonBuffer() {
}
}
