#ifndef __DEFINE_CRYPTO_H__
#define __DEFINE_CRYPTO_H__

#include <string>
using namespace std;

typedef enum PinFormat
{
	PF_WITH_ACCNO = 0x01,
	PF_WITHOUT_ACCNO = 0x06	
}PinFormat;

typedef enum MacMode
{
	MM_X99 = 1,
	MM_X919,
	MM_XOR,
	MM_CUP,
	MM_PBOC
}MacMode;

//暂时只支持ECB和CBC
typedef enum CryptoMode
{
	CM_ECB = 0,
	CM_CBC
}CryptoMode;

//暂时只支持Pkcs5、Pkcs7、补0x800x00...
typedef enum PaddingType
{
	PT_NOPADDING = 0,
	PT_PKCS1 = 1,
	PT_PKCS1_OAEP = 2,
	PT_PKCS5 = 5,	//缺几个字节就补几个字节的几
	PT_PKCS7 = 7,	//缺几个字节就补几个字节的0
	PT_80
}PaddingType;

//暂时只支持SHA1
typedef enum HashType
{
	HT_Md5 = 0,
	HT_SHA1,	
	HT_SHA224,
	HT_SHA256,
	HT_SHA384,
	HT_SHA512
}HashType;

typedef struct CertInfo
{
	string cert_no;
	string crt;
	string key;
	string start_date;
	string end_date;
	string common_name;
	string organization;
	string org_unit;
	string email;
	string issuer;	
	string status;
}CertInfo;
	
#endif //__DEFINE_CRYPTO_H__
