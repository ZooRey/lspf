#include "aes_crypto.h"
#include "common/base64.h"
#include "common/string_utility.h"
#include "log/log.h"
#include <openssl/aes.h>
#include <openssl/err.h>
#include <vector>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>

using namespace std;

#define BOLOCK_LEN 16

void AesCrypto::printHex(const string &title, const string &src)
{
	string hex;

	StringUtility::bcd2asc(src, hex);
	cout << title << hex << endl;
}

int AesCrypto::Encrypt(int crypto_mode, int padding_type, const string &key, const string &src, const string &ivec, string &dest)
{
	int remained;
	string src_padding = src;
	string pt_80 = string("\x80\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16);

	remained = src_padding.size() % BOLOCK_LEN;
	if (remained > 0)
	{
		if (padding_type == PT_PKCS5)
		{
			src_padding = src_padding + string(BOLOCK_LEN-remained, (char)(BOLOCK_LEN-remained));
		}
		else if (padding_type == PT_PKCS7)
		{
			src_padding = src_padding + string(BOLOCK_LEN-remained, 0x00);
		}
		else if (padding_type == PT_80)
		{
			src_padding = src_padding + pt_80.substr(0, BOLOCK_LEN-remained);
		}
	}
	else
    {
        if (padding_type == PT_PKCS5)
		{
			src_padding = src_padding + string(BOLOCK_LEN-remained, (char)(BOLOCK_LEN-remained));
		}
    }
	//printHex("src_padding=", src_padding);

	if (crypto_mode == CM_ECB)
	{
		return crypt_ecb(AES_ENCRYPT, key, src_padding, dest);
	}
	else
	{
		return crypt_cbc(AES_ENCRYPT, key, src_padding, ivec, dest);
	}
}

int AesCrypto::Decrypt(int crypto_mode, const string &key, const string &src, const string &ivec, string &dest)
{
	if (crypto_mode == CM_ECB)
	{
		return crypt_ecb(AES_DECRYPT, key, src, dest);
	}
	else
	{
		return crypt_cbc(AES_DECRYPT, key, src, ivec, dest);
	}
}

int AesCrypto::crypt_ecb(int enc_dec, const string &key, const string &src, string &dest)
{
	int ret;
	const char* p = src.c_str();
	unsigned char block_dest[BOLOCK_LEN];
	AES_KEY ak;

	if (enc_dec == AES_ENCRYPT)
	{
		ret = AES_set_encrypt_key((const unsigned char *)key.c_str(), key.size()*8, &ak);
	}
	else
	{
		ret = AES_set_decrypt_key((const unsigned char *)key.c_str(), key.size()*8, &ak);
	}
	if (ret != 0)
	{

		return -1;
	}

	dest = "";
	for (unsigned int i = 0; i < src.size()/BOLOCK_LEN; i++)
	{
		AES_ecb_encrypt((const unsigned char *)p+i*BOLOCK_LEN, block_dest, &ak, enc_dec);
		dest += string((char *)block_dest, BOLOCK_LEN);
	}
	//printHex("crypt_ecb=", dest);
	return 0;
}

int AesCrypto::crypt_cbc(int enc_dec, const string &key, const string &src, const string &ivec, string &dest)
{
	int ret;
	unsigned char block_ivec[BOLOCK_LEN] = {0};
	unsigned char *block_dest = new unsigned char[src.size()];
	AES_KEY ak;

	if (ivec.size() < BOLOCK_LEN)
	{
		memcpy(block_ivec, ivec.c_str(), ivec.size());
	}
	else
	{
		memcpy(block_ivec, ivec.c_str(), BOLOCK_LEN);
	}

	if (enc_dec == AES_ENCRYPT)
	{
		ret = AES_set_encrypt_key((const unsigned char *)key.c_str(), key.size()*8, &ak);
	}
	else
	{
		ret = AES_set_decrypt_key((const unsigned char *)key.c_str(), key.size()*8, &ak);
	}
	if (ret != 0)
	{
		return -1;
	}

	AES_cbc_encrypt((const unsigned char *)src.c_str(), block_dest, src.size(), &ak, block_ivec, enc_dec);
	dest = string((char *)block_dest, src.size());
	delete[] block_dest;

	//printHex("crypt_cbc=", dest);
	return 0;
}
