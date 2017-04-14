#ifndef MYDEFINE_H
#define MYDEFINE_H

#include "define.h"
#include "log.h"

//长度字节数(报文中域长度的实际字节数)
#define FIX     0    //定长
#define LD      1    //1字节长度
#define LLD     2    //2字节长度
#define LLLD    3    //3字节长度

//数据类型
#define NL      0    //左靠BCD，后补0
#define NR      1    //右靠BCD，前补0
#define ASC     2    //ASC数据
#define BIN     3    //二进制数据

//长度类型
#define HEXF    8    //16进制表示的高位在前的长度
#define HEXA    9    //16进制表示的高位在后的长度

#define PACK_MAXLEN     2048    //单包最大长度
#define FIELD_MAXLEN    1024    //单域最大长度

#define FIELDCNT       129

typedef struct _Iso8583MessageFormat
{
    int iFieldCnt;  //域总数(含消息类型)
    int iOffSet;    //表示报文长度的字节数
    int iLenType;   //长度格式：NR, ASC, HEXF, HEXA
    int iHeadLen;   //报文头长度
    int iTpduLen;   //tpdu长度
    int iBitMapLen; //位图长度
    unsigned char cHead[64];//报文头
    unsigned char cTpdu[12];//tpdu
    unsigned char cBitMap[16];//位图
}MsgFmt;

#endif // MYDEFINE_H
