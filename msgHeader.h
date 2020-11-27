#ifndef __MSG_HEADER_H__
#define __MSG_HEADER_H__

// define port
#define PORT_SERVER_S 2001
#define PORT_SERVER_R 2002
#define PORT_CLIENT_S 2003
#define PORT_CLIENT_R 2004

// define msg id
#define reqGetTime 0x00020001
#define rspGetTime 0x00020001
#define reqSysInfo 0x00020003
#define rsqSysInfo 0x00020004

// define msg structure
#include <arpa/inet.h>

inline int Write(int x) { return (htonl(x)); }
inline unsigned int Write(unsigned int x) { return (htonl(x)); }
inline short int Write(short int x) { return (htons(x)); }
inline short unsigned int Write(short unsigned int x) { return (htons(x)); }

inline int Read(int x) { return (ntohl(x)); }
inline unsigned int Read(unsigned int x) { return (ntohl(x)); }
inline short int Read(short int x) { return (ntohs(x)); }
inline short unsigned int Read(short unsigned int x) { return (ntohs(x)); }

// 64bit endian 변경
inline long long ntoh64(long x) { return (((long)ntohl(0xFFFFFFFF & x) << 32) + ((long)ntohl(x >> 32))); }
inline long hton64(long x) { return (((long)htonl(0xFFFFFFFF&x))<<32) + ((long)htonl(x>>32)); }

// message structure
struct msgHeader
{
	int cmdId;
    int dummy;
};
inline void Write(struct msgHeader &x) {
	x.cmdId = Write(x.cmdId);
}
inline void Read(struct msgHeader &x) {
    x.cmdId = Read(x.cmdId);
}

// get time
struct dataTime
{
    long time;
    int retValue;
    int dummy;
};
inline void Write(struct dataTime& x) {
    x.time = hton64(x.time);
    x.retValue = Write(x.retValue);
}
inline void Read(struct dataTime& x) {
    x.time = ntoh64(x.time);
    x.retValue = Read(x.retValue);
}

struct msgTime
{
    struct msgHeader hdr;
    struct dataTime data;
};
inline void Write(struct msgTime& x) {
    Write(x.hdr);
    Write(x.data);
}
inline void Read(struct msgTime& x) {
    Read(x.hdr);
    Read(x.hdr);
}

//get sysinfo
struct dataSysInfo
{
    char hostname[64]; // hostname
    unsigned long totalram; // total usable main memory size
    unsigned long freeram; // available memory size
    int retValue;
    int dummy;
};
inline void Write(struct dataSysInfo& x) {
    x.totalram = hton64(x.totalram);
    x.freeram = hton64(x.freeram);
    x.retValue = Write(x.retValue);
}
inline void Read(struct dataSysInfo& x) {
    x.totalram = ntoh64(x.totalram);
    x.freeram = ntoh64(x.freeram);
    x.retValue = Read(x.retValue);
}

struct msgSysInfo
{
    struct msgHeader hdr;
    struct dataSysInfo data;
};
inline void Write(struct msgSysInfo& x) {
    Write(x.hdr);
    Write(x.data);
}
inline void Read(struct msgSysInfo& x) {
    Read(x.hdr);
    Read(x.data);
}

#endif // __MSG_HEADER_H__
