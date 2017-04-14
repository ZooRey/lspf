/**
 *Unit Name: toolkits4.h
 *Current Version: 4.0
 *Description:
 *   ���ù��ߺ���(��42��)��װ:
 *   BCDת�����������BASE64���롢�Գ�/�ǶԳ�/RSA(512~4096 bits)��SSL���ӡ�ѹ����UTF8ת��...��.
 *Author:
 *   24291878@qq.com
 *Copyright:
 *   Copyright(C) 2014.08
 *History:
 *   ֧��32/64λƽ̨
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
 *ȡ[0, 0xFF]֮��������.
 *Parameters:
 *  pDest[out]  - ��������
 *  uSize[in]   - ���������
 *Return:
 **/
DLL_IMPORT void GetRandom(unsigned char *pDest, unsigned short uSize);

/**
 *ȡ[0, iMax]֮����������.
 *Parameters:
 *  pDest[out]  - ����������
 *  uSize[in]   - �����������
 *  iMaxInt[in] - ����������ֵ(�޶���ֵ��Χ, Ĭ��: 65535)
 *Return:
 **/
DLL_IMPORT void GetRandomInt(unsigned int  *pDest, unsigned short uSize, int iMaxInt = 65535);

/**
 *ȡ[0, 1]֮������ʵ��.
 *Parameters:
 *  pDest[out]  - ������ʵ��
 *  uSize[in]   - ���ʵ������
 *Return:
 **/
DLL_IMPORT void GetRandomReal(double *pDest, unsigned short uSize);
//-----------------------------------------------------------------------------


/**
 *Description:
 *  ѹ��BCDתΪASCII�ַ�����
 *parameter��
 *  Dest[out] - ASCII�ַ���
 *  Src[in] - ��������
 *  Len[in] - ���ݳ���
 *Return��
 *  ������ݳ���(�����볤�ȵ�2�����豣֤�㹻������������)
 **/
DLL_IMPORT int asc2bcd(unsigned char *Dest, char* Src);

/**
 *Description:
 *  ASCII�ַ���תΪѹ��BCD.
 *Parameter��
 *  Dest[out] - �������
    Src[in]   - ԴBCD�ַ���
 *Return��
 *  �ɹ� - ����������ݳ���(���볤�ȵ�һ��)
 *  ʧ�� - ����0
**/
DLL_IMPORT int bcd2asc(char *Dest, unsigned char* Src, int Len);

/**
 *Description:
 *  Base64����.
 *Parameter:
 *  base64_out/base64_len[out] - ����
    src_in/src_len[in]         - Դ����
 *Return:
 *   0 - �ɹ�
 *  -1 - ʧ��
**/
DLL_IMPORT int Base64Encode(char* base64_out, size_t *base64_len,
                            const unsigned char* src_in, size_t src_len);

/**
 *Description:
 *  Base64����.
 *Parameter:
 *  dest_out/dest_len[out] - ��������
    base64_in[in]          - base64��������
 *Return:
 *  0 - �ɹ�
 * -1 - ʧ��
**/
DLL_IMPORT int Base64Decode(unsigned char* dest_out, size_t *dest_len, const char* base64_in);

/**
 *Description:
 *  Ansi(GB2312)�ַ���תΪUTF-8�ַ���.
 *Paramter:
 *  utf8[out] - ������
 *  utf8_size[in] - �����������С
 *  gbk/gbk_size[in] - ��������
 *Return:
 *  0 - ʧ��
 * >0 - �ɹ�, ����ת���ɹ����ַ�����С.
**/
DLL_IMPORT int gbk_to_utf8(unsigned char *utf8, size_t utf8_size,
                           const unsigned char *gbk, const size_t gbk_size);

/**
 *Description:
 *  UTF-8�ַ���תAnsi(GB2312)�ַ���.
 *Paramter:
 *  gbk[out] - ������
 *  gbk_size[in] - �����������С
 *  utf8/utf8_size[in] - ��������
 *Return:
 *  0 - ʧ��
 * >0 - �ɹ�, ����ת���ɹ����ַ�����С.
**/
DLL_IMPORT int utf8_to_gbk(unsigned char *gbk, size_t gbk_size,
                           const unsigned char *utf8, const size_t utf8_size);
//-----------------------------------------------------------------------------


/**
 *Description:
 *  ����16λУ���.
 *Input��
 *  Data - ��������
 *  DataLen - ���ݳ���
 *Output��
 *Return��
 *  CRCУ���.
 **/
DLL_IMPORT unsigned short crc16(unsigned char* Src, int Len);

/**
 *Description:
 *  ����32λУ���.
 *Paramter:
 *  Dest[out]   - ������(4 Bytes)
 *  Src/Len[in] - ��������
 *Return:
**/
DLL_IMPORT unsigned int crc32(unsigned char* Src, int Len);

/**
 *Description:
 *  MD2 hash.
 *Input:
 *  HMAC[out]      - ��ϣֵ(16 Bytes)
 *  Src/Len[in]    - Դ����
 *  Key/KeyLen[in] - ����Key: NULLʱ�������׼��ϣֵ.
 *Return:
 **/
DLL_IMPORT void md2HMAC(unsigned char HMAC[16],
                        const unsigned char* Src, size_t Len,
                        const unsigned char *Key = NULL, size_t KeyLen = 0);
/**
 *Description:
 *  MD4 hash.
 *Input:
 *  HMAC[out]      - ��ϣֵ(16 Bytes)
 *  Src/Len[in]    - Դ����
 *  Key/KeyLen[in] - ����Key: NULLʱ�������׼��ϣֵ.
 *Return:
 **/
DLL_IMPORT void md4HMAC(unsigned char HMAC[16],
                        const unsigned char* Src, size_t Len,
                        const unsigned char *Key = NULL, size_t KeyLen = 0);

/**
 *Description:
 *  MD5 hash.
 *Input:
 *  HMAC[out]      - ��ϣֵ(16 Bytes)
 *  Src/Len[in]    - Դ����
 *  Key/KeyLen[in] - ����Key: NULLʱ�������׼��ϣֵ.
 *Return:
 **/
DLL_IMPORT void md5HMAC(unsigned char HMAC[16],
                        const unsigned char* Src, size_t Len,
                        const unsigned char *Key = NULL, size_t KeyLen = 0);
/**
 *Description:
 *  ripemd160HMAC.
 *Paramter:
 *  HMAC[out]      - ��ϣֵ(20 Bytes)
 *  Src/Len[in]    - Դ����
 *  Key/KeyLen[in] - ����Key: NULLʱ�������׼��ϣֵ.
 *Return:
**/
DLL_IMPORT void ripemd160HMAC(unsigned char HMAC[20],
                              const unsigned char *Src, size_t Len,
                              const unsigned char *Key = NULL, size_t KeyLen = 0);
/**
 *Description:
 *  sha1HMAC.
 *Paramter:
 *  HMAC[out]      - ��ϣֵ(20 Bytes)
 *  Src/Len[in]    - Դ����
 *  Key/KeyLen[in] - ����Key: NULLʱ�������׼��ϣֵ
 *Return:
**/
DLL_IMPORT void sha1HMAC(unsigned char HMAC[20],
                         const unsigned char *Src, size_t Len,
                         const unsigned char *Key = NULL, size_t KeyLen = 0);
/**
 *Description:
 *  sha256HMAC.
 *Paramter:
 *  HMAC[out]      - ��ϣֵ(32 Bytes)
 *  Src/Len[in]    - Դ����
 *  Key/KeyLen[in] - ����Key: NULLʱ�������׼��ϣֵ
 *Return:
**/
DLL_IMPORT void sha256HMAC(unsigned char HMAC[32],
                           const unsigned char *Src, size_t Len,
                           const unsigned char *Key = NULL, size_t KeyLen = 0);
/**
 *Description:
 *  sha384HMAC.
 *Paramter:
 *  HMAC[out]      - ��ϣֵ(48 Bytes)
 *  Src/Len[in]    - Դ����
 *  Key/KeyLen[in] - ����Key: NULLʱ�������׼��ϣֵ
 *Return:
**/
DLL_IMPORT void sha384HMAC(unsigned char HMAC[48],
                           const unsigned char *Src, size_t Len,
                           const unsigned char *Key = NULL, size_t KeyLen = 0);
/**
 *Description:
 *  sha512HMAC.
 *Paramter:
 *  HMAC[out]      - ��ϣֵ(64 Bytes)
 *  Src/Len[in]    - Դ����
 *  Key/KeyLen[in] - ����Key: NULLʱ�������׼��ϣֵ
 *Return:
**/
DLL_IMPORT void sha512HMAC(unsigned char HMAC[64],
                           const unsigned char *Src, size_t Len,
                           const unsigned char *Key = NULL, size_t KeyLen = 0);
//-----------------------------------------------------------------------------


/**
 *Description:
 *  xTea����(ECBģʽ).
 *parameters:
 *  Mode[in]    - 1:����; 0:����
 *  Key[in]     - Key(8�ֽ�)
 *  Src/Len[in] - ����(8�ֽڿ�, ����8�ֽڣ��Զ���0)
 *  Dest[out]   - ����(8�ֽڿ�)
 *Return:
 *  0 - ʧ��
 * >0 - ���ؼ��ܺ�����ݳ���(8�ı���)
**/
DLL_IMPORT int xTeaEncrypt(const char Mode, const unsigned char Key[8],
                           const unsigned char* Src, const int Len,
                           unsigned char* Dest);

/**
 *Description:
 *  xxTEA/Base64�ַ����򵥼�/����.
 *Parameter��
 *  Mode      - 1:����; 0-����
 *  Dest[out] - ����ַ���(��\0��β)
 *  Src[in]   - �����ַ���(��\0��β)
 *  Key[in]   - ����Key
 *Return��
 *  >0 - �ɹ�(���ؼӽ��ܺ�����ݳ���)
 * <=0 - ʧ��
**/
DLL_IMPORT int xxTeaString(char Mode, char *Dest, char* Src, char* Key = NULL);

/**
 *Description:
 *  DES����(ECBģʽ).
 *parameters:
 *  Mode[in]    - 1:����; 0:����
 *  Key[in]     - Key(8 bytes)
 *  Src/Len[in] - ����(8�ֽڿ�, ����8�ֽڣ��Զ���0)
 *  Dest[out]   - ����(8�ֽڿ�)
 *Return:
 *  0 - ʧ��
 * >0 - ���ؼ��ܺ�����ݳ���(8�ı���)
**/
DLL_IMPORT int DesEncrypt(const char Mode, const unsigned char Key[8],
                          const unsigned char* Src, const int Len,
                          unsigned char* Dest);

/**
 *Description:
 *  3DES����(ECBģʽ).
 *parameters:
 *  Mode[in]    - 1:����; 0:����
 *  Key[in]     - Key(16 bytes)
 *  Src/Len[in] - ����(8�ֽڿ�, ����8�ֽڣ��Զ���0)
 *  Dest[out]   - ����(8�ֽڿ�)
 *Return:
 *  0 - ʧ��
 * >0 - ���ؼ��ܺ�����ݳ���(8�ı���)
**/
DLL_IMPORT int Des3Encrypt(const char Mode, const unsigned char Key[16],
                           const unsigned char* Src, const int Len,
                           unsigned char* Dest);

/**
 *Description:
 *  AES-128/192/256-bits����(ECBģʽ).
 *parameters:
 *  Mode[in]       - 1:����; 0:����
 *  Key/KeyLen[in] - Key(16/24/32�ֽ�)
 *  Src/Len[in]    - ����(16�ֽڿ�, ����16�ֽڣ��Զ���0)
 *  Dest[out]      - ����(16�ֽڿ�)
 *Return:
 *  0 - ʧ��
 * >0 - ���ؼ��ܺ�����ݳ���(16�ı���)
**/
DLL_IMPORT int AesEncrypt(char Mode,
                          const unsigned char* Key, const int KeyLen,
                          const unsigned char* Src, const int Len,
                          unsigned char* Dest);

/**
 * Function:
 *   IDEA����(ECBģʽ).
 *Input:
 *  Mode[in]    - 1:���ܣ�0:����
 *  Key[in]     - ��Կ(16 bytes)
 *  Src/Len[in] - ��������(8�ı���������8�ֽڣ��Զ���0)
 *  Dest[out]   - �������
 *Return:
 *  0 - ʧ��
 * >0 - �ɹ�, ���ؼӽ��ܺ�����ݳ���(8�ı���)
**/
DLL_IMPORT int IdeaEncrypt(const char Mode, const unsigned char Key[16],
                           const unsigned char* Src, const size_t Len,
                           unsigned char* Dest);
//-----------------------------------------------------------------------------


/**
 * Function:
 *   LZ77����ѹ��.
 *Input:
 *  pOutputBuffer/pulNumberOfBits[out] - �����ѹ������
 *  pDataBuffer/ulDataLength - �����Դ����
 *Memo:
 *  pulNumberOfBits - ת��Ϊ�ֽڳ���: ulNumberOfBits = (*pulNumberOfBits + 7) >> 3;
**/
DLL_IMPORT void Lz77Compress(unsigned char *pOutputBuffer, unsigned long *pulNumberOfBits,
                             unsigned char *pDataBuffer, unsigned long ulDataLength);
/**
 * Function:
 *   LZ77���ݽ�ѹ��.
 *Input:
 *  pOutputBuffer/pulNumberOfBytes[out] - ����Ľ�ѹ������
 *  pDataBuffer/ulNumberOfBits - �����Դ����
 *Memo:
 *  ulNumberOfBits - λ����
**/
DLL_IMPORT void Lz77DeCompress(unsigned char *pOutputBuffer, unsigned long *pulNumberOfBytes,
                               unsigned char *pDataBuffer, unsigned long ulNumberOfBits);

/**
 * Function:
 *   �ļ�ѹ��.
 *Input:
 *  dst_file - ������ļ���(ѹ��)
 *  src_file - ������ļ���
 *Return:
 *  1 - �ɹ�
 *  0 - ʧ��
**/
DLL_IMPORT int CompressFile(char *dst_file, char *src_file);
/**
 * Function:
 *   �ļ���ѹ��.
 *Input:
 *  dst_file - ������ļ���(��ѹ��)
 *  src_file - ������ļ���
 *Return:
 *  1 - �ɹ�
 *  0 - ʧ��
**/
DLL_IMPORT int DeCompressFile(char *dst_file, char *src_file);
//-----------------------------------------------------------------------------

//---------------------------RSA && X.509--------------------------------------

/**
 *����RSA-PKCS#1-v1.5��Կ��(256..4096-bits)
 *Parameters:
 *  PK/pkLen[out] - ��Կ
 *  SK/skLen[out] - ˽Կ
 *  bits[in]      - ��Կ����λ��
 *  format[in]    - �����Կ��ʽ
 *                  0:DER[�����Ƹ�ʽ], 1:PEM[base64��ʽ]
 *                  >2 && <=16: [PK�������Կ,ֱ�������Կģ��N (10:ʮ���ƣ�16:ʮ������)]
 *  exp[in]       - ��Կָ��(����: 1,3,5,17,257,65537...Ĭ��: 65537)
 *Return:
 *  0 - �ɹ�
 * !0 - ʧ��
 **/
DLL_IMPORT int RSA_GenPaires(unsigned char *PK, size_t *pkLen,
                             unsigned char *SK, size_t *skLen,
                             const int  bits = 1024,
                             const char format = 1,
                             const int  exp = 65537);   //EXPONENT
/**
 *ͨ��������ģ(N)�͹�Կָ��(E)����RSA��Կ(Ĭ��:PEM��ʽ)
 *Parameters:
 *  PK/pkLen[out] - ��Կ
 *  N[in]         - ģ��(�ַ���, \0������)
 *  E[in]         - ��Կָ��(�ַ���, Ĭ��ֵ: radixΪʮ����ʱΪ["65537"]/ʮ������ʱΪ["10001"])
 *  radix[in]     - ��ʽ(Ĭ��: 10[ʮ����])
 *  format[in]    - �����Կ��ʽ(0-DER[�����Ƹ�ʽ], Ĭ��ֵ: 1-PEM[base64��ʽ])
 *Return:
 *  0 - �ɹ�
 * !0 - ʧ��
 **/
DLL_IMPORT int RSA_GenPK_NE(unsigned char *PK, size_t *pkLen,
                            char *N,
                            const char *E = "65537",    //EXPONENT
                            const int  radix = 10,
                            const char format = 1);
/**
 *RSA��Կ����.
 *Parameters:
 *  output/outLen[out] - ����
 *  input/outLen[in]   - ����
 *  PK/pkLen[in]       - RSA��Կ
 *Return:
 *  0 - �ɹ�
 * !0 - ʧ��(�������, RSA_LastError��ȡ������Ϣ)
 **/
DLL_IMPORT  int RSA_PKEncrypt(unsigned char *output, size_t *outLen,
                              const unsigned char *input, const size_t inLen,
                              const unsigned char *PK, const size_t pkLen);
/**
 *RSA˽Կ����.
 *Parameters:
 *  output/outLen[out] - ����
 *  input/inLen[in]    - ����
 *  SK/skLen[in]       - RSA˽Կ
 *Return:
 *  0 - �ɹ�
 * !0 - ʧ��(�������, RSA_LastError��ȡ������Ϣ)
 **/
DLL_IMPORT  int RSA_SKDecrypt(unsigned char *output, size_t *outLen,
                              const unsigned char *input, const size_t inLen,
                              const unsigned char *SK, const size_t skLen,
                              const unsigned char *pwd = NULL, const size_t pwdLen = 0);
/**
 *RSA˽Կ����.
 *Parameters:
 *  output/outLen[out] - ����
 *  input/outLen[in]   - ����
 *  PK/pkLen[in]       - RSA˽Կ
 *Return:
 *  0 - �ɹ�
 * !0 - ʧ��(�������, RSA_LastError��ȡ������Ϣ)
 **/
DLL_IMPORT  int RSA_SKEncrypt(unsigned char *output, size_t *outLen,
                              const unsigned char *input, const size_t inLen,
                              const unsigned char *SK, const size_t skLen,
                              const unsigned char *pwd = NULL, const size_t pwdLen = 0);

/**
 *RSA��Կ����.
 *Parameters:
 *  output/outLen[out] - ����
 *  input/outLen[in]   - ����
 *  PK/pkLen[in]       - RSA��Կ
 *Return:
 *  0 - �ɹ�
 * !0 - ʧ��(�������, RSA_LastError��ȡ������Ϣ)
 **/
DLL_IMPORT  int RSA_PKDecrypt(unsigned char *output, size_t *outLen,
                              const unsigned char *input, const size_t inLen,
                              const unsigned char *PK, const size_t pkLen);

/**
 *˽Կǩ��.
 *Parameters:
 *  sig/sigLen[out] - �����ǩ��(16����)
 *  data/datLen[in] - ��Ҫǩ��������
 *  SK/skLen[in]    - ˽Կ
 *  PWD/pwdLen[in]  - ˽Կ�ı�������(ȱʡ: NULL)
 *Return:
 *  0 - �ɹ�
 * !0 - ʧ��
 **/
DLL_IMPORT  int RSA_SKSign(unsigned char *sig, size_t *sigLen,
                           const unsigned char *data, const size_t datLen,
                           const unsigned char *SK, const size_t skLen,
                           const unsigned char *pwd = NULL, const size_t pwdLen = 0);
/**
 *��Կ��ǩ.
 *Parameters:
 *  sig/sigLen[in]  - ��Ҫ��֤��ǩ��(16����)
 *  data/datLen[in] - ��Ҫ��ǩ������
 *  PK/pkLen[in]    - ��Կ(�Զ��ж�PEM/DER��ʽ)
 *Return:
 *  0 - �ɹ�
 * !0 - ʧ��
 *ע:
 *  PK - ����ʹ��[X509_CertInfo]������x509֤������ȡ��Կ.
 **/
DLL_IMPORT  int RSA_PKVerify(const unsigned char *sig, const size_t sigLen,
                             const unsigned char *data, const size_t datLen,
                             const unsigned char *PK, const size_t pkLen);


/**
 * ���������Ϣ.
 *Parameters:
 *  errno[in]      - �������
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

/*֤������*/
typedef struct _cert_options {
	unsigned char issuer_crt[CERT_MAXSIZE];     /* the issuer certificate, if specified, issuer_name is ignored */
	int  issuer_crt_len;
	unsigned char issuer_name[NAME_MAXSIZE];    /* issuer name for certificate */
	unsigned char issuer_key[CERT_MAXSIZE];     /* the issuer key */
	int  issuer_key_len;
	char issuer_pwd[PASS_MAXSIZE];              /* password for the issuer key */

	unsigned char request_csr[CERT_MAXSIZE];    /* the certificate request, if specified, subject_name|subject_key|subject_pwd is ignored */
	int  request_csr_len;
    int request_csr_type;						/* 0:��׼VeriSign[CSR]��ʽ(Ĭ��ֵ), 1:��������[CRT]��ʽ(�Ѿ���ǩ��) */

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
	int output_type;                            /* �������:  0-DER(������), 1-PEM(Ĭ��:base64��ʽ) */

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
		 *CN = common name              (ͨ����)
		 *OU = organizational unitname  (������)
		 * O = organization name        (��֯/��˾��)
		 * L = locality name            (������)
		 * S = province name            (ʡ��)
		 *ST = state name               (����)
		 * C = country name             (������)
		 * R = email address            (�����ʼ�)
		 * serialNumber =               (ϵ�к�)
		 * postalAddress =              (������ַ)
		 * postalCode =                 (�ʱ�)
		 */
		strcpy((char *)issuer_name,  "CN=www.site.com, OU=department, O=site.com, C=CN");   /*�䷢��*/
		strcpy((char *)subject_name, "CN=www.site.com, OU=department, O=site.com, C=CN");   /*ʹ����*/
		strcpy(not_before, "20150101000000");
		strcpy(not_after,  "20301231235959");
		strcpy(serial, "1");                                    /* ϵ�к� */

        request_csr_type = 0;									/* ֤�������ʽ */
		max_pathlen = -1;                                       /* ֤�������·��(-1..127) */
		selfsign = 1;                                           /* ��ǩ�� */
		is_ca = 0;                                              /* ��CA֤��? */

		key_usage = 0;          //֤��ʹ��Ŀ��: KU_KEY_CERT_SIGN | KU_DIGITAL_SIGNATURE | KU_KEY_ENCIPHERMENT;
		ns_cert_type = 0;       //֤������: NS_CERT_TYPE_SSL_SERVER | NS_CERT_TYPE_SSL_CA;
	};
} cert_options;

/*֤����������*/
typedef struct _csr_options {
	unsigned char subject_name[NAME_MAXSIZE];   /* subject name for certificate */
	unsigned char subject_key[CERT_MAXSIZE];    /* the subject key */
	int subject_key_len;
	char subject_pwd[PASS_MAXSIZE];             /* password for the subject key */

	unsigned char key_usage;                    /* key usage flags */
	unsigned char ns_cert_type;                 /* NS cert type */

	unsigned char output_buf[CERT_MAXSIZE];     /* output certificate */
	int output_len;                             /* output sizeof */
	int output_type;                            /* �������:  0-DER(������), 1-PEM(Ĭ��:base64��ʽ) */

	_csr_options() {
		memset(subject_key, 0, sizeof(subject_key));
		subject_key_len = 0;
		memset(subject_pwd, 0, sizeof(subject_pwd));

		memset(output_buf, 0, sizeof(output_buf));
		output_len = 0;
		output_type = 1;

		key_usage = 0;          //֤��ʹ��Ŀ��: KU_DIGITAL_SIGNATURE | KU_KEY_ENCIPHERMENT | KU_DATA_ENCIPHERMENT;
		ns_cert_type = 0;       //֤������: NS_CERT_TYPE_SSL_CLIENT | NS_CERT_TYPE_SSL_SERVER;
		strcpy((char *)subject_name, "CN=www.site.com, OU=department, O=site.com, C=CN");   /*ʹ����*/
	};
} csr_options;

/**
 * ���ɻ�ǩ��X509֤��[CRT/SCR/CRL].
 *Parameters:
 *  opts[in|out] - ֤������
 *Return:
 *  0 - �ɹ�
 * !0 - ʧ��(-1:֤������֤ʧ��, ����: RSA_GetLastError��ȡ������Ϣ)
**/
DLL_IMPORT  int X509_Certificate(cert_options *opts);

/**
 * ����X509֤������[CSR].
 *Parameters:
 *  opts[in|out] - ����֤�������
 *Return:
 *  0 - �ɹ�
 * !0 - ʧ��(�������, RSA_LastError��ȡ������Ϣ)
**/
DLL_IMPORT  int X509_CertRequest(csr_options *opts);

/**
 * ��ȡX509[CRT/CSR/CRL]֤�������Ϣ.
 *Parameters:
 *  out_msg/msg_size[in/out] - ���֤�����Ϣ
 *  PK/pkLen[in/out]         - ���֤���еĹ�Կ
 *  x509_buf/buf_size[in]    - �����������֤��
 *  format[in]               - �����Կ�ĸ�ʽ(0:DER[�����Ƹ�ʽ], 1:PEM[base64��ʽ])
 *Return:
 *  0 - �ɹ�(����֤����Ϣ����, [TAB..����]�ָ�)
 * !0 - ʧ��(�������, RSA_LastError��ȡ������Ϣ)
 *Momo:
 * ��ʱ��֧��CRL(�����б�)֤��.
**/
DLL_IMPORT  int X509_CertInfo(char *out_msg, size_t *msg_size,
                              unsigned char *PK, size_t *pkLen,
                              const unsigned char *x509_buf, const size_t buf_size,
                              const char format = 1);
//-----------------------------------------------------------------------------

/**
 * ����SSL����.
 * ����:
 *  ip/port[in]        - ��������ַ
 *  caBuf/caLen[in]    - ��������Կ֤��
 *  check_cn[in]       - �Ƿ�У��֤��ʹ���ߵ�ͨ������(CN), Ĭ��: true
 *  cliCert/cliLen[in] - �ͻ��˹�Կ֤��(����δ��)
 *  keyCert/keyLen[in] - �ͻ���˽Կ֤��(����δ��)
 * ����ֵ:
 *   >0: �ɹ�(����SSL���Ӿ��:        ssl_fd)
 *    0: ʧ��(�����������Ѵ�SSL���������)
 *   <0: ʧ��(����SSL���Ӿ��[��ֵ]: -ssl_fd)
 * ��ע:
 *   1. ��ʱ��֧��SSL��������;
 *   2. ÿ�������֧��256������SSL����;
 *   3. �̰߳�ȫ.
**/
DLL_IMPORT  int  ssl_connect(const char *ip, const int port,
                             const unsigned char *caBuf, const size_t caLen,
                             const bool check_cn = true,
                             const unsigned char *cliCert = NULL, const size_t cliLen = 0,
                             const unsigned char *keyCert = NULL, const size_t keyLen = 0);
/**
 * SSL����/��������.
 *Parameters:
 *  ssl_fd[in]   - ssl_connect���ص�SSL���
 *  buf[out]     - ��Ϣ������
 *  msg_len[in]  - ������Ϣ����
 *  buf_size[in] - buf��������С
 *Return:
 *  >0 - �ɹ�(���ؽ��յ���Ϣ����)
 * <=0 - ʧ��
**/
DLL_IMPORT  int  ssl_send_recv(const int ssl_fd, unsigned char *buf, const int msg_len, const size_t buf_size);

/**
 * �ر�һ��SSL����.
 *Parameters:
 *  ssl_fd[in]   - ssl_connect���ص�SSL���
 *Return:
 *  ��.
**/
DLL_IMPORT void  ssl_close(const int ssl_fd);

/**
 * �������SSL����������Ϣ.
 *Parameters:
 *  ssl_fd[in]   - ssl_connect���ص�SSL���
 *Return:
 *  ����SSL����������Ϣ.
**/
DLL_IMPORT char* ssl_last_error(const int ssl_fd);
//-----------------------------------------------------------------------------



#endif  //_toolkits4_H_
