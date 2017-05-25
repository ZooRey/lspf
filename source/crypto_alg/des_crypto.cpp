#include "des_crypto.h"
#include "common/base64.h"
#include "common/string_utility.h"
#include "log/log.h"
#include <openssl/des.h>
#include <openssl/err.h>
#include <vector>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>

using namespace std;

#define BOLOCK_LEN 8

void DesCrypto::printHex(const string &title, const string &src)
{
	string hex;

	StringUtility::bcd2asc(src, hex);
	cout << title << hex << endl;
}

int DesCrypto::Encrypt(int crypto_mode, int padding_type, const string &key, const string &src, const string &ivec, string &dest)
{
	int remained;
	string src_padding = src;
	string pt_80 = string("\x80\x00\x00\x00\x00\x00\x00\x00", 8);

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
	switch(key.size())
	{
		case 8:
			if (crypto_mode == CM_ECB)
			{
				crypt_ecb_1(DES_ENCRYPT, key, src_padding, dest);
			}
			else
			{
				crypt_cbc_1(DES_ENCRYPT, key, src_padding, ivec, dest);
			}
			break;
		case 16:
		case 24:
			if (crypto_mode == CM_ECB)
			{
				crypt_ecb_3(DES_ENCRYPT, key, src_padding, dest);
			}
			else
			{
				crypt_cbc_3(DES_ENCRYPT, key, src_padding, ivec, dest);
			}
			break;
		default:
			return -1;
	}

	return 0;
}

int DesCrypto::Decrypt(int crypto_mode, const string &key, const string &src, const string &ivec, string &dest)
{
	switch(key.size())
	{
		case 8:
			if (crypto_mode == CM_ECB)
			{
				crypt_ecb_1(DES_DECRYPT, key, src, dest);
			}
			else
			{
				crypt_cbc_1(DES_DECRYPT, key, src, ivec, dest);
			}
			break;
		case 16:
		case 24:
			if (crypto_mode == CM_ECB)
			{
				crypt_ecb_3(DES_DECRYPT, key, src, dest);
			}
			else
			{
				crypt_cbc_3(DES_DECRYPT, key, src, ivec, dest);
			}
			break;
		default:
			return -1;
	}

	return 0;
}

int DesCrypto::CalcMAC(int mac_mode, const string &key, const string &mac_data, const string &ivec, string &mac_value)
{
	if ( (key.size() !=8) && (key.size() !=16) && (key.size() !=24) )
	{
		return -1;
	}

	int remained;
	string mac_data_padding = mac_data;
	string pt_80 = string(1, 0x80) + string(BOLOCK_LEN-1, 0x00);

	remained = mac_data_padding.size() % BOLOCK_LEN;
	//printHex("mac_data_padding=", mac_data_padding);
	switch(mac_mode)
	{
		case MM_X99:
			if (remained > 0)
			{
				mac_data_padding = mac_data_padding + string(BOLOCK_LEN-remained, 0x00);
			}
			calc_mac_x99(key, mac_data_padding, ivec, mac_value);
			break;
		case MM_X919:
			if (remained > 0)
			{
				mac_data_padding = mac_data_padding + string(BOLOCK_LEN-remained, 0x00);
			}
			calc_mac_x919(key, mac_data_padding, ivec, mac_value);
			break;
		case MM_XOR:
			if (remained > 0)
			{
				mac_data_padding = mac_data_padding + string(BOLOCK_LEN-remained, 0x00);
			}
			calc_mac_xor(key, mac_data_padding, ivec, mac_value);
			break;
		case MM_CUP:
			if (remained > 0)
			{
				mac_data_padding = mac_data_padding + string(BOLOCK_LEN-remained, 0x00);
			}
			calc_mac_cup(key, mac_data_padding, ivec, mac_value);
			break;
		case MM_PBOC:
			mac_data_padding = mac_data_padding + pt_80.substr(0, BOLOCK_LEN-remained);
			calc_mac_x919(key, mac_data_padding, ivec, mac_value);
			break;
		default:
			return -2;
	}

	return 0;
}

int DesCrypto::ConvertPin(int left_pin_format, int right_pin_format, const string &acc_no, const string &left_pin_cipher, const string &left_pik, const string &right_pik, string &right_pin_cipher)
{
	if ( (left_pik.size() !=8) && (left_pik.size() !=16) && (left_pik.size() !=24) )
	{
		return -1;
	}
	if ( (right_pik.size() !=8) && (right_pik.size() !=16) && (right_pik.size() !=24) )
	{
		return -1;
	}

	string left_pin;
	string pan, pan_bcd;

	if (left_pik.size() ==8)
	{
		crypt_ecb_1(DES_DECRYPT, left_pik, left_pin_cipher, left_pin);
	}
	else
	{
		crypt_ecb_3(DES_DECRYPT, left_pik, left_pin_cipher, left_pin);
	}

	//左端带主账号异或并且右端不带  或者   右端带主账号异或并且左端不带
	if ( (left_pin_format == PF_WITH_ACCNO && right_pin_format == PF_WITHOUT_ACCNO)
		|| (left_pin_format == PF_WITHOUT_ACCNO && right_pin_format == PF_WITH_ACCNO) )
	{
		if (acc_no.size() > 12)
		{
			pan = acc_no.substr(acc_no.size()-13, 12);
		}
		else
		{
			pan = acc_no.substr(0, acc_no.size()-1);
		}
		pan.insert(0, 16-pan.size(), '0');
		StringUtility::asc2bcd_l(pan, pan_bcd);
		StringUtility::Xor(pan_bcd, left_pin);
	}

	if (right_pik.size() ==8)
	{
		crypt_ecb_1(DES_ENCRYPT, right_pik, left_pin, right_pin_cipher);
	}
	else
	{
		crypt_ecb_3(DES_ENCRYPT, right_pik, left_pin, right_pin_cipher);
	}

	return 0;
}

int DesCrypto::crypt_ecb_1(int enc_dec, const string &key, const string &src, string &dest)
{
	const char* p = src.c_str();
	unsigned char block_key[BOLOCK_LEN];
	unsigned char block_src[BOLOCK_LEN];
	unsigned char block_dest[BOLOCK_LEN];
	DES_key_schedule ks;

	memcpy(block_key, key.c_str(), BOLOCK_LEN);
	DES_set_key_unchecked((const_DES_cblock*)block_key, &ks);

	dest = "";
	for (unsigned int i = 0; i < src.size()/BOLOCK_LEN; i++)
	{
		memcpy(block_src, p+i*BOLOCK_LEN, BOLOCK_LEN);
		DES_ecb_encrypt((const_DES_cblock*)block_src, (DES_cblock*)block_dest, &ks, enc_dec);
		dest += string((char *)block_dest, BOLOCK_LEN);
	}
	//printHex("crypt_ecb_1=", dest);
	return 0;
}

int DesCrypto::crypt_ecb_3(int enc_dec, const string &key, const string &src, string &dest)
{
	const char* p = src.c_str();
	const char* k = key.c_str();
	unsigned char block_key[BOLOCK_LEN];
	unsigned char block_src[BOLOCK_LEN];
	unsigned char block_dest[BOLOCK_LEN];
	DES_key_schedule ks1, ks2, ks3;

	memcpy(block_key, k, BOLOCK_LEN);
	DES_set_key_unchecked((const_DES_cblock*)block_key, &ks1);
	memcpy(block_key, k+BOLOCK_LEN, BOLOCK_LEN);
	DES_set_key_unchecked((const_DES_cblock*)block_key, &ks2);
	if (key.size() == BOLOCK_LEN*3)
	{
		memcpy(block_key, k+BOLOCK_LEN*2, BOLOCK_LEN);
	}
	else
	{
		memcpy(block_key, k, BOLOCK_LEN);
	}
	DES_set_key_unchecked((const_DES_cblock*)block_key, &ks3);

	dest = "";
	for (unsigned int i = 0; i < src.size()/BOLOCK_LEN; i++)
	{
		memcpy(block_src, p+i*BOLOCK_LEN, BOLOCK_LEN);
		DES_ecb3_encrypt((const_DES_cblock*)block_src, (DES_cblock*)block_dest, &ks1, &ks2, &ks3, enc_dec);
		dest += string((char *)block_dest, BOLOCK_LEN);
	}
	//printHex("crypt_ecb_3=", dest);
	return 0;
}

int DesCrypto::crypt_cbc_1(int enc_dec, const string &key, const string &src, const string &ivec, string &dest)
{
	unsigned char block_key[BOLOCK_LEN];
	unsigned char block_ivec[BOLOCK_LEN] = {0};
	unsigned char *block_dest = new unsigned char[src.size()];
	DES_key_schedule ks;

	if (ivec.size() < BOLOCK_LEN)
	{
		memcpy(block_ivec, ivec.c_str(), ivec.size());
	}
	else
	{
		memcpy(block_ivec, ivec.c_str(), BOLOCK_LEN);
	}

	memcpy(block_key, key.c_str(), BOLOCK_LEN);
	DES_set_key_unchecked((const_DES_cblock*)block_key, &ks);

	DES_ncbc_encrypt((const unsigned char *)src.c_str(), block_dest,
               (long)src.size(), &ks, (DES_cblock *)block_ivec, enc_dec);

	dest = string((char *)block_dest, src.size());
	delete[] block_dest;

	//printHex("crypt_cbc_1=", dest);
	return 0;
}

int DesCrypto::crypt_cbc_3(int enc_dec, const string &key, const string &src, const string &ivec, string &dest)
{
	const char *k = key.c_str();
	unsigned char block_key[BOLOCK_LEN];
	unsigned char block_ivec[BOLOCK_LEN] = {0};
	unsigned char *block_dest = new unsigned char[src.size()];
	DES_key_schedule ks1, ks2, ks3;

	if (ivec.size() < BOLOCK_LEN)
	{
		memcpy(block_ivec, ivec.c_str(), ivec.size());
	}
	else
	{
		memcpy(block_ivec, ivec.c_str(), BOLOCK_LEN);
	}

	memcpy(block_key, k, BOLOCK_LEN);
	DES_set_key_unchecked((const_DES_cblock*)block_key, &ks1);
	memcpy(block_key, k+BOLOCK_LEN, BOLOCK_LEN);
	DES_set_key_unchecked((const_DES_cblock*)block_key, &ks2);
	if (key.size() == BOLOCK_LEN*3)
	{
		memcpy(block_key, k+BOLOCK_LEN*2, BOLOCK_LEN);
	}
	else
	{
		memcpy(block_key, k, BOLOCK_LEN);
	}
	DES_set_key_unchecked((const_DES_cblock*)block_key, &ks3);

	DES_ede3_cbc_encrypt((const unsigned char *)src.c_str(), block_dest,
               (long)src.size(), &ks1, &ks2, &ks3, (DES_cblock *)block_ivec, enc_dec);

	dest = string((char *)block_dest, src.size());
	delete[] block_dest;

	//printHex("crypt_cbc_3=", dest);
	return 0;
}

void DesCrypto::calc_mac_xor(const string &key, const string &mac_data, const string &ivec, string &mac_value)
{
	unsigned int offset = 0;
	string mab;
	string out;

	mab = mac_data.substr(offset, 8);
	StringUtility::Xor(ivec, mab);
	out = mab;

	offset += 8;
	while(offset < mac_data.size())
	{
		mab = mac_data.substr(offset, 8);
		StringUtility::Xor(mab, out);
		offset = offset + 8;
	}

	if (key.size() == 8)
	{
		crypt_ecb_1(DES_ENCRYPT, key, out, mac_value);
	}
	else
	{
		crypt_ecb_3(DES_ENCRYPT, key, out, mac_value);
	}
}

void DesCrypto::calc_mac_x99(const string &key, const string &mac_data, const string &ivec, string &mac_value)
{
	unsigned int offset = 0;
	string mab;
	string out;

	mab = mac_data.substr(offset, 8);
	StringUtility::Xor(ivec, mab);
	if (key.size() == 8)
	{
		crypt_ecb_1(DES_ENCRYPT, key, mab, out);
	}
	else
	{
		crypt_ecb_3(DES_ENCRYPT, key, mab, out);
	}

	offset += 8;
	while(offset < mac_data.size())
	{
		mab = mac_data.substr(offset, 8);
		StringUtility::Xor(out, mab);
		if (key.size() == 8)
		{
			crypt_ecb_1(DES_ENCRYPT, key, mab, out);
		}
		else
		{
			crypt_ecb_3(DES_ENCRYPT, key, mab, out);
		}
		offset = offset + 8;
	}

	mac_value = out;
}

void DesCrypto::calc_mac_x919(const string &key, const string &mac_data, const string &ivec, string &mac_value)
{
	unsigned int offset = 0;
	string left_key;
	string right_key;
	string mab;
	string enc_out;
	string dec_out;

	if (key.size() > 8)
	{
		left_key = key.substr(0, 8);
		right_key = key.substr(8, 8);
	}
	else
	{
		left_key = key;
		right_key = key;
	}

	mab = mac_data.substr(offset, 8);
	StringUtility::Xor(ivec, mab);
	crypt_ecb_1(DES_ENCRYPT, left_key, mab, enc_out);

	offset += 8;
	while(offset < mac_data.size())
	{
		mab = mac_data.substr(offset, 8);
		StringUtility::Xor(enc_out, mab);
		crypt_ecb_1(DES_ENCRYPT, left_key, mab, enc_out);
		offset = offset + 8;
	}

	crypt_ecb_1(DES_DECRYPT, right_key, enc_out, dec_out);
	crypt_ecb_1(DES_ENCRYPT, left_key, dec_out, enc_out);

	mac_value = enc_out;
}

void DesCrypto::calc_mac_cup(const string &key, const string &mac_data, const string &ivec, string &mac_value)
{
	unsigned int offset = 0;
	string mab;
	string out_asc;
	string out_bcd;

	mab = mac_data.substr(offset, 8);
	StringUtility::Xor(ivec, mab);
	out_bcd = mab;

	offset += 8;
	while(offset < mac_data.size())
	{
		mab = mac_data.substr(offset, 8);
		StringUtility::Xor(mab, out_bcd);
		offset = offset + 8;
	}
	StringUtility::bcd2asc(out_bcd, out_asc);

	//左半部分加密
	if (key.size() == 8)
	{
		crypt_ecb_1(DES_ENCRYPT, key, out_asc.substr(0,8), out_bcd);
	}
	else
	{
		crypt_ecb_3(DES_ENCRYPT, key, out_asc.substr(0,8), out_bcd);
	}
	//和右半部分异或
	StringUtility::Xor(out_asc.substr(8,8), out_bcd);
	//加密
	if (key.size() == 8)
	{
		crypt_ecb_1(DES_ENCRYPT, key, out_bcd, mac_value);
	}
	else
	{
		crypt_ecb_3(DES_ENCRYPT, key, out_bcd, mac_value);
	}
}
