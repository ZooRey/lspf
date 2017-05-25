#ifndef __RSA_CRYPTO_H__
#define __RSA_CRYPTO_H__

#include "define_crypto.h"
#include <string>
#include <vector>
#include <openssl/rsa.h>
#include <openssl/x509.h>
using namespace std;

#define X509_EXT_COUNT 5

class RsaCrypto
{
public:
	RsaCrypto() {}
	~RsaCrypto() {}

public:
	//PEM
	static int PubKeyDecryptByBuffer(const string &buffer, const string &src, string &dest, int padding_type=PT_PKCS1);
	static int PubKeyDecryptByBuffer_X509(const string &buffer, const string &src, string &dest, int padding_type=PT_PKCS1);
	static int PubKeyDecryptByFile(const string &buffer, const string &src, string &dest, int padding_type=PT_PKCS1);
	static int PubKeyDecryptByFile_X509(const string &buffer, const string &src, string &dest, int padding_type=PT_PKCS1);

	static int PubKeyEncryptByBuffer(const string &buffer, const string &src, string &dest, int padding_type=PT_PKCS1);
	static int PubKeyEncryptByBuffer_X509(const string &buffer, const string &src, string &dest, int padding_type=PT_PKCS1);
	static int PubKeyEncryptByFile(const string &file, const string &src, string &dest, int padding_type=PT_PKCS1);
	static int PubKeyEncryptByFile_X509(const string &file, const string &src, string &dest, int padding_type=PT_PKCS1);

	static int PubKeyVerifyByBuffer(const string &buffer, const string &data, const string &sign, int hash_type=HT_SHA1);
	static int PubKeyVerifyByBuffer_X509(const string &buffer, const string &data, const string &sign, int hash_type=HT_SHA1);
	static int PubKeyVerifyByFile(const string &file, const string &data, const string &sign, int hash_type=HT_SHA1);
	static int PubKeyVerifyByFile_X509(const string &file, const string &data, const string &sign, int hash_type=HT_SHA1);

	static int PriKeyEncryptByBuffer(const string &buffer, const string &src, string &dest, int padding_type=PT_PKCS1);
	static int PriKeyEncryptByFile(const string &file, const string &src, string &dest, int padding_type=PT_PKCS1);

	static int PriKeyDecryptByBuffer(const string &buffer, const string &src, string &dest, int padding_type=PT_PKCS1);
	static int PriKeyDecryptByFile(const string &file, const string &src, string &dest, int padding_type=PT_PKCS1);
	
	static int PriKeySignByBuffer(const string &buffer, const string &data, string &sign, int hash_type=HT_SHA1);
	static int PriKeySignByFile(const string &file, const string &data, string &sign, int hash_type=HT_SHA1);

	static int PubKeyDecrypt(RSA *rsa_pub_key, const string &src, string &dest, int padding_type=PT_PKCS1);
	static int PubKeyEncrypt(RSA *rsa_pub_key, const string &src, string &dest, int padding_type=PT_PKCS1);
	//sign and verify a X509_SIG ASN1 object inside PKCS#1 padded RSA encryption	
	static int PubKeyVerify(RSA *rsa_pub_key, const string &data, const string &sign, int hash_type=HT_SHA1);
	
	static int PriKeyEncrypt(RSA *rsa_pri_key, const string &src, string &dest, int padding_type=PT_PKCS1);
	static int PriKeyDecrypt(RSA *rsa_pri_key, const string &src, string &dest, int padding_type=PT_PKCS1);
	//sign and verify a X509_SIG ASN1 object inside PKCS#1 padded RSA encryption	
	static int PriKeySign(RSA *rsa_pri_key, const string &data, string &sign, int hash_type=HT_SHA1);

	static RSA* GetPubKeyFromBuffer_X509(const string &buffer);
	static RSA* GetPubKeyFromFile_X509(const string &file);
	static RSA* GetPubKeyFromBuffer(const string &buffer);
	static RSA* GetPubKeyFromFile(const string &file);

	static RSA* GetPriKeyFromBuffer(const string &buffer);
	static RSA* GetPriKeyFromFile(const string &file);

	static EVP_PKEY* GetPubEvpKeyFromBuffer_X509(const string &buffer);
	static EVP_PKEY* GetPubEvpKeyFromFile_X509(const string &file);

	static EVP_PKEY* GetPriEvpKeyFromBuffer(const string &buffer);
	static EVP_PKEY* GetPriEvpKeyFromFile(const string &file);
	
	static X509* GetX509FromBuffer(const string &buffer);
	static X509* GetX509FromFile(const string &file);
	
	static X509_REQ* GetX509ReqFromBuffer(const string &buffer);
	static X509_REQ* GetX509ReqFromFile(const string &file);
	
	static int IssuePubKeyCert(const string &ca_prikey, const string &ca_pubkey_x509, const string &req_cert, int expire_seconds, CertInfo &cert_info);
	
	//type: 0---Decimal, 1---Hex
	static string GetCertIDFromBuffer(const string &buffer, int type = 0);
	static string GetCertIDFromFile(const string &file, int type = 0);
	static string GetCertIDFromX509(X509 *x509, int type = 0);
	static string GetErrorInfo()
	{
		return m_error_info;
	}	
private:
	static void printHex(const string &title, const string &src);
	
	static void fragmentData(const string &data, vector<string> &vec, int block_len);
	
	static void setErrorInfo(const string &title);
private:
	static string m_error_info;	
	static string m_ext_key[X509_EXT_COUNT];
	static string m_ext_value[X509_EXT_COUNT];
};
#endif //__RSA_CRYPTO_H__
