/*
MIT License

Copyright (c) 2022 salvato

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "iobuffer.h"
#include <QDebug>


IOBuffer::IOBuffer(char* buffer, int sampleNumber, QObject *parent)
    : QIODevice(parent)
    , pBuffer(buffer)
    , sampleCount(sampleNumber)
{
    for(int i=0; i<sampleCount; i++)
        pBuffer[i] = 0;
    bufferStart = 0;
}


IOBuffer::~IOBuffer() {
}


qint64
IOBuffer::readData(char* pData, qint64 dataSize) {
    Q_UNUSED(pData)
    Q_UNUSED(dataSize)
    return -1;
}


qint64
IOBuffer::writeData(const char* pData, qint64 dataSize) {
    int start = 0;
    if(dataSize < sampleCount) {
        start = sampleCount-dataSize;
        for(int s=0; s<start; ++s)
            pBuffer[s] = pBuffer[s+dataSize];
    }
    for(int s=start; s<sampleCount; ++s, pData++)
        pBuffer[s] =*pData;
    emit bufferFull();
    return dataSize;//(sampleCount-start);
}
