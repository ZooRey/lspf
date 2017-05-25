#include "rsa_crypto.h"
#include "common/base64.h"
#include "common/string_utility.h"
#include "log/log.h"
#include <openssl/sha.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/err.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>

using namespace std;
#define RSA_PRIKEY_BEGIN 		"-----BEGIN RSA PRIVATE KEY-----"
#define RSA_PRIKEY_END 			"-----END RSA PRIVATE KEY-----"
#define RSA_PUBKEY_BEGIN 		"-----BEGIN PUBLIC KEY-----"
#define RSA_PUBKEY_END 			"-----END PUBLIC KEY-----"
#define RSA_PUBKEY_CERT_BEGIN 	"-----BEGIN CERTIFICATE-----"
#define RSA_PUBKEY_CERT_END 	"-----END CERTIFICATE-----"
#define RSA_REQ_CERT_BEGIN 		"-----BEGIN CERTIFICATE REQUEST-----"
#define RSA_REQ_CERT_END 		"-----END CERTIFICATE REQUEST-----"


#define RSA_DATA_ELN 8192

string RsaCrypto::m_error_info = "";

string RsaCrypto::m_ext_key[X509_EXT_COUNT] =
	{"basicConstraints",
	"subjectKeyIdentifier",
	"authorityKeyIdentifier",
	"keyUsage",
	"nsCertType"};
string RsaCrypto::m_ext_value[X509_EXT_COUNT] =
	{"CA:FALSE",
	"hash",
	"keyid:always",//"keyid:always,issuer:always",
	"digitalSignature,keyEncipherment",
	"client,server"};

void RsaCrypto::printHex(const string &title, const string &src)
{
	string hex;

	StringUtility::bcd2asc(src, hex);
	cout << title << hex << endl;
}

void RsaCrypto::fragmentData(const string &data, vector<string> &vec, int block_len)
{
    size_t data_len = data.length();
    size_t remain_len = 0;
	size_t every_len = 0;
	size_t pos_begin = 0;
	size_t pos_end = 0;
	string sub_data;

    while(data_len > pos_end)
    {
        remain_len = data_len - pos_end;
		every_len = (remain_len > block_len) ? block_len : remain_len;
        pos_end += every_len;
        sub_data = data.substr(pos_begin, every_len);
        pos_begin = pos_end;
        vec.push_back(sub_data);
    }
}

void RsaCrypto::setErrorInfo(const string &title)
{
	ERR_load_crypto_strings();
	char err_buff[512] = {0};

	ERR_error_string_n(ERR_get_error(), err_buff, sizeof(err_buff));
	ERR_clear_error();
	ERR_free_strings();

	m_error_info = title + " failed:" + string(err_buff);
}

/////////////////////////获取公钥(RSA)////////////////////////////////////////////
RSA* RsaCrypto::GetPubKeyFromBuffer_X509(const string &buffer)
{
	EVP_PKEY *pkey = NULL;
	RSA *rsa = NULL;

	pkey = GetPubEvpKeyFromBuffer_X509(buffer);
	if (pkey == NULL)
	{
		return NULL;
	}
	rsa = EVP_PKEY_get1_RSA(pkey);
	EVP_PKEY_free(pkey);

	return rsa;
}

RSA* RsaCrypto::GetPubKeyFromFile_X509(const string &file)
{
	EVP_PKEY *pkey = NULL;
	RSA *rsa = NULL;

	pkey = GetPubEvpKeyFromFile_X509(file);
	if (pkey == NULL)
	{
		return NULL;
	}
	rsa = EVP_PKEY_get1_RSA(pkey);
	EVP_PKEY_free(pkey);

	return rsa;
}

RSA* RsaCrypto::GetPubKeyFromBuffer(const string &buffer)
{
	unsigned int i;
	char *c_pub_key = NULL;
	string tmp;
	string error_info;
	BIO *bio = NULL;
	RSA *rsa_pub_key = NULL;

	tmp = buffer;      //strPublicKey为base64编码的公钥字符串
	if (buffer.find(RSA_PUBKEY_BEGIN) == string::npos)
	{
		for(i = 64; i < buffer.size(); i+=64)
		{
			if(tmp[i] != '\n')
			{
				tmp.insert(i, "\n");
			}
			i++;
		}
		tmp.insert(0, string(RSA_PUBKEY_BEGIN)+"\n");
		tmp.append("\n"+string(RSA_PUBKEY_END)+"\n");
	}

	c_pub_key = const_cast<char *>(tmp.c_str());
	int len = tmp.size();
	if ((bio = BIO_new_mem_buf(c_pub_key, len)) == NULL)       //从字符串读取RSA公钥
	{
		setErrorInfo("BIO_new_mem_buf");
		return NULL;
	}
	rsa_pub_key = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);   //从bio结构中得到rsa结构
	if (!rsa_pub_key)
	{
		setErrorInfo("PEM_read_bio_RSA_PUBKEY");
		BIO_free_all(bio);
		return NULL;
	}

	BIO_free_all(bio);

	return rsa_pub_key;
}

RSA* RsaCrypto::GetPubKeyFromFile(const string &file)
{
	string error_info;
	FILE *fp;
	RSA *rsa_pub_key = NULL;

	fp = fopen(file.c_str(), "r");
	if (fp == NULL)
	{
		m_error_info = string("file is null:") + file;
		return NULL;
	}
	if( (rsa_pub_key=PEM_read_RSA_PUBKEY(fp, NULL, NULL, NULL)) == NULL )
	{
		fclose(fp);
		setErrorInfo("PEM_read_RSA_PUBKEY");
		return NULL;
	}
	fclose(fp);

	return rsa_pub_key;
}

////////////////////////////获取私钥(RSA)/////////////////////////////////////
RSA* RsaCrypto::GetPriKeyFromBuffer(const string &buffer)
{
	unsigned int i;
	char *c_pri_key = NULL;
	string tmp;
	string error_info;
	BIO *bio = NULL;
	RSA *rsa_pri_key = NULL;

	tmp = buffer;      //strPublicKey为base64编码的公钥字符串
	if (buffer.find(RSA_PRIKEY_BEGIN) == string::npos)
	{
		for(i = 64; i < buffer.size(); i+=64)
		{
			if(tmp[i] != '\n')
			{
				tmp.insert(i, "\n");
			}
			i++;
		}
		tmp.insert(0, string(RSA_PRIKEY_BEGIN)+"\n");
		tmp.append("\n"+string(RSA_PRIKEY_END)+"\n");
	}

	c_pri_key = const_cast<char *>(tmp.c_str());
	int len = tmp.size();
	if ((bio = BIO_new_mem_buf(c_pri_key, len)) == NULL)       //从字符串读取RSA公钥
	{
		setErrorInfo("BIO_new_mem_buf");
		return NULL;
	}
	rsa_pri_key = PEM_read_bio_RSAPrivateKey(bio, NULL, 0, NULL);
	if (!rsa_pri_key)
	{
		setErrorInfo("PEM_read_bio_RSAPrivateKey");
		BIO_free_all(bio);
		return NULL;
	}

	BIO_free_all(bio);

	return rsa_pri_key;
}

RSA* RsaCrypto::GetPriKeyFromFile(const string &file)
{
	string error_info;
	FILE *fp;
	RSA *rsa_pri_key = NULL;

	fp = fopen(file.c_str(), "r");
	if (fp == NULL)
	{
		m_error_info = string("file is null:") + file;
		return NULL;
	}
	if( (rsa_pri_key=PEM_read_RSAPrivateKey(fp, NULL, NULL, NULL)) == NULL )
	{
		fclose(fp);
		setErrorInfo("PEM_read_RSAPrivateKey");
		return NULL;
	}
	fclose(fp);

	return rsa_pri_key;
}

///////////////////////////////获取公钥(EVP_PKEY)/////////////////////////////////////
EVP_PKEY* RsaCrypto::GetPubEvpKeyFromBuffer_X509(const string &buffer)
{
	string error_info;
	EVP_PKEY *pkey = NULL;
	X509 *x509 = NULL;

	x509 = GetX509FromBuffer(buffer);
	if (x509 == NULL)
	{
		return NULL;
	}

	pkey = X509_get_pubkey(x509);
	X509_free(x509);
	if (pkey == NULL)
	{
		setErrorInfo("X509_get_pubkey");
		return NULL;
	}

	return pkey;
}

EVP_PKEY* RsaCrypto::GetPubEvpKeyFromFile_X509(const string &file)
{
	string error_info;
	EVP_PKEY *pkey = NULL;
	X509 *x509 = NULL;

	x509 = GetX509FromFile(file);
	if (x509 == NULL)
	{
		return NULL;
	}

	pkey = X509_get_pubkey(x509);
	X509_free(x509);
	if (pkey == NULL)
	{
		setErrorInfo("X509_get_pubkey");
		return NULL;
	}

	return pkey;
}

/////////////////////////////获取私钥(EVP_PKEY)/////////////////////////
EVP_PKEY* RsaCrypto::GetPriEvpKeyFromBuffer(const string &buffer)
{
	unsigned int i;
	char *c_pri_key = NULL;
	string tmp;
	string error_info;
	BIO *bio = NULL;
	EVP_PKEY *pkey = NULL;

	tmp = buffer;      //strPublicKey为base64编码的公钥字符串
	if (buffer.find(RSA_PRIKEY_BEGIN) == string::npos)
	{
		for(i = 64; i < buffer.size(); i+=64)
		{
			if(tmp[i] != '\n')
			{
				tmp.insert(i, "\n");
			}
			i++;
		}
		tmp.insert(0, string(RSA_PRIKEY_BEGIN)+"\n");
		tmp.append("\n"+string(RSA_PRIKEY_END)+"\n");
	}

	c_pri_key = const_cast<char *>(tmp.c_str());
	int len = tmp.size();
	if ((bio = BIO_new_mem_buf(c_pri_key, len)) == NULL)       //从字符串读取RSA公钥
	{
		setErrorInfo("BIO_new_mem_buf");
		return NULL;
	}
	pkey = PEM_read_bio_PrivateKey(bio, NULL, 0, NULL);
	if (!pkey)
	{
		setErrorInfo("PEM_read_bio_PrivateKey");
		BIO_free_all(bio);
		return NULL;
	}

	BIO_free_all(bio);

	return pkey;
}

EVP_PKEY* RsaCrypto::GetPriEvpKeyFromFile(const string &file)
{
	string error_info;
	FILE *fp;
	EVP_PKEY *pkey = NULL;

	fp = fopen(file.c_str(), "r");
	if (fp == NULL)
	{
		m_error_info = string("file is null:") + file;
		return NULL;
	}
	if( (pkey=PEM_read_PrivateKey(fp, NULL, NULL, NULL)) == NULL )
	{
		fclose(fp);
		setErrorInfo("PEM_read_PrivateKey");
		return NULL;
	}
	fclose(fp);

	return pkey;
}

//////////////////////////获取X509//////////////////////////////
X509* RsaCrypto::GetX509FromBuffer(const string &buffer)
{
	unsigned int i;
	char *c_pub_key = NULL;
	string tmp;
	string error_info;
	BIO *bio = NULL;
	X509 *x509 = NULL;

	tmp = buffer;      //strPublicKey为base64编码的公钥字符串
	if (buffer.find(RSA_PUBKEY_CERT_BEGIN) == string::npos)
	{
		for(i = 64; i < buffer.size(); i+=64)
		{
			if(tmp[i] != '\n')
			{
				tmp.insert(i, "\n");
			}
			i++;
		}
		tmp.insert(0, string(RSA_PUBKEY_CERT_BEGIN)+"\n");
		tmp.append("\n"+string(RSA_PUBKEY_CERT_END)+"\n");
	}

	c_pub_key = const_cast<char *>(tmp.c_str());
	int len = tmp.size();
	if ((bio = BIO_new_mem_buf(c_pub_key, len)) == NULL)
	{
		setErrorInfo("BIO_new_mem_buf");
		return NULL;
	}

	x509 = PEM_read_bio_X509(bio, NULL, NULL, NULL);//
	BIO_free_all(bio);
	if (x509 == NULL)
	{
		setErrorInfo("PEM_read_bio_X509");
		return NULL;
	}

	return x509;
}

X509* RsaCrypto::GetX509FromFile(const string &file)
{
	string error_info;
	FILE *fp;
	X509 *x509 = NULL;

	fp = fopen(file.c_str(), "r");
	if (fp == NULL)
	{
		m_error_info = string("file is null:") + file;
		return NULL;
	}

	x509 = PEM_read_X509(fp, NULL, NULL, NULL);//
	fclose(fp);
	if (x509 == NULL)
	{
		setErrorInfo("PEM_read_X509");
		return NULL;
	}

	return x509;
}

/////////////////////////获取X509_REQ////////////////////////////////
X509_REQ* RsaCrypto::GetX509ReqFromBuffer(const string &buffer)
{
	unsigned int i;
	char *c_pub_key = NULL;
	string tmp;
	string error_info;
	BIO *bio = NULL;
	X509_REQ *req = NULL;//CSR对象

	tmp = buffer;      //strPublicKey为base64编码的公钥字符串
	if (buffer.find(RSA_REQ_CERT_BEGIN) == string::npos)
	{
		for(i = 64; i < buffer.size(); i+=64)
		{
			if(tmp[i] != '\n')
			{
				tmp.insert(i, "\n");
			}
			i++;
		}
		tmp.insert(0, string(RSA_REQ_CERT_BEGIN)+"\n");
		tmp.append("\n"+string(RSA_REQ_CERT_END)+"\n");
	}

	c_pub_key = const_cast<char *>(tmp.c_str());
	int len = tmp.size();
	if ((bio = BIO_new_mem_buf(c_pub_key, len)) == NULL)       //从字符串读取RSA公钥
	{
		setErrorInfo("BIO_new_mem_buf");
		return NULL;
	}

	req = PEM_read_bio_X509_REQ(bio, NULL, NULL, NULL);//
	if (req == NULL)
	{
		setErrorInfo("PEM_read_bio_X509_REQ");
		BIO_free_all(bio);
		return NULL;
	}

	BIO_free_all(bio);

	return req;
}

X509_REQ* RsaCrypto::GetX509ReqFromFile(const string &file)
{
	string error_info;
	FILE *fp;
	X509_REQ *req = NULL;//CSR对象

	fp = fopen(file.c_str(), "r");
	if (fp == NULL)
	{
		m_error_info = string("file is null:") + file;
		return NULL;
	}

	req = PEM_read_X509_REQ(fp, NULL, NULL, NULL);//
	fclose(fp);
	if (req == NULL)
	{
		setErrorInfo("PEM_read_X509_REQ");
		return NULL;
	}

	return req;
}

//////////////////////////公钥基本运算(验签、加密、解密)/////////////////////////////////////
int RsaCrypto::PubKeyVerify(RSA *rsa_pub_key, const string &data, const string &sign, int hash_type)
{
	int ret = 0;
	unsigned char hash[SHA_DIGEST_LENGTH] = {0};
	string error_info;

	switch(hash_type)
	{
		case HT_SHA1:
			SHA1((const unsigned char *)data.c_str(), data.size(), hash);
			//printHex("hash=", string((char *)hash, SHA_DIGEST_LENGTH));
			//printHex("sign=", sign);
			ret = RSA_verify(NID_sha1, hash, SHA_DIGEST_LENGTH, (const unsigned char *)sign.c_str(), sign.size(), rsa_pub_key);//成功返回1
			break;
		default:
			error_info = "hash algorithm is not supported";
			return -1;
	}
	if (ret <= 0)
	{
		setErrorInfo("RSA_verify");
		return -1;
	}

	return 0;
}

int RsaCrypto::PubKeyDecrypt(RSA *rsa_pub_key, const string &src, string &dest, int padding_type)
{
	int ret = 0;
	int rsa_padding_type;
	int block_len;
	unsigned char *every_buf;
	string error_info;
	vector<string> vec;

	block_len = RSA_size(rsa_pub_key); //RSA_size返回的是字节数
	every_buf = new unsigned char[block_len];
	switch(padding_type)
	{
		case PT_PKCS1:
			rsa_padding_type = RSA_PKCS1_PADDING;
			break;
		case PT_PKCS1_OAEP:
			rsa_padding_type = RSA_PKCS1_OAEP_PADDING;
			break;
		default:
			rsa_padding_type = RSA_NO_PADDING;
			break;
	}

	fragmentData(src, vec, block_len);
	dest = "";
	for (int i=0; i<(int)vec.size(); i++)
    {
        //成功返回解密后密文的长度
        ret = RSA_public_decrypt(vec[i].size(), (const unsigned char *)vec[i].c_str(), every_buf,
								rsa_pub_key, rsa_padding_type);
        if (ret <= 0)
        {
			delete[] every_buf;
			setErrorInfo("RSA_public_decrypt");
            return -1;
        }
        dest  = dest + string((char *)every_buf, ret);
    }
	delete[] every_buf;
	return 0;
}

int RsaCrypto::PubKeyEncrypt(RSA *rsa_pub_key, const string &src, string &dest, int padding_type)
{
	int ret = 0;
	int rsa_padding_type;
	int block_len;
	unsigned char *every_buf;
	string error_info;
	vector<string> vec;

	block_len = RSA_size(rsa_pub_key); //RSA_size返回的是字节数
	every_buf = new unsigned char[block_len];
	switch(padding_type)
	{
		case PT_PKCS1:
			rsa_padding_type = RSA_PKCS1_PADDING;
			block_len = block_len - 11;
			break;
		case PT_PKCS1_OAEP:
			rsa_padding_type = RSA_PKCS1_OAEP_PADDING;
			block_len = block_len - 41;
			break;
		default:
			rsa_padding_type = RSA_NO_PADDING;
			break;
	}

	fragmentData(src, vec, block_len);
	dest = "";
	for (int i=0; i<(int)vec.size(); i++)
    {
        //成功返回加密后密文的长度
        ret = RSA_public_encrypt(vec[i].size(), (const unsigned char *)vec[i].c_str(), every_buf,
								rsa_pub_key, rsa_padding_type);
        if (ret <= 0)
        {
			delete[] every_buf;
			setErrorInfo("RSA_public_encrypt");
            return -1;
        }
        dest  = dest + string((char *)every_buf, ret);
    }
	//printHex("pubKeyEncrypt=", dest);
	delete[] every_buf;
	return 0;
}

////////////////////////////////公钥加密////////////////////////////////////////////
int RsaCrypto::PubKeyEncryptByBuffer(const string &buffer, const string &src, string &dest, int padding_type)
{
	int ret;
	RSA *rsa_pub_key;

	rsa_pub_key = GetPubKeyFromBuffer(buffer);
	if (rsa_pub_key == NULL)
	{
		return -1;
	}

	ret = PubKeyEncrypt(rsa_pub_key, src, dest, padding_type);
	RSA_free(rsa_pub_key);

	return ret;
}

int RsaCrypto::PubKeyEncryptByBuffer_X509(const string &buffer, const string &src, string &dest, int padding_type)
{
	int ret;
	RSA *rsa_pub_key;

	rsa_pub_key = GetPubKeyFromBuffer_X509(buffer);
	if (rsa_pub_key == NULL)
	{
		return -1;
	}

	ret = PubKeyEncrypt(rsa_pub_key, src, dest, padding_type);
	RSA_free(rsa_pub_key);

	return ret;
}

int RsaCrypto::PubKeyEncryptByFile(const string &file, const string &src, string &dest, int padding_type)
{
	int ret;
	RSA *rsa_pub_key;

	rsa_pub_key = GetPubKeyFromFile(file);
	if (rsa_pub_key == NULL)
	{
		return -1;
	}

	ret = PubKeyEncrypt(rsa_pub_key, src, dest, padding_type);
	RSA_free(rsa_pub_key);

	return ret;
}

int RsaCrypto::PubKeyEncryptByFile_X509(const string &file, const string &src, string &dest, int padding_type)
{
	int ret;
	RSA *rsa_pub_key;

	rsa_pub_key = GetPubKeyFromFile_X509(file);
	if (rsa_pub_key == NULL)
	{
		return -1;
	}

	ret = PubKeyEncrypt(rsa_pub_key, src, dest, padding_type);
	RSA_free(rsa_pub_key);

	return ret;
}

//////////////////////////////////公钥验签////////////////////////////////////////////////
int RsaCrypto::PubKeyVerifyByBuffer(const string &buffer, const string &data, const string &sign, int hash_type)
{
	int ret;
	RSA *rsa_pub_key;

	rsa_pub_key = GetPubKeyFromBuffer(buffer);
	if (rsa_pub_key == NULL)
	{
		return -1;
	}

	ret = PubKeyVerify(rsa_pub_key, data, sign, hash_type);
	RSA_free(rsa_pub_key);

	return ret;
}

int RsaCrypto::PubKeyVerifyByBuffer_X509(const string &buffer, const string &data, const string &sign, int hash_type)
{
	int ret;
	RSA *rsa_pub_key;

	rsa_pub_key = GetPubKeyFromBuffer_X509(buffer);
	if (rsa_pub_key == NULL)
	{
		return -1;
	}

	ret = PubKeyVerify(rsa_pub_key, data, sign, hash_type);
	RSA_free(rsa_pub_key);

	return ret;
}

int RsaCrypto::PubKeyVerifyByFile(const string &file, const string &data, const string &sign, int hash_type)
{
	int ret;
	RSA *rsa_pub_key;

	rsa_pub_key = GetPubKeyFromFile(file);
	if (rsa_pub_key == NULL)
	{
		return -1;
	}

	ret = PubKeyVerify(rsa_pub_key, data, sign, hash_type);
	RSA_free(rsa_pub_key);

	return ret;
}

int RsaCrypto::PubKeyVerifyByFile_X509(const string &file, const string &data, const string &sign, int hash_type)
{
	int ret;
	RSA *rsa_pub_key;

	rsa_pub_key = GetPubKeyFromFile_X509(file);
	if (rsa_pub_key == NULL)
	{
		return -1;
	}

	ret = PubKeyVerify(rsa_pub_key, data, sign, hash_type);
	RSA_free(rsa_pub_key);

	return ret;
}

/////////////////////////////公钥解密/////////////////////////////////////
int RsaCrypto::PubKeyDecryptByBuffer(const string &buffer, const string &src, string &dest, int padding_type)
{
	int ret;
	RSA *rsa_pub_key;

	rsa_pub_key = GetPubKeyFromBuffer(buffer);
	if (rsa_pub_key == NULL)
	{
		return -1;
	}

	ret = PubKeyDecrypt(rsa_pub_key, src, dest, padding_type);
	RSA_free(rsa_pub_key);

	return ret;
}

int RsaCrypto::PubKeyDecryptByBuffer_X509(const string &buffer, const string &src, string &dest, int padding_type)
{
	int ret;
	RSA *rsa_pub_key;

	rsa_pub_key = GetPubKeyFromBuffer_X509(buffer);
	if (rsa_pub_key == NULL)
	{
		return -1;
	}

	ret = PubKeyDecrypt(rsa_pub_key, src, dest, padding_type);
	RSA_free(rsa_pub_key);

	return ret;
}

int RsaCrypto::PubKeyDecryptByFile(const string &file, const string &src, string &dest, int padding_type)
{
	int ret;
	RSA *rsa_pub_key;

	rsa_pub_key = GetPubKeyFromFile(file);
	if (rsa_pub_key == NULL)
	{
		return -1;
	}

	ret = PubKeyDecrypt(rsa_pub_key, src, dest, padding_type);
	RSA_free(rsa_pub_key);

	return ret;
}

int RsaCrypto::PubKeyDecryptByFile_X509(const string &file, const string &src, string &dest, int padding_type)
{
	int ret;
	RSA *rsa_pub_key;

	rsa_pub_key = GetPubKeyFromFile_X509(file);
	if (rsa_pub_key == NULL)
	{
		return -1;
	}

	ret = PubKeyDecrypt(rsa_pub_key, src, dest, padding_type);
	RSA_free(rsa_pub_key);

	return ret;
}

/////////////////////私钥基本运算(签名、解密、加密)///////////////////////
int RsaCrypto::PriKeySign(RSA *rsa_pri_key, const string &data, string &sign, int hash_type)
{
	int ret = 0;
	unsigned char buf[RSA_DATA_ELN] = {0}, hash[SHA_DIGEST_LENGTH] = {0};
	unsigned int  len = sizeof(buf);
	string error_info;

	switch(hash_type)
	{
		case HT_SHA1:
			SHA1((const unsigned char *)data.c_str(), data.size(), hash);
			ret = RSA_sign(NID_sha1, hash, SHA_DIGEST_LENGTH, buf, &len, rsa_pri_key); //成功返回1
			break;
		default:
			error_info = "hash algorithm is not supported";
			return -1;
	}

    if (ret <= 0)
    {
		setErrorInfo("RSA_sign");
		return -1;
    }
	sign = string((char *)buf, len);
	//printHex("PriKeySign=", sign);
	return 0;
}

int RsaCrypto::PriKeyDecrypt(RSA *rsa_pri_key, const string &src, string &dest, int padding_type)
{
	int ret = 0;
	int rsa_padding_type;
	int block_len;
	unsigned char *every_buf;
	string error_info;
	vector<string> vec;

	block_len = RSA_size(rsa_pri_key);
	every_buf = new unsigned char[block_len];
	switch(padding_type)
	{
		case PT_PKCS1:
			rsa_padding_type = RSA_PKCS1_PADDING;
			break;
		case PT_PKCS1_OAEP:
			rsa_padding_type = RSA_PKCS1_OAEP_PADDING;
			break;
		default:
			rsa_padding_type = RSA_NO_PADDING;
			break;
	}

    fragmentData(src, vec, block_len);
	dest = "";
	for (int i=0; i<(int)vec.size(); i++)
    {
        //成功返回解密后明文的长度
        ret = RSA_private_decrypt(vec[i].size(), (const unsigned char*)vec[i].c_str(), every_buf,
						rsa_pri_key, rsa_padding_type);
        if (ret <= 0)
        {
			delete[] every_buf;
			setErrorInfo("RSA_private_decrypt");
            return -1;
        }
        dest  = dest + string((char *)every_buf, ret);
    }
	//printHex("PriKeyDecrypt=", dest);
	delete[] every_buf;
	return 0;
}

int RsaCrypto::PriKeyEncrypt(RSA *rsa_pri_key, const string &src, string &dest, int padding_type)
{
	int ret = 0;
	int rsa_padding_type;
	int block_len;
	unsigned char *every_buf;
	string error_info;
	vector<string> vec;

	block_len = RSA_size(rsa_pri_key); //RSA_size返回的是字节数
	every_buf = new unsigned char[block_len];
	switch(padding_type)
	{
		case PT_PKCS1:
			rsa_padding_type = RSA_PKCS1_PADDING;
			block_len = block_len - 11;
			break;
		case PT_PKCS1_OAEP:
			rsa_padding_type = RSA_PKCS1_OAEP_PADDING;
			block_len = block_len - 41;
			break;
		default:
			rsa_padding_type = RSA_NO_PADDING;
			break;
	}

	fragmentData(src, vec, block_len);
	dest = "";
	for (int i=0; i<(int)vec.size(); i++)
    {
        //成功返回加密后密文的长度
        ret = RSA_private_encrypt(vec[i].size(), (const unsigned char *)vec[i].c_str(), every_buf,
								rsa_pri_key, rsa_padding_type);
        if (ret <= 0)
        {
			delete[] every_buf;
			setErrorInfo("RSA_private_encrypt");
            return -1;
        }
        dest  = dest + string((char *)every_buf, ret);
    }

	delete[] every_buf;
	return 0;
}

//////////////////////////////私钥签名/////////////////////////////
int RsaCrypto::PriKeySignByBuffer(const string &buffer, const string &data, string &sign, int hash_type)
{
	int ret = 0;
	RSA *rsa_pri_key;

	rsa_pri_key = GetPriKeyFromBuffer(buffer);
	if (rsa_pri_key == NULL)
	{
		return -1;
	}

	ret = PriKeySign(rsa_pri_key, data, sign, hash_type);

	RSA_free(rsa_pri_key);
	return 0;
}

int RsaCrypto::PriKeySignByFile(const string &file, const string &data, string &sign, int hash_type)
{
	int ret = 0;
	RSA *rsa_pri_key;

	rsa_pri_key = GetPriKeyFromFile(file);
	if (rsa_pri_key == NULL)
	{
		return -1;
	}

	ret = PriKeySign(rsa_pri_key, data, sign, hash_type);

	RSA_free(rsa_pri_key);
	return 0;
}

//////////////////////////私钥解密////////////////////////
int RsaCrypto::PriKeyDecryptByBuffer(const string &buffer, const string &src, string &dest, int padding_type)
{
	int ret = 0;
	RSA *rsa_pri_key;

	rsa_pri_key = GetPriKeyFromBuffer(buffer);
	if (rsa_pri_key == NULL)
	{
		return -1;
	}
	ret = PriKeyDecrypt(rsa_pri_key, src, dest, padding_type);

	RSA_free(rsa_pri_key);
	return ret;
}

int RsaCrypto::PriKeyDecryptByFile(const string &file, const string &src, string &dest, int padding_type)
{
	int ret = 0;
	RSA *rsa_pri_key;

	rsa_pri_key = GetPriKeyFromFile(file);
	if (rsa_pri_key == NULL)
	{
		return -1;
	}
	ret = PriKeyDecrypt(rsa_pri_key, src, dest, padding_type);

	RSA_free(rsa_pri_key);
	return ret;
}

//////////////////////////私钥加密////////////////////////
int RsaCrypto::PriKeyEncryptByBuffer(const string &buffer, const string &src, string &dest, int padding_type)
{
	int ret = 0;
	RSA *rsa_pri_key;

	rsa_pri_key = GetPriKeyFromBuffer(buffer);
	if (rsa_pri_key == NULL)
	{
		return -1;
	}
	ret = PriKeyEncrypt(rsa_pri_key, src, dest, padding_type);

	RSA_free(rsa_pri_key);
	return ret;
}

int RsaCrypto::PriKeyEncryptByFile(const string &file, const string &src, string &dest, int padding_type)
{
	int ret = 0;
	RSA *rsa_pri_key;

	rsa_pri_key = GetPriKeyFromFile(file);
	if (rsa_pri_key == NULL)
	{
		return -1;
	}
	ret = PriKeyEncrypt(rsa_pri_key, src, dest, padding_type);

	RSA_free(rsa_pri_key);
	return ret;
}

////////////////////////获取证书ID///////////////////////////////////////////
string RsaCrypto::GetCertIDFromBuffer(const string &buffer, int type)
{
	string ret;
	X509 *x509 = NULL;

	x509 = GetX509FromBuffer(buffer);
	if (x509 == NULL)
	{
		return "";
	}

	ret = GetCertIDFromX509(x509, type);
	X509_free(x509);

	return ret;

}

string RsaCrypto::GetCertIDFromFile(const string &file, int type)
{
	string ret;
	X509 *x509 = NULL;

	x509 = GetX509FromFile(file);
	if (x509 == NULL)
	{
		return "";
	}

	ret = GetCertIDFromX509(x509, type);
	X509_free(x509);

	return ret;

}

string RsaCrypto::GetCertIDFromX509(X509 *x509, int type)
{
	string ret;
	string error_info;
	ASN1_INTEGER *asn1_i = NULL;
	BIGNUM *bignum = NULL;
	char *serial = NULL;

	asn1_i = X509_get_serialNumber(x509);
	bignum = ASN1_INTEGER_to_BN(asn1_i, NULL);
	if (bignum == NULL)
	{
		setErrorInfo("ASN1_INTEGER_to_BN");
		return "";
	}
	if (type == 0)
	{
		serial = BN_bn2dec(bignum);
	}
	else
	{
		serial = BN_bn2hex(bignum);
	}
	if (serial == NULL)
	{
		setErrorInfo("BN_bn2dec or BN_bn2hex");
		//ASN1_INTEGER_free(asn1_i); //貌似只能BN_free，不能ASN1_INTEGER_free
		BN_free(bignum);
		return "";
	}
	ret = string(serial);

	OPENSSL_free(serial);
	//ASN1_INTEGER_free(asn1_i); //貌似只能BN_free，不能ASN1_INTEGER_free
	BN_free(bignum);

	return ret;
}

//////////////////////////////签发证书//////////////////////////
int RsaCrypto::IssuePubKeyCert(const string &ca_prikey, const string &ca_pubkey_x509, const string &req_cert, int expire_seconds, CertInfo &cert_info)
{
	int ret = 0;
	int name_len = 128;
	int pubkey_cert_len = 8192;
	string::size_type pos=0;
	char c_pubkey_cert[8192+1] = {0};
	char c_name[128+1] = {0};
	X509_REQ *req = NULL;
	EVP_PKEY *user_pub_pkey = NULL;
	EVP_PKEY *ca_pri_pkey = NULL;
	EVP_PKEY *ca_pub_pkey = NULL;
	X509 *ca_pub_x509 = NULL;
	X509 *cert = NULL;
	X509_NAME *name = NULL;
	X509V3_CTX ctx;
	ASN1_INTEGER *asn1_i = NULL;
	BIGNUM *bignum = NULL;
	BIO *bio = NULL;

	OpenSSL_add_all_algorithms();

	ca_pri_pkey = GetPriEvpKeyFromBuffer(ca_prikey);
	if (ca_pri_pkey == NULL)
	{
		ret = -1;
		goto EXIT;
	}
	ca_pub_x509 = GetX509FromBuffer(ca_pubkey_x509);
	if (ca_pub_x509 == NULL)
	{
		ret = -1;
		goto EXIT;
	}
	ca_pub_pkey = X509_get_pubkey(ca_pub_x509);
	if (ca_pub_pkey == NULL)
	{
		setErrorInfo("X509_get_pubkey");
		ret = -1;
		goto EXIT;
	}
	if(X509_check_private_key(ca_pub_x509, ca_pri_pkey) != 1)
    {
        setErrorInfo("X509_check_private_key");
		ret = -1;
		goto EXIT;
    }
	//get issuer and common_name
	name = X509_get_issuer_name(ca_pub_x509);
	if(!name)
    {
        setErrorInfo("X509_get_issuer_name");
		ret = -1;
		goto EXIT;
    }
	name_len = X509_NAME_get_text_by_NID(name, NID_commonName, c_name, name_len);
	if(-1 == name_len)
    {
        setErrorInfo("X509_NAME_get_text_by_NID(NID_commonName)");
		ret = -1;
		goto EXIT;
    }
	cert_info.common_name = string(c_name, name_len);
	cert_info.issuer = cert_info.common_name;

	req = GetX509ReqFromBuffer(req_cert);
	if (req == NULL)
	{
		ret = -1;
		goto EXIT;
	}

	user_pub_pkey = X509_REQ_get_pubkey(req);
	if (user_pub_pkey == NULL)
	{
		setErrorInfo("X509_REQ_get_pubkey");
		ret = -1;
		goto EXIT;
	}

	if (X509_REQ_verify(req, user_pub_pkey) != 1)  //验证请求证书中的签名时出错
	{
		setErrorInfo("X509_REQ_verify");
		ret = -1;
		goto EXIT;
	}

	name = X509_REQ_get_subject_name(req);
	if (!name)
	{
		setErrorInfo("X509_REQ_get_subject_name");
		ret = -1;
		goto EXIT;
	}
	//get email  NID_pkcs9_emailAddress
	name_len = 128;
	memset(c_name, 0, sizeof(c_name));
	name_len = X509_NAME_get_text_by_NID(name, NID_pkcs9_emailAddress, c_name, name_len);
	if(name_len > 0)
    {
        cert_info.email = string(c_name, name_len);
    }

	//get unit  NID_organizationalUnitName
	name_len = 128;
	memset(c_name, 0, sizeof(c_name));
	name_len = X509_NAME_get_text_by_NID(name, NID_organizationalUnitName, c_name, name_len);
	if(name_len > 0)
    {
        cert_info.org_unit = string(c_name, name_len);
    }

	//get org   NID_organizationName
	name_len = 128;
	memset(c_name, 0, sizeof(c_name));
	name_len = X509_NAME_get_text_by_NID(name, NID_organizationName, c_name, name_len);
	if(name_len > 0)
    {
        cert_info.organization = string(c_name, name_len);
    }
	//cout << "email=" << cert_info.email << ", org_unit=" << cert_info.org_unit << ", organization=" << cert_info.organization << endl;
	if((cert = X509_new()) == NULL)
	{
		setErrorInfo("X509_new");
		ret = -1;
		goto EXIT;
	}

	if (X509_set_version(cert, 2L) != 1)  //设置证书版本时出错
	{
		setErrorInfo("X509_set_version");
		ret = -1;
		goto EXIT;
	}

	BN_hex2bn(&bignum, cert_info.cert_no.c_str());
	asn1_i = BN_to_ASN1_INTEGER(bignum, asn1_i);
	if (asn1_i == NULL)
	{
		setErrorInfo("BN_to_ASN1_INTEGER");
		ret = -1;
		goto EXIT;
	}
	if (X509_set_serialNumber(cert, asn1_i) != 1)
	{
		setErrorInfo("X509_set_serialNumber");
		ret = -1;
		goto EXIT;
	}


	if (X509_set_subject_name(cert, name) != 1)
	{
		setErrorInfo("X509_set_subject_name");
		ret = -1;
		goto EXIT;
	}

	if (!(name = X509_get_subject_name(ca_pub_x509)))   //从CA证书中获取主题名时出错
	{
		setErrorInfo("X509_get_subject_name");
		ret = -1;
		goto EXIT;
	}

	if (X509_set_issuer_name(cert, name) != 1)  //设置证书发行名时出错
	{
		setErrorInfo("X509_set_issuer_name");
		ret = -1;
		goto EXIT;
	}

    if (X509_set_pubkey(cert, user_pub_pkey) != 1)  //设置证书的公钥时出错
	{
		setErrorInfo("X509_set_pubkey");
		ret = -1;
		goto EXIT;
	}

	if (!(X509_gmtime_adj(X509_get_notBefore(cert), 0)))  //设置证书的启用时间时出错
	{
		setErrorInfo("X509_gmtime_adj(X509_get_notBefore)");
		ret = -1;
		goto EXIT;
	}

	if (!(X509_gmtime_adj(X509_get_notAfter(cert), expire_seconds)))  //设置证书的结束时间时出错
	{
		setErrorInfo("X509_gmtime_adj(X509_get_notAfter)");
		ret = -1;
		goto EXIT;
	}

	//添加x509 v3扩展
    X509V3_set_ctx(&ctx, ca_pub_x509, cert, NULL, NULL, 0);
	//扩展属性 basicConstraints、subjectKeyIdentifier、authorityKeyIdentifier、keyUsage、nsCertType
	for(int i=0; i<X509_EXT_COUNT; i++)
	{
		X509_EXTENSION *ext;
		char ext_key[64+1] = {0};
		char ext_value[256+1] = {0};

		memcpy(ext_key, m_ext_key[i].c_str(), m_ext_key[i].size());
		memcpy(ext_value, m_ext_value[i].c_str(), m_ext_value[i].size());
		ext = X509V3_EXT_conf(NULL, &ctx, ext_key, ext_value);
		if (!ext) //创建X509扩展对象时出错
		{
			setErrorInfo(string("X509V3_EXT_conf(")+ext_key+")");
			ret = -1;
			goto EXIT;
		}
		if (!X509_add_ext(cert, ext, -1)) //增加 X509 extension到证书中时出错
		{
			setErrorInfo(string("X509_add_ext(")+ext_key+")");
			ret = -1;
			goto EXIT;
		}
		X509_EXTENSION_free(ext);
	}
	//X509_sign
    if(!X509_sign(cert, ca_pri_pkey, EVP_sha1()))
	{
		setErrorInfo("X509_sign");
		ret = -1;
		goto EXIT;
	}

	//cert 转 string
	// FILE *fp;
    // if (!(fp = fopen("k100_0.crt", "w")))
	// {
		// ret = -1;
		// goto EXIT;
	// }
    //以PEM格式写完整的签名证书到文件
    // if (PEM_write_X509(fp, cert) != 1)
	// {
		// setErrorInfo();
		// m_error_info = "PEM_write_X509 failed:" + m_error_info;
		// fclose(fp);
		// ret = -1;
		// goto EXIT;
	// }
    // fclose(fp);

	bio = BIO_new(BIO_s_mem());
    if (PEM_write_bio_X509(bio, cert) != 1)
	{
		setErrorInfo("PEM_write_bio_X509");
		ret = -1;
		goto EXIT;
	}
	if ( (pubkey_cert_len=BIO_read(bio, c_pubkey_cert, pubkey_cert_len)) <= 0)
	{
		setErrorInfo("BIO_read");
		ret = -1;
		goto EXIT;
	}
	cert_info.crt = string(c_pubkey_cert, pubkey_cert_len);
	//cout << "cert_info.crt:" << endl;
	//cout << cert_info.crt << endl;
EXIT:
	if (bignum != NULL)
		BN_free(bignum);
	if (asn1_i != NULL)
		ASN1_INTEGER_free(asn1_i);
	if (user_pub_pkey != NULL)
		EVP_PKEY_free(user_pub_pkey);
	if (req != NULL)
		X509_REQ_free(req);
	if (ca_pub_pkey != NULL)
		EVP_PKEY_free(ca_pub_pkey);
	if (ca_pri_pkey != NULL)
		EVP_PKEY_free(ca_pri_pkey);
	if (ca_pub_x509 != NULL)
		X509_free(ca_pub_x509);
	if (cert != NULL)
		X509_free(cert);
	if (bio != NULL)
		BIO_free_all(bio);

	return ret;
}
