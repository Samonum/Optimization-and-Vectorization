#pragma once

#define L1CACHESIZE		8192		/4			// total L1$ size, in bytes
#define L2CACHESIZE		16384		/4			// total L2$ size, in bytes
#define L3CACHESIZE		65536		/4			// total L3$ size, in bytes
#define SLOTSIZE		64						// cache slot size, in bytes
#define ADDRESSMASK		(0x1000000 - SLOTSIZE)	// used for masking out lowest log2(SLOTSIZE) bits
#define OFFSETMASK		(SLOTSIZE - 1)			// used for masking out bits above log2(SLOTSIZE)
#define RAMACCESSCOST	110
#define L1ACCESSCOST	4
#define L2ACCESSCOST	16
#define L3ACCESSCOST	48
#define VALID           0b1
#define DIRTY           0b10
#define NWAYN			8

#define EVICTION		4						// 0 = random, 1 = FIFO, 2 = Bit-PLRU, 3 = Tree-PLRU, 4 = Modified-Tree-PLRU
#define LRUMARKER		0b100
typedef unsigned int address;

struct CacheLine
{
	uint tag = 0;
	byte value[SLOTSIZE];
	bool IsValid();
	bool IsDirty();
};

struct ParkingLot
{
	CacheLine cacheLine[NWAYN];
	int evictionData = 0;
};


inline bool CacheLine::IsValid()
{
	return tag & VALID;
}

inline bool CacheLine::IsDirty()
{
	return tag & DIRTY;
}

class MemCac
{
public:
	virtual byte READB(address a) = 0;
	virtual CacheLine READCL(address a, bool wrrite = false) = 0;
	virtual void WRITEB(address a, byte) = 0;
	virtual void WRITECL(address a, CacheLine& line) = 0;
	virtual byte READB16(address a) = 0;
	virtual CacheLine READCL16(address a, bool wrrite = false) = 0;
	virtual void WRITEB16(address a, byte) = 0;
	virtual void WRITECL16(address a, CacheLine& line) = 0;
};

class Memory: public MemCac
{
public:
	// ctor/dtor
	Memory( uint size );
	~Memory();
	byte READB(address a) { return 0; };
	CacheLine READCL(address a, bool write = false);
	void WRITEB(address a, byte) {};
	void WRITECL(address a, CacheLine& line);

	byte READB16(address a) { return 0; };
	CacheLine READCL16(address a, bool wrrite = false);
	void WRITEB16(address a, byte) {};
	void WRITECL16(address a, CacheLine& line);

	void ConsoleWrite();
	// data members
	CacheLine* data;
	int* tag;
	bool artificialDelay;
	int read, write;
};

class Cache: public MemCac
{
public:
	// ctor/dtor
	Cache( MemCac* mem , int cSize, int level);
	~Cache();
	// methods
	byte READB(address a);
	CacheLine READCL(address a, bool write = false);
	void UpdateLRUTree(ParkingLot &lot, int i);
	int TreeFindLRU(ParkingLot &lot);
	CacheLine ReadMiss(address a, bool isWrite);
	void WRITEB(address a, byte);
	void WRITECL(address a, CacheLine& line);

	byte READB16(address a);
	CacheLine READCL16(address a, bool wrrite = false);
	void WRITEB16(address a, byte);
	void WRITECL16(address a, CacheLine& line);

	void ResetStats();
	void ConsoleDebug();
	// TODO: READ/WRITE functions for (aligned) 16 and 32-bit values
	// data
	ParkingLot* lot;
	MemCac* memory;
	int rHits, rMisses, totalCost, rCacheAdd, rEvict, read, write, wHits, wMisses, wCacheAdd, wEvict, level = 0;
	int rtotalHits = 0, rtotalMisses = 0, wtotalHits = 0, wtotalMisses = 0;
	int slotMask;
};