#include "hash_alg.h"
#include "common/string_utility.h"
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <openssl/err.h>
#include <vector>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>

using namespace std;

#define BOLOCK_LEN 16

void HashAlg::printHex(const string &title, const string &src)
{
	string hex;
	
	StringUtility::bcd2asc(src, hex);
	cout << title << hex << endl;
}

void HashAlg::MD5(const string &src, string &dest)
{
	unsigned char hash[MD5_DIGEST_LENGTH] = {0};
	::MD5((const unsigned char *)src.c_str(), src.size(), hash);
	dest = string((char *)hash, MD5_DIGEST_LENGTH);
	//printHex("md5=", dest);	
}

void HashAlg::SHA1(const string &src, string &dest)
{	
	unsigned char hash[SHA_DIGEST_LENGTH] = {0};
	::SHA1((const unsigned char *)src.c_str(), src.size(), hash);
	dest = string((char *)hash, SHA_DIGEST_LENGTH);
	//printHex("sha1=", dest);
}

void HashAlg::SHA224(const string &src, string &dest)
{	
	unsigned char hash[SHA224_DIGEST_LENGTH] = {0};
	::SHA224((const unsigned char *)src.c_str(), src.size(), hash);
	dest = string((char *)hash, SHA224_DIGEST_LENGTH);
	//printHex("sha224=", dest);
}

void HashAlg::SHA256(const string &src, string &dest)
{	
	unsigned char hash[SHA256_DIGEST_LENGTH] = {0};
	::SHA256((const unsigned char *)src.c_str(), src.size(), hash);
	dest = string((char *)hash, SHA256_DIGEST_LENGTH);
	//printHex("sha256=", dest);
}

void HashAlg::SHA384(const string &src, string &dest)
{	
	unsigned char hash[SHA384_DIGEST_LENGTH] = {0};
	::SHA384((const unsigned char *)src.c_str(), src.size(), hash);
	dest = string((char *)hash, SHA384_DIGEST_LENGTH);
	//printHex("sha384=", dest);
}

void HashAlg::SHA512(const string &src, string &dest)
{	
	unsigned char hash[SHA512_DIGEST_LENGTH] = {0};
	::SHA512((const unsigned char *)src.c_str(), src.size(), hash);
	dest = string((char *)hash, SHA512_DIGEST_LENGTH);
	//printHex("sha512=", dest);
}