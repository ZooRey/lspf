#ifndef __XXTEA_CRYPTO_H__
#define __XXTEA_CRYPTO_H__

#include <string>
#include <vector>
using namespace std;


class XxteaCrypto
{
public:
	XxteaCrypto() {}
	~XxteaCrypto() {}

public:
	//src:base64; key:bcd; dest:asc
	static void Decrypt(const string &src, const string &key, string &dest, bool if_old_alg = false);
	//src:asc; key:bcd; dest:base64
	static void Encrypt(const string &src, const string &key, string &dest, bool if_old_alg = false);
	
private:
	static void arrayByteToInt(const string &src, bool include_length, vector<unsigned int> &dest);
	static void arrayIntToByte(const vector<unsigned int> &src, bool include_length, string &dest);
};

#endif //__XXTEA_CRYPTO_H__
