#ifndef __HASH_ALG_H__
#define __HASH_ALG_H__

#include "define_crypto.h"
#include <string>
using namespace std;

//自己指定密钥时，切记勿使用函数DES_string_to_key，该函数是根据输入的string随机计算key，并不是将输入的string当作key。
//设置key必须使用DES_set_key_unchecked函数，而不能使用DES_set_key_check函数，否则计算出来的数据会不正确。
//openssl在进行DES运算时，仅按8字节块加密，所以必须自己进行数据拆分

class HashAlg
{
public:
	HashAlg() {}
	~HashAlg() {}

public:
	static void MD5(const string &src, string &dest);
	static void SHA1(const string &src, string &dest);
	static void SHA224(const string &src, string &dest);
	static void SHA256(const string &src, string &dest);
	static void SHA384(const string &src, string &dest);
	static void SHA512(const string &src, string &dest);

private:
	static void printHex(const string &title, const string &src);
};
#endif //__HASH_ALG_H__
