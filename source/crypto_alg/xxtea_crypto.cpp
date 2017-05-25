#include "xxtea_crypto.h"
#include "common/base64.h"
#include "common/string_utility.h"
#include "log/log.h"
#include <vector>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>

//^ 按位异或
//优先级: + >> ^
#define MX(k) (z>>5^y<<2) + (y>>3^z<<4)^(sum^y) + (k[p&3^e]^z);
#define MX_OLD(k)  (z >> 5 ^ y << 2) + ((y >> 3 ^ z << 4) ^ (sum ^ y)) + (k[(p & 3) ^ e] ^ z)//为了兼容旧的动态库所加密的数据

//static string m_lmk = string("\x36\x38\x34\x38\x32\x35\x36\x39\x39\x36\x35\x32\x38\x34\x38\x36");

void XxteaCrypto::arrayByteToInt(const string &src, bool include_length, vector<unsigned int> &dest)
{
	//判断src的长度是否4的倍数
	unsigned int i;
	int n = (((src.size() & 3) == 0) ? (src.size() >> 2) : ((src.size() >> 2) + 1));
	
	if (include_length)
	{
		dest.resize(n+1);
		dest[n] = src.size();
	}
	else
	{
		dest.resize(n);
	}
	for (i = 0; i < src.size(); i++)
	{				
		dest[i >> 2] |= (0x000000ff & (unsigned char)src[i]) << ((i & 3) << 3);
	}	
}

void XxteaCrypto::arrayIntToByte(const vector<unsigned int> &src, bool include_length, string &dest)
{
	//dest是src的4倍长
	unsigned int n = src.size() << 2;

	if (include_length)
	{
		unsigned int m = src[src.size() - 1];
		if (m > n)
		{
			return;
		}
		else
		{
			n = m;
		}
	}
	
	unsigned char *result = new unsigned char[n];
	for (unsigned int i = 0; i < n; i++)
	{
		result[i] = (unsigned char)((src[i >> 2] >> ((i & 3) << 3)) & 0xff);
	}
	dest = string((char *)result, n);
	delete[] result;
}

void XxteaCrypto::Decrypt(const string &src, const string &key, string &dest, bool if_old_alg)
{
	string src_base64;
	vector<unsigned int> vec_src;
	vector<unsigned int> vec_key;
	
	Base64::Decode(src, &src_base64);
	
	string realKey = key;
	if(key.empty())
	{
		realKey = "__TOOLKITS_DEF__";
	}
	else if(key.size() >= 16)
	{
		realKey.resize(16);
	}
	else 
	{
		realKey.resize(16, 0x00);
	}

	arrayByteToInt(src_base64, false, vec_src);
	arrayByteToInt(realKey, false, vec_key);

	int n = vec_src.size() - 1;
	if (n < 1)
	{
		return;
	}
	if (vec_key.size() < 4)
	{
		return;
	}

	unsigned int z = vec_src[n], y = vec_src[0], delta = 0x9E3779B9, sum, e;
	int p, q = 6 + 52 / (n + 1);

	sum = q * delta;
	while (sum != 0)
	{
		e = sum >> 2 & 3;
		for (p = n; p > 0; p--)
		{
			z = vec_src[p - 1];
			if(if_old_alg)
			{
				y = vec_src[p] -= MX_OLD(vec_key); 
			}
			else
			{
				y = vec_src[p] -= MX(vec_key); //(z >> 5 ^ y << 2) + (y >> 3 ^ z << 4) ^ (sum ^ y) + (vec_key[p & 3 ^ e] ^ z);
			}
			
		}
		z = vec_src[n];
		if(if_old_alg)
		{
			y = vec_src[0] -= MX_OLD(vec_key); 
		}
		else
		{
			y = vec_src[0] -= MX(vec_key); //(z >> 5 ^ y << 2) + (y >> 3 ^ z << 4) ^ (sum ^ y) + (vec_key[p & 3 ^ e] ^ z);
		}
		
		sum = sum - delta;
	}	

	arrayIntToByte(vec_src, true, dest);
}
	
void XxteaCrypto::Encrypt(const string &src, const string &key, string &dest, bool if_old_alg)
{
	vector<unsigned int> vec_src;
	vector<unsigned int> vec_key;
	
	string realKey = key;
	if(key.empty())
	{
		realKey = "__TOOLKITS_DEF__";
	}
	else if(key.size() >= 16)
	{
		realKey.resize(16);
	}
	else 
	{
		realKey.resize(16, 0x00);
	}
		
	arrayByteToInt(src, true, vec_src);
	arrayByteToInt(realKey, false, vec_key);
	
	int n = vec_src.size() - 1;
	if (n < 1)
	{
		return;
	}
	if (vec_key.size() < 4)
	{
		return;
	}
	
	unsigned int z = vec_src[n], y = vec_src[0], delta = 0x9E3779B9, sum = 0, e;
	int p, q = 6 + 52 / (n + 1);

	while (q-- > 0)
	{
		sum = sum + delta;
		e = sum >> 2 & 3;
		for (p = 0; p < n; p++)
		{
			y = vec_src[p + 1];
			if(if_old_alg)
			{
				z = vec_src[p] += MX_OLD(vec_key); 
			}
			else
			{
				z = vec_src[p] += MX(vec_key); //(z >> 5 ^ y << 2) + (y >> 3 ^ z << 4) ^ (sum ^ y) + (vec_key[p & 3 ^ e] ^ z);
			}
			
		}
		y = vec_src[0];
		if(if_old_alg)
		{
			z = vec_src[n] += MX_OLD(vec_key); 
		}
		else
		{
			z = vec_src[n] += MX(vec_key); //(z >> 5 ^ y << 2) + (y >> 3 ^ z << 4) ^ (sum ^ y) + (vec_key[p & 3 ^ e] ^ z);
		}
		
	}
	
	string dest_base64;
	arrayIntToByte(vec_src, false, dest_base64);
	Base64::Encode(dest_base64, &dest);	
}

