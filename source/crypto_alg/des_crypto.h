#ifndef __DES_CRYPTO_H__
#define __DES_CRYPTO_H__

#include "define_crypto.h"
#include <string>
using namespace std;

//自己指定密钥时，切记勿使用函数DES_string_to_key，该函数是根据输入的string随机计算key，并不是将输入的string当作key。
//设置key必须使用DES_set_key_unchecked函数，而不能使用DES_set_key_check函数，否则计算出来的数据会不正确。
//openssl在进行DES运算时，仅按8字节块加密，所以必须自己进行数据拆分

class DesCrypto
{
public:
	DesCrypto() {}
	~DesCrypto() {}

public:
	static int Encrypt(int crypto_mode, int padding_type, const string &key, const string &src, const string &ivec, string &dest);
	static int Decrypt(int crypto_mode, const string &key, const string &src, const string &ivec, string &dest);

	static int CalcMAC(int mac_mode, const string &key, const string &mac_data, const string &ivec, string &mac_value);

	static int ConvertPin(int left_pin_format, int right_pin_format, const string &acc_no, const string &left_pin_cipher, const string &left_pik, const string &right_pik, string &right_pin_cipher);	
private:
	static void printHex(const string &title, const string &src);
	
	static int crypt_ecb_1(int enc_dec, const string &key, const string &src, string &dest);
	static int crypt_ecb_3(int enc_dec, const string &key, const string &src, string &dest);

	static int crypt_cbc_1(int enc_dec, const string &key, const string &src, const string &ivec, string &dest);
	static int crypt_cbc_3(int enc_dec, const string &key, const string &src, const string &ivec, string &dest);
	
	static void calc_mac_xor(const string &key, const string &mac_data, const string &ivec, string &mac_value);
	static void calc_mac_x99(const string &key, const string &mac_data, const string &ivec, string &mac_value);
	static void calc_mac_x919(const string &key, const string &mac_data, const string &ivec, string &mac_value);
	static void calc_mac_cup(const string &key, const string &mac_data, const string &ivec, string &mac_value);
};
#endif //__DES_CRYPTO_H__
