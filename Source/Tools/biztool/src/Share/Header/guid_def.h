/* -------------------------------------------------------------------------
//	文件名		：	guid_def.h
//	创建者		：	wangbin
//	创建时间	：	2007-12-19 17:53:00
//	功能描述	：	进程间通信用的GUID
// -----------------------------------------------------------------------*/
#ifndef __GUID_DEF_H__
#define __GUID_DEF_H__

#define BUFLEN_SHAREMEM		(200 * 1024)

#define GUID_GAMESERVER_W	"{0EE3EDA3-250E-4757-B3F5-2B227771B7FC}"	// 写缓冲（对于GameServer）
#define GUID_GAMESERVER_R	"{689074C6-C197-46f5-B31E-3821CB8ADFF1}"	// 读缓冲（对于GameServer）
#define GUID_GAMECENTER_W	"{FD52B660-086B-41c8-86E3-D95956E94335}"	// 写缓冲（对于GameCenter）
#define GUID_GAMECENTER_R	"{F5F6A9D7-F97E-40b1-BAEE-C408E490BCD0}"	// 读缓冲（对于GameCenter）

#endif //__GUID_DEF_H__
