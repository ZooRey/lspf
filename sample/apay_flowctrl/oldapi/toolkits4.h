/**
 *Unit Name: toolkits4.h
 *Current Version: 4.0
 *Description:
 *   常用工具函数(共42个)封装:
 *   BCD转换、随机数、BASE64编码、对称/非对称/RSA(512~4096 bits)、SSL连接、压缩、UTF8转换...等.
 *Author:
 *   24291878@qq.com
 *Copyright:
 *   Copyright(C) 2014.08
 *History:
 *   支持32/64位平台
 *TODO:
**/

#ifndef _toolkits4_H_
#define _toolkits4_H_

#include <string.h>

#if defined(_WIN32) || defined(_WINDOWS) || defined(WIN32) || defined(_WINDOWS_)
#define DLL_IMPORT  extern "C" __declspec(dllimport)
#define WINAPI      __stdcall
#else
#define DLL_IMPORT  extern "C"
#define WINAPI
#endif
//-----------------------------------------------------------------------------

/**
 *取[0, 0xFF]之间的随机数.
 *Parameters:
 *  pDest[out]  - 输出随机数
 *  uSize[in]   - 随机数个数
 *Return:
 **/
DLL_IMPORT void GetRandom(unsigned char *pDest, unsigned short uSize);

/**
 *取[0, iMax]之间的随机整数.
 *Parameters:
 *  pDest[out]  - 输出随机整数
 *  uSize[in]   - 随机整数个数
 *  iMaxInt[in] - 随机整数最大值(限定数值范围, 默认: 65535)
 *Return:
 **/
DLL_IMPORT void GetRandomInt(unsigned int  *pDest, unsigned short uSize, int iMaxInt = 65535);

/**
 *取[0, 1]之间的随机实数.
 *Parameters:
 *  pDest[out]  - 输出随机实数
 *  uSize[in]   - 随机实数个数
 *Return:
 **/
DLL_IMPORT void GetRandomReal(double *pDest, unsigned short uSize);
//-----------------------------------------------------------------------------


/**
 *Description:
 *  压缩BCD转为ASCII字符串。
 *parameter：
 *  Dest[out] - ASCII字符串
 *  Src[in] - 输入数据
 *  Len[in] - 数据长度
 *Return：
 *  输出数据长度(是输入长度的2倍，需保证足够大的输出缓冲区)
 **/
DLL_IMPORT int asc2bcd(unsigned char *Dest, char* Src);

/**
 *Description:
 *  ASCII字符串转为压缩BCD.
 *Parameter：
 *  Dest[out] - 输出数据
    Src[in]   - 源BCD字符串
 *Return：
 *  成功 - 返回输出数据长度(输入长度的一半)
 *  失败 - 返回0
**/
DLL_IMPORT int bcd2asc(char *Dest, unsigned char* Src, int Len);

/**
 *Description:
 *  Base64编码.
 *Parameter:
 *  base64_out/base64_len[out] - 编码
    src_in/src_len[in]         - 源数据
 *Return:
 *   0 - 成功
 *  -1 - 失败
**/
DLL_IMPORT int Base64Encode(char* base64_out, size_t *base64_len,
                            const unsigned char* src_in, size_t src_len);

/**
 *Description:
 *  Base64解码.
 *Parameter:
 *  dest_out/dest_len[out] - 解码数据
    base64_in[in]          - base64编码数据
 *Return:
 *  0 - 成功
 * -1 - 失败
**/
DLL_IMPORT int Base64Decode(unsigned char* dest_out, size_t *dest_len, const char* base64_in);

/**
 *Description:
 *  Ansi(GB2312)字符串转为UTF-8字符串.
 *Paramter:
 *  utf8[out] - 输出结果
 *  utf8_size[in] - 输出缓存区大小
 *  gbk/gbk_size[in] - 输入数据
 *Return:
 *  0 - 失败
 * >0 - 成功, 返回转换成功的字符串大小.
**/
DLL_IMPORT int gbk_to_utf8(unsigned char *utf8, size_t utf8_size,
                           const unsigned char *gbk, const size_t gbk_size);

/**
 *Description:
 *  UTF-8字符串转Ansi(GB2312)字符串.
 *Paramter:
 *  gbk[out] - 输出结果
 *  gbk_size[in] - 输出缓存区大小
 *  utf8/utf8_size[in] - 输入数据
 *Return:
 *  0 - 失败
 * >0 - 成功, 返回转换成功的字符串大小.
**/
DLL_IMPORT int utf8_to_gbk(unsigned char *gbk, size_t gbk_size,
                           const unsigned char *utf8, const size_t utf8_size);
//-----------------------------------------------------------------------------


/**
 *Description:
 *  计算16位校验和.
 *Input：
 *  Data - 输入数据
 *  DataLen - 数据长度
 *Output：
 *Return：
 *  CRC校验和.
 **/
DLL_IMPORT unsigned short crc16(unsigned char* Src, int Len);

/**
 *Description:
 *  计算32位校验和.
 *Paramter:
 *  Dest[out]   - 输出结果(4 Bytes)
 *  Src/Len[in] - 输入数据
 *Return:
**/
DLL_IMPORT unsigned int crc32(unsigned char* Src, int Len);

/**
 *Description:
 *  MD2 hash.
 *Input:
 *  HMAC[out]      - 哈希值(16 Bytes)
 *  Src/Len[in]    - 源数据
 *  Key/KeyLen[in] - 混淆Key: NULL时，输出标准哈希值.
 *Return:
 **/
DLL_IMPORT void md2HMAC(unsigned char HMAC[16],
                        const unsigned char* Src, size_t Len,
                        const unsigned char *Key = NULL, size_t KeyLen = 0);
/**
 *Description:
 *  MD4 hash.
 *Input:
 *  HMAC[out]      - 哈希值(16 Bytes)
 *  Src/Len[in]    - 源数据
 *  Key/KeyLen[in] - 混淆Key: NULL时，输出标准哈希值.
 *Return:
 **/
DLL_IMPORT void md4HMAC(unsigned char HMAC[16],
                        const unsigned char* Src, size_t Len,
                        const unsigned char *Key = NULL, size_t KeyLen = 0);

/**
 *Description:
 *  MD5 hash.
 *Input:
 *  HMAC[out]      - 哈希值(16 Bytes)
 *  Src/Len[in]    - 源数据
 *  Key/KeyLen[in] - 混淆Key: NULL时，输出标准哈希值.
 *Return:
 **/
DLL_IMPORT void md5HMAC(unsigned char HMAC[16],
                        const unsigned char* Src, size_t Len,
                        const unsigned char *Key = NULL, size_t KeyLen = 0);
/**
 *Description:
 *  ripemd160HMAC.
 *Paramter:
 *  HMAC[out]      - 哈希值(20 Bytes)
 *  Src/Len[in]    - 源数据
 *  Key/KeyLen[in] - 混淆Key: NULL时，输出标准哈希值.
 *Return:
**/
DLL_IMPORT void ripemd160HMAC(unsigned char HMAC[20],
                              const unsigned char *Src, size_t Len,
                              const unsigned char *Key = NULL, size_t KeyLen = 0);
/**
 *Description:
 *  sha1HMAC.
 *Paramter:
 *  HMAC[out]      - 哈希值(20 Bytes)
 *  Src/Len[in]    - 源数据
 *  Key/KeyLen[in] - 混淆Key: NULL时，输出标准哈希值
 *Return:
**/
DLL_IMPORT void sha1HMAC(unsigned char HMAC[20],
                         const unsigned char *Src, size_t Len,
                         const unsigned char *Key = NULL, size_t KeyLen = 0);
/**
 *Description:
 *  sha256HMAC.
 *Paramter:
 *  HMAC[out]      - 哈希值(32 Bytes)
 *  Src/Len[in]    - 源数据
 *  Key/KeyLen[in] - 混淆Key: NULL时，输出标准哈希值
 *Return:
**/
DLL_IMPORT void sha256HMAC(unsigned char HMAC[32],
                           const unsigned char *Src, size_t Len,
                           const unsigned char *Key = NULL, size_t KeyLen = 0);
/**
 *Description:
 *  sha384HMAC.
 *Paramter:
 *  HMAC[out]      - 哈希值(48 Bytes)
 *  Src/Len[in]    - 源数据
 *  Key/KeyLen[in] - 混淆Key: NULL时，输出标准哈希值
 *Return:
**/
DLL_IMPORT void sha384HMAC(unsigned char HMAC[48],
                           const unsigned char *Src, size_t Len,
                           const unsigned char *Key = NULL, size_t KeyLen = 0);
/**
 *Description:
 *  sha512HMAC.
 *Paramter:
 *  HMAC[out]      - 哈希值(64 Bytes)
 *  Src/Len[in]    - 源数据
 *  Key/KeyLen[in] - 混淆Key: NULL时，输出标准哈希值
 *Return:
**/
DLL_IMPORT void sha512HMAC(unsigned char HMAC[64],
                           const unsigned char *Src, size_t Len,
                           const unsigned char *Key = NULL, size_t KeyLen = 0);
//-----------------------------------------------------------------------------


/**
 *Description:
 *  xTea加密(ECB模式).
 *parameters:
 *  Mode[in]    - 1:加密; 0:解密
 *  Key[in]     - Key(8字节)
 *  Src/Len[in] - 明文(8字节块, 不满8字节，自动补0)
 *  Dest[out]   - 密文(8字节块)
 *Return:
 *  0 - 失败
 * >0 - 返回加密后的数据长度(8的倍数)
**/
DLL_IMPORT int xTeaEncrypt(const char Mode, const unsigned char Key[8],
                           const unsigned char* Src, const int Len,
                           unsigned char* Dest);

/**
 *Description:
 *  xxTEA/Base64字符串简单加/解密.
 *Parameter：
 *  Mode      - 1:加密; 0-解密
 *  Dest[out] - 输出字符串(以\0结尾)
 *  Src[in]   - 输入字符串(以\0结尾)
 *  Key[in]   - 混淆Key
 *Return：
 *  >0 - 成功(返回加解密后的数据长度)
 * <=0 - 失败
**/
DLL_IMPORT int xxTeaString(char Mode, char *Dest, char* Src, char* Key = NULL);

/**
 *Description:
 *  DES加密(ECB模式).
 *parameters:
 *  Mode[in]    - 1:加密; 0:解密
 *  Key[in]     - Key(8 bytes)
 *  Src/Len[in] - 明文(8字节块, 不满8字节，自动补0)
 *  Dest[out]   - 密文(8字节块)
 *Return:
 *  0 - 失败
 * >0 - 返回加密后的数据长度(8的倍数)
**/
DLL_IMPORT int DesEncrypt(const char Mode, const unsigned char Key[8],
                          const unsigned char* Src, const int Len,
                          unsigned char* Dest);

/**
 *Description:
 *  3DES加密(ECB模式).
 *parameters:
 *  Mode[in]    - 1:加密; 0:解密
 *  Key[in]     - Key(16 bytes)
 *  Src/Len[in] - 明文(8字节块, 不满8字节，自动补0)
 *  Dest[out]   - 密文(8字节块)
 *Return:
 *  0 - 失败
 * >0 - 返回加密后的数据长度(8的倍数)
**/
DLL_IMPORT int Des3Encrypt(const char Mode, const unsigned char Key[16],
                           const unsigned char* Src, const int Len,
                           unsigned char* Dest);

/**
 *Description:
 *  AES-128/192/256-bits加密(ECB模式).
 *parameters:
 *  Mode[in]       - 1:加密; 0:解密
 *  Key/KeyLen[in] - Key(16/24/32字节)
 *  Src/Len[in]    - 明文(16字节块, 不满16字节，自动补0)
 *  Dest[out]      - 密文(16字节块)
 *Return:
 *  0 - 失败
 * >0 - 返回加密后的数据长度(16的倍数)
**/
DLL_IMPORT int AesEncrypt(char Mode,
                          const unsigned char* Key, const int KeyLen,
                          const unsigned char* Src, const int Len,
                          unsigned char* Dest);

/**
 * Function:
 *   IDEA加密(ECB模式).
 *Input:
 *  Mode[in]    - 1:加密，0:解密
 *  Key[in]     - 密钥(16 bytes)
 *  Src/Len[in] - 输入数据(8的倍数，不足8字节，自动补0)
 *  Dest[out]   - 输出数据
 *Return:
 *  0 - 失败
 * >0 - 成功, 返回加解密后的数据长度(8的倍数)
**/
DLL_IMPORT int IdeaEncrypt(const char Mode, const unsigned char Key[16],
                           const unsigned char* Src, const size_t Len,
                           unsigned char* Dest);
//-----------------------------------------------------------------------------


/**
 * Function:
 *   LZ77数据压缩.
 *Input:
 *  pOutputBuffer/pulNumberOfBits[out] - 输出的压缩数据
 *  pDataBuffer/ulDataLength - 输入的源数据
 *Memo:
 *  pulNumberOfBits - 转换为字节长度: ulNumberOfBits = (*pulNumberOfBits + 7) >> 3;
**/
DLL_IMPORT void Lz77Compress(unsigned char *pOutputBuffer, unsigned long *pulNumberOfBits,
                             unsigned char *pDataBuffer, unsigned long ulDataLength);
/**
 * Function:
 *   LZ77数据解压缩.
 *Input:
 *  pOutputBuffer/pulNumberOfBytes[out] - 输出的解压缩数据
 *  pDataBuffer/ulNumberOfBits - 输入的源数据
 *Memo:
 *  ulNumberOfBits - 位长度
**/
DLL_IMPORT void Lz77DeCompress(unsigned char *pOutputBuffer, unsigned long *pulNumberOfBytes,
                               unsigned char *pDataBuffer, unsigned long ulNumberOfBits);

/**
 * Function:
 *   文件压缩.
 *Input:
 *  dst_file - 输出的文件名(压缩)
 *  src_file - 输入的文件名
 *Return:
 *  1 - 成功
 *  0 - 失败
**/
DLL_IMPORT int CompressFile(char *dst_file, char *src_file);
/**
 * Function:
 *   文件解压缩.
 *Input:
 *  dst_file - 输出的文件名(解压缩)
 *  src_file - 输入的文件名
 *Return:
 *  1 - 成功
 *  0 - 失败
**/
DLL_IMPORT int DeCompressFile(char *dst_file, char *src_file);
//-----------------------------------------------------------------------------

//---------------------------RSA && X.509--------------------------------------

/**
 *产生RSA-PKCS#1-v1.5密钥对(256..4096-bits)
 *Parameters:
 *  PK/pkLen[out] - 公钥
 *  SK/skLen[out] - 私钥
 *  bits[in]      - 密钥长度位数
 *  format[in]    - 输出密钥格式
 *                  0:DER[二进制格式], 1:PEM[base64格式]
 *                  >2 && <=16: [PK不输出公钥,直接输出公钥模数N (10:十进制，16:十六进制)]
 *  exp[in]       - 公钥指数(素数: 1,3,5,17,257,65537...默认: 65537)
 *Return:
 *  0 - 成功
 * !0 - 失败
 **/
DLL_IMPORT int RSA_GenPaires(unsigned char *PK, size_t *pkLen,
                             unsigned char *SK, size_t *skLen,
                             const int  bits = 1024,
                             const char format = 1,
                             const int  exp = 65537);   //EXPONENT
/**
 *通过给定的模(N)和公钥指数(E)生成RSA公钥(默认:PEM格式)
 *Parameters:
 *  PK/pkLen[out] - 公钥
 *  N[in]         - 模数(字符串, \0结束符)
 *  E[in]         - 公钥指数(字符串, 默认值: radix为十进制时为["65537"]/十六进制时为["10001"])
 *  radix[in]     - 格式(默认: 10[十进制])
 *  format[in]    - 输出密钥格式(0-DER[二进制格式], 默认值: 1-PEM[base64格式])
 *Return:
 *  0 - 成功
 * !0 - 失败
 **/
DLL_IMPORT int RSA_GenPK_NE(unsigned char *PK, size_t *pkLen,
                            char *N,
                            const char *E = "65537",    //EXPONENT
                            const int  radix = 10,
                            const char format = 1);
/**
 *RSA公钥加密.
 *Parameters:
 *  output/outLen[out] - 密文
 *  input/outLen[in]   - 明文
 *  PK/pkLen[in]       - RSA公钥
 *Return:
 *  0 - 成功
 * !0 - 失败(错误代码, RSA_LastError获取错误信息)
 **/
DLL_IMPORT  int RSA_PKEncrypt(unsigned char *output, size_t *outLen,
                              const unsigned char *input, const size_t inLen,
                              const unsigned char *PK, const size_t pkLen);
/**
 *RSA私钥解密.
 *Parameters:
 *  output/outLen[out] - 明文
 *  input/inLen[in]    - 密文
 *  SK/skLen[in]       - RSA私钥
 *Return:
 *  0 - 成功
 * !0 - 失败(错误代码, RSA_LastError获取错误信息)
 **/
DLL_IMPORT  int RSA_SKDecrypt(unsigned char *output, size_t *outLen,
                              const unsigned char *input, const size_t inLen,
                              const unsigned char *SK, const size_t skLen,
                              const unsigned char *pwd = NULL, const size_t pwdLen = 0);
/**
 *RSA私钥加密.
 *Parameters:
 *  output/outLen[out] - 密文
 *  input/outLen[in]   - 明文
 *  PK/pkLen[in]       - RSA私钥
 *Return:
 *  0 - 成功
 * !0 - 失败(错误代码, RSA_LastError获取错误信息)
 **/
DLL_IMPORT  int RSA_SKEncrypt(unsigned char *output, size_t *outLen,
                              const unsigned char *input, const size_t inLen,
                              const unsigned char *SK, const size_t skLen,
                              const unsigned char *pwd = NULL, const size_t pwdLen = 0);

/**
 *RSA公钥解密.
 *Parameters:
 *  output/outLen[out] - 密文
 *  input/outLen[in]   - 明文
 *  PK/pkLen[in]       - RSA公钥
 *Return:
 *  0 - 成功
 * !0 - 失败(错误代码, RSA_LastError获取错误信息)
 **/
DLL_IMPORT  int RSA_PKDecrypt(unsigned char *output, size_t *outLen,
                              const unsigned char *input, const size_t inLen,
                              const unsigned char *PK, const size_t pkLen);

/**
 *私钥签名.
 *Parameters:
 *  sig/sigLen[out] - 输出的签名(16进制)
 *  data/datLen[in] - 需要签名的数据
 *  SK/skLen[in]    - 私钥
 *  PWD/pwdLen[in]  - 私钥的保护口令(缺省: NULL)
 *Return:
 *  0 - 成功
 * !0 - 失败
 **/
DLL_IMPORT  int RSA_SKSign(unsigned char *sig, size_t *sigLen,
                           const unsigned char *data, const size_t datLen,
                           const unsigned char *SK, const size_t skLen,
                           const unsigned char *pwd = NULL, const size_t pwdLen = 0);
/**
 *公钥验签.
 *Parameters:
 *  sig/sigLen[in]  - 需要验证的签名(16进制)
 *  data/datLen[in] - 需要验签的数据
 *  PK/pkLen[in]    - 公钥(自动判断PEM/DER格式)
 *Return:
 *  0 - 成功
 * !0 - 失败
 *注:
 *  PK - 可以使用[X509_CertInfo]函数从x509证书中提取公钥.
 **/
DLL_IMPORT  int RSA_PKVerify(const unsigned char *sig, const size_t sigLen,
                             const unsigned char *data, const size_t datLen,
                             const unsigned char *PK, const size_t pkLen);


/**
 * 输出错误信息.
 *Parameters:
 *  errno[in]      - 错误代码
 *Return:
 *  error message.
**/
DLL_IMPORT char *RSA_LastError(const int rsa_errno);
//-----------------------------------------------------------------------------

/*
 * X.509 v3 Key Usage Extension flags
 */
#define KU_DIGITAL_SIGNATURE            (0x80)  /* bit 0 */
#define KU_NON_REPUDIATION              (0x40)  /* bit 1 */
#define KU_KEY_ENCIPHERMENT             (0x20)  /* bit 2 */
#define KU_DATA_ENCIPHERMENT            (0x10)  /* bit 3 */
#define KU_KEY_AGREEMENT                (0x08)  /* bit 4 */
#define KU_KEY_CERT_SIGN                (0x04)  /* bit 5 */
#define KU_CRL_SIGN                     (0x02)  /* bit 6 */

/*
 * Netscape certificate types
 */
#define NS_CERT_TYPE_SSL_CLIENT         (0x80)  /* bit 0 */
#define NS_CERT_TYPE_SSL_SERVER         (0x40)  /* bit 1 */
#define NS_CERT_TYPE_EMAIL              (0x20)  /* bit 2 */
#define NS_CERT_TYPE_OBJECT_SIGNING     (0x10)  /* bit 3 */
#define NS_CERT_TYPE_RESERVED           (0x08)  /* bit 4 */
#define NS_CERT_TYPE_SSL_CA             (0x04)  /* bit 5 */
#define NS_CERT_TYPE_EMAIL_CA           (0x02)  /* bit 6 */
#define NS_CERT_TYPE_OBJECT_SIGNING_CA  (0x01)  /* bit 7 */

#define CERT_MAXSIZE                    (8192)  /* maximum certificate size */
#define NAME_MAXSIZE                    (512)   /* maximum name size */
#define PASS_MAXSIZE                    (32)    /* maximum password size */

/*证书配置*/
typedef struct _cert_options {
	unsigned char issuer_crt[CERT_MAXSIZE];     /* the issuer certificate, if specified, issuer_name is ignored */
	int  issuer_crt_len;
	unsigned char issuer_name[NAME_MAXSIZE];    /* issuer name for certificate */
	unsigned char issuer_key[CERT_MAXSIZE];     /* the issuer key */
	int  issuer_key_len;
	char issuer_pwd[PASS_MAXSIZE];              /* password for the issuer key */

	unsigned char request_csr[CERT_MAXSIZE];    /* the certificate request, if specified, subject_name|subject_key|subject_pwd is ignored */
	int  request_csr_len;
    int request_csr_type;						/* 0:标准VeriSign[CSR]格式(默认值), 1:天威诚信[CRT]格式(已经自签名) */

	unsigned char subject_name[NAME_MAXSIZE];   /* subject name for certificate */
	unsigned char subject_key[CERT_MAXSIZE];    /* the subject key */
	int subject_key_len;
	char subject_pwd[PASS_MAXSIZE];             /* password for the subject key */

	char not_before[16];                        /* validity period not before, such as: 20140101000000 */
	char not_after[16];                         /* validity period not after,  such as: 20301231235959 */
	char serial[33];                            /* serial number string */
	int selfsign;                               /* selfsign the certificate(0|1) */
	int is_ca;                                  /* is a CA certificate */
	int max_pathlen;                            /* maximum CA path length(-1..127) */
	unsigned char key_usage;                    /* key usage flags */
	unsigned char ns_cert_type;                 /* NS cert type */

	unsigned char output_buf[CERT_MAXSIZE];     /* output certificate */
	int output_len;                             /* output sizeof */
	int output_type;                            /* 输出类型:  0-DER(二进制), 1-PEM(默认:base64格式) */

	_cert_options() {
		memset(issuer_crt, 0, sizeof(issuer_crt));
		issuer_crt_len = 0;
		memset(issuer_key, 0, sizeof(issuer_key));
		memset(issuer_pwd, 0, sizeof(issuer_pwd));

		memset(request_csr, 0, sizeof(request_csr));
		request_csr_len = 0;

		memset(subject_key, 0, sizeof(subject_key));
		subject_key_len = 0;
		memset(subject_pwd, 0, sizeof(subject_pwd));

		memset(output_buf, 0, sizeof(output_buf));
		output_len = 0;
		output_type = 1;
		/*
		 *issuer_name|subject_name RFC keyworkd.
		 *CN = common name              (通用名)
		 *OU = organizational unitname  (部门名)
		 * O = organization name        (组织/公司名)
		 * L = locality name            (城市名)
		 * S = province name            (省名)
		 *ST = state name               (州名)
		 * C = country name             (国家名)
		 * R = email address            (电子邮件)
		 * serialNumber =               (系列号)
		 * postalAddress =              (邮政地址)
		 * postalCode =                 (邮编)
		 */
		strcpy((char *)issuer_name,  "CN=www.site.com, OU=department, O=site.com, C=CN");   /*颁发者*/
		strcpy((char *)subject_name, "CN=www.site.com, OU=department, O=site.com, C=CN");   /*使用者*/
		strcpy(not_before, "20150101000000");
		strcpy(not_after,  "20301231235959");
		strcpy(serial, "1");                                    /* 系列号 */

        request_csr_type = 0;									/* 证书请求格式 */
		max_pathlen = -1;                                       /* 证书链最大路径(-1..127) */
		selfsign = 1;                                           /* 自签名 */
		is_ca = 0;                                              /* 是CA证书? */

		key_usage = 0;          //证书使用目的: KU_KEY_CERT_SIGN | KU_DIGITAL_SIGNATURE | KU_KEY_ENCIPHERMENT;
		ns_cert_type = 0;       //证书类型: NS_CERT_TYPE_SSL_SERVER | NS_CERT_TYPE_SSL_CA;
	};
} cert_options;

/*证书请求配置*/
typedef struct _csr_options {
	unsigned char subject_name[NAME_MAXSIZE];   /* subject name for certificate */
	unsigned char subject_key[CERT_MAXSIZE];    /* the subject key */
	int subject_key_len;
	char subject_pwd[PASS_MAXSIZE];             /* password for the subject key */

	unsigned char key_usage;                    /* key usage flags */
	unsigned char ns_cert_type;                 /* NS cert type */

	unsigned char output_buf[CERT_MAXSIZE];     /* output certificate */
	int output_len;                             /* output sizeof */
	int output_type;                            /* 输出类型:  0-DER(二进制), 1-PEM(默认:base64格式) */

	_csr_options() {
		memset(subject_key, 0, sizeof(subject_key));
		subject_key_len = 0;
		memset(subject_pwd, 0, sizeof(subject_pwd));

		memset(output_buf, 0, sizeof(output_buf));
		output_len = 0;
		output_type = 1;

		key_usage = 0;          //证书使用目的: KU_DIGITAL_SIGNATURE | KU_KEY_ENCIPHERMENT | KU_DATA_ENCIPHERMENT;
		ns_cert_type = 0;       //证书类型: NS_CERT_TYPE_SSL_CLIENT | NS_CERT_TYPE_SSL_SERVER;
		strcpy((char *)subject_name, "CN=www.site.com, OU=department, O=site.com, C=CN");   /*使用者*/
	};
} csr_options;

/**
 * 生成或签发X509证书[CRT/SCR/CRL].
 *Parameters:
 *  opts[in|out] - 证书配置
 *Return:
 *  0 - 成功
 * !0 - 失败(-1:证书链验证失败, 其他: RSA_GetLastError获取错误信息)
**/
DLL_IMPORT  int X509_Certificate(cert_options *opts);

/**
 * 生成X509证书请求[CSR].
 *Parameters:
 *  opts[in|out] - 请求证书的配置
 *Return:
 *  0 - 成功
 * !0 - 失败(错误代码, RSA_LastError获取错误信息)
**/
DLL_IMPORT  int X509_CertRequest(csr_options *opts);

/**
 * 获取X509[CRT/CSR/CRL]证书相关信息.
 *Parameters:
 *  out_msg/msg_size[in/out] - 输出证书的信息
 *  PK/pkLen[in/out]         - 输出证书中的公钥
 *  x509_buf/buf_size[in]    - 输入需解析的证书
 *  format[in]               - 输出公钥的格式(0:DER[二进制格式], 1:PEM[base64格式])
 *Return:
 *  0 - 成功(返回证书信息长度, [TAB..换行]分割)
 * !0 - 失败(错误代码, RSA_LastError获取错误信息)
 *Momo:
 * 暂时不支持CRL(吊销列表)证书.
**/
DLL_IMPORT  int X509_CertInfo(char *out_msg, size_t *msg_size,
                              unsigned char *PK, size_t *pkLen,
                              const unsigned char *x509_buf, const size_t buf_size,
                              const char format = 1);
//-----------------------------------------------------------------------------

/**
 * 建立SSL链接.
 * 参数:
 *  ip/port[in]        - 服务器地址
 *  caBuf/caLen[in]    - 服务器公钥证书
 *  check_cn[in]       - 是否校验证书使用者的通用名称(CN), 默认: true
 *  cliCert/cliLen[in] - 客户端公钥证书(保留未用)
 *  keyCert/keyLen[in] - 客户端私钥证书(保留未用)
 * 返回值:
 *   >0: 成功(返回SSL连接句柄:        ssl_fd)
 *    0: 失败(参数错，或者已达SSL最大连接数)
 *   <0: 失败(返回SSL连接句柄[负值]: -ssl_fd)
 * 备注:
 *   1. 暂时仅支持SSL单向链接;
 *   2. 每进程最大支持256个独立SSL连接;
 *   3. 线程安全.
**/
DLL_IMPORT  int  ssl_connect(const char *ip, const int port,
                             const unsigned char *caBuf, const size_t caLen,
                             const bool check_cn = true,
                             const unsigned char *cliCert = NULL, const size_t cliLen = 0,
                             const unsigned char *keyCert = NULL, const size_t keyLen = 0);
/**
 * SSL发送/接收数据.
 *Parameters:
 *  ssl_fd[in]   - ssl_connect返回的SSL句柄
 *  buf[out]     - 消息缓存区
 *  msg_len[in]  - 发送消息长度
 *  buf_size[in] - buf缓存区大小
 *Return:
 *  >0 - 成功(返回接收的消息长度)
 * <=0 - 失败
**/
DLL_IMPORT  int  ssl_send_recv(const int ssl_fd, unsigned char *buf, const int msg_len, const size_t buf_size);

/**
 * 关闭一个SSL链接.
 *Parameters:
 *  ssl_fd[in]   - ssl_connect返回的SSL句柄
 *Return:
 *  无.
**/
DLL_IMPORT void  ssl_close(const int ssl_fd);

/**
 * 返回最后SSL操作错误信息.
 *Parameters:
 *  ssl_fd[in]   - ssl_connect返回的SSL句柄
 *Return:
 *  最后的SSL操作错误信息.
**/
DLL_IMPORT char* ssl_last_error(const int ssl_fd);
//-----------------------------------------------------------------------------



#endif  //_toolkits4_H_
