#ifndef _M_WEBSOCKETPARSE_
#define _M_WEBSOCKETPARSE_

#include "m_common.h"



typedef int (*SetParseFunc_t)(u32_t tab_uuid, u32_t id, void *pData, u32_t len);



//
//int SetDeviceInfo(u32_t tab_uuid, u32_t id, void *pData, u32_t len)
//{
//
//	return 0;
//}
//
//
//int SetDeviceStatus(u32_t tab_uuid, u32_t id, void *pData, u32_t len)
//{
//
//	return 0;
//}


//	参数表初始化：数组--参数表tabid个数（子表id个数）		，
//	单个参数表初始化信息结构体
struct ChildTabPasrInitInfo
{
	u32_t id;
	u32_t len;
	void *pData;
	SetParseFunc_t *SetParseFunc;
	u32_t flag;	//读写权限，只有读才可以被订阅，只有写才能被设置
};

struct TabPasrInitInfo
{
	u32_t tab_uuid;	//参数表id
	u8_t childTabNums;	//子表数量
	struct ChildTabPasrInitInfo *childArr;		//子表info
};

struct AllTabPasrInitInfo
{
	u8_t tabNums;	//子表数量
	struct TabPasrInitInfo *tabArr;		//表info
};

#endif
