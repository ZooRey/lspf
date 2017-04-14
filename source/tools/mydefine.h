#ifndef MYDEFINE_H
#define MYDEFINE_H

#include "define.h"
#include "log.h"

//�����ֽ���(�������򳤶ȵ�ʵ���ֽ���)
#define FIX     0    //����
#define LD      1    //1�ֽڳ���
#define LLD     2    //2�ֽڳ���
#define LLLD    3    //3�ֽڳ���

//��������
#define NL      0    //��BCD����0
#define NR      1    //�ҿ�BCD��ǰ��0
#define ASC     2    //ASC����
#define BIN     3    //����������

//��������
#define HEXF    8    //16���Ʊ�ʾ�ĸ�λ��ǰ�ĳ���
#define HEXA    9    //16���Ʊ�ʾ�ĸ�λ�ں�ĳ���

#define PACK_MAXLEN     2048    //������󳤶�
#define FIELD_MAXLEN    1024    //������󳤶�

#define FIELDCNT       129

typedef struct _Iso8583MessageFormat
{
    int iFieldCnt;  //������(����Ϣ����)
    int iOffSet;    //��ʾ���ĳ��ȵ��ֽ���
    int iLenType;   //���ȸ�ʽ��NR, ASC, HEXF, HEXA
    int iHeadLen;   //����ͷ����
    int iTpduLen;   //tpdu����
    int iBitMapLen; //λͼ����
    unsigned char cHead[64];//����ͷ
    unsigned char cTpdu[12];//tpdu
    unsigned char cBitMap[16];//λͼ
}MsgFmt;

#endif // MYDEFINE_H
