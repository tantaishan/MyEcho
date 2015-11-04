//	base_types.h
//	2012/10/23
//

#ifndef __BASE_TYPES_H__
#define __BASE_TYPES_H__


#define OS_WIN	1

#ifndef DISALLOW_COPY_AND_ASSIGN
#	define DISALLOW_COPY_AND_ASSIGN(TypeName)	\
	TypeName(const TypeName&);					\
	void operator=(const TypeName&)
#endif

#define DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
	TypeName();                                    \
	DISALLOW_COPY_AND_ASSIGN(TypeName)


#ifndef Safe_Delete
#	define Safe_Delete(x) if(x){delete (x); (x)=NULL;}
#endif
#ifndef Safe_Release
#	define Safe_Release(x) if(x){(x)->Release(); (x)=NULL;}
#endif


typedef __int8 int8;
typedef __int16 int16;
typedef __int32 int32;
typedef __int64 int64;
typedef unsigned __int8 uint8;
typedef unsigned __int16 uint16;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;

//typedef int8 int8_t;
typedef int16 int16_t;
typedef int32 int32_t;
typedef int64 int64_t;
typedef uint8 uint8_t;
typedef uint16 uint16_t;
typedef uint32 uint32_t;
typedef uint64 uint64_t;


#endif // __BASE_TYPES_H__

