#ifndef __TYPE_H__
#define __TYPE_H__

typedef unsigned long uint64_t;
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

//typedef unsigned long off_t;
//typedef unsigned long size_t;

#define EMFILE  -2
#define ENOENT  -3
#define ENOTDIR -4
#define EBADF   -5
#define EINVAL  -6

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#endif //__TYPE_H__
