#pragma once

/**
 * RingBuffer Class.
 * Each data are pushed by push method. Then commit method fixes each pushed data to one data.
 */
class CRingBuffer
{
public:
    //! table of item
    struct SItemTable
    {
        unsigned long long m_serialNum; ///< serial number (= commit count)
        unsigned long m_addr; ///< head
        unsigned long m_size; ///< pushed size
        bool m_wraparound; ///< wrap-around?
        bool m_enable; ///< enable access area?
        SItemTable();
    };

    bool init(const unsigned long in_num, const unsigned long in_size);
    bool push(const unsigned char* in_data, const unsigned long in_size);
    bool commit();

    CRingBuffer();
    virtual ~CRingBuffer();

private:
    unsigned long m_num; ///< num of table
    unsigned long m_capacity; ///< capacity of ring buffer
    unsigned long m_indexTable; ///< current index of table
    unsigned long m_indexTableBegin; ///< begin index of table
    unsigned long m_indexTableEnd; ///< end index of table
    unsigned long m_addrWrite; ///< write pointer

    SItemTable* m_pTable; ///< table
    unsigned char* m_pBuffer; ///< buffer

    unsigned long calcAddrEnd(const unsigned long in_addr, const unsigned long in_size) const;
    unsigned long incrementIndexTable(const unsigned long in_indexTable) const;
    void stepBegin(const unsigned long in_addr, const unsigned long in_size);
    void writeData(const unsigned char* in_data, const unsigned long in_size);
};

inline CRingBuffer::SItemTable::SItemTable():
    m_serialNum(0), m_addr(0), m_size(0), m_wraparound(false), m_enable(false)
{
}

/**
 * calc end of data.
 * @return address
 */
unsigned long CRingBuffer::calcAddrEnd(
		const unsigned long in_addr, ///< [in] aaddress where data is written
		const unsigned long in_size  ///< [in] size of data
		) const
{
    if (in_addr + in_size < m_capacity)
    {
        return in_addr + in_size;
    }

    return (in_addr + in_size) - m_capacity;
}

/**
 * increment index of table.
 * @return incremented index
 */
unsigned long CRingBuffer::incrementIndexTable(
		const unsigned long in_indexTable ///< [in] current index of table
		) const
{
    return (in_indexTable + 1) % m_num;
}
