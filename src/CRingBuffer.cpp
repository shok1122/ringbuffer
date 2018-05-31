#include "CRingBuffer.h"

#include <string.h>

CRingBuffer::CRingBuffer():
    m_num(0),
    m_capacity(0),
    m_indexTable(0),
    m_indexTableBegin(0),
    m_indexTableEnd(0),
    m_addrWrite(0),
    m_pTable(nullptr),
    m_pBuffer(nullptr)
{
}

CRingBuffer::~CRingBuffer()
{
}

/**
 * make ringbuffer.
 * allocate memory.
 */
bool CRingBuffer::init(
        const unsigned long in_num, ///< [in] num of table
        const unsigned long in_size ///< [in] size of ringbuffer
    )
{
    m_num = in_num;
    m_capacity = in_size;

    m_pTable = new SItemTable[in_num];
    if (nullptr == m_pTable)
    {
        return false;
    }

    m_pBuffer = new unsigned char[in_size];
    if (nullptr == m_pBuffer)
    {
        delete[] m_pTable;
        return false;
    }

    return true;
}

/**
 * push data.
 */
bool CRingBuffer::push(
        const unsigned char* in_data, ///< [in] data
        const unsigned long in_size   ///< [in] size of data
    )
{
    // TODO: lock
    SItemTable* pItem = &(m_pTable[m_indexTable]);
    pItem->m_size += in_size;

    // TODO: wrap-around
    writeData(in_data, in_size);
}

/**
 * fix data.
 */
bool CRingBuffer::commit()
{
    // save
    SItemTable* pItem = &(m_pTable[m_indexTable]);
    pItem->m_enable = true;
    m_indexTableEnd = m_indexTable;

    // increment write index
    m_indexTable = incrementIndexTable(m_indexTable);

    // update next table
    SItemTable* pItemNext = &(m_pTable[m_indexTable]);
    pItemNext->m_serialNum = pItem->m_serialNum + 1;
    pItemNext->m_addr = m_addrWrite;
    pItemNext->m_size = 0;
    pItemNext->m_enable = false;

    // TODO: unlock
}

/**
 * step begin index of table.
 */
void CRingBuffer::stepBegin(
        const unsigned long in_addr, ///< [in] address where data is written
        const unsigned long in_size  ///< [in] size of data
    )
{
    const unsigned long c_addrEnd = calcAddrEnd(in_addr, in_size);

    while (true)
    {
        SItemTable* pItem = &(m_pTable[m_indexTableBegin]);
        if (pItem->m_addr <= c_addrEnd || c_addrEnd < (pItem->m_addr + pItem->m_size))
        {
            m_indexTableBegin = incrementIndexTable(m_indexTableBegin);
        }
        else
        {
            break;
        }
    }
}

/**
 * write data.
 */
void CRingBuffer::writeData(
        const unsigned char* in_data, ///< [in] data
        const unsigned long in_size   ///< [in] size of data
    )
{
    const unsigned long c_sizeToEnd = m_capacity - m_addrWrite;

    // step begin if override is needed
    stepBegin(m_addrWrite, in_size);

    if (m_addrWrite < c_sizeToEnd)
    {
        // do not need wrap-around
        memcpy(&(m_pBuffer[m_addrWrite]), in_data, in_size);
        m_addrWrite += in_size;
    }
    else
    {
        // need wrap-around
        memcpy(&(m_pBuffer[m_addrWrite]), in_data, c_sizeToEnd);
        memcpy(m_pBuffer, &(in_data[c_sizeToEnd]), c_sizeToEnd - in_size);
        m_addrWrite = c_sizeToEnd - in_size;
    }
}