#include <string.h>
#include <algorithm>
#include <sstream>
#include <iconv.h>
#include <iostream>
#include <stdio.h>
#include "string_utility.h"
#include <arpa/inet.h>
//#include "define.h"

void StringUtility::Split(const std::string& str,
    const std::string& delim,
    std::vector<std::string>* result) {
    if (str.empty()) {
        return;
    }
    if (delim[0] == '\0') {
        result->push_back(str);
        return;
    }

    size_t delim_length = delim.length();

    for (std::string::size_type begin_index = 0; begin_index < str.size();) {
        std::string::size_type end_index = str.find(delim, begin_index);
        if (end_index == std::string::npos) {
            result->push_back(str.substr(begin_index));
            return;
        }
        if (end_index > begin_index) {
            result->push_back(str.substr(begin_index, (end_index - begin_index)));
        }

        begin_index = end_index + delim_length;
    }
}

bool StringUtility::StartsWith(const std::string& str, const std::string& prefix) {
    if (prefix.length() > str.length()) {
        return false;
    }

    if (memcmp(str.c_str(), prefix.c_str(), prefix.length()) == 0) {
        return true;
    }

    return false;
}

bool StringUtility::EndsWith(const std::string& str, const std::string& suffix) {
    if (suffix.length() > str.length()) {
        return false;
    }

    return (str.substr(str.length() - suffix.length()) == suffix);
}

std::string& StringUtility::Ltrim(std::string& str) { // NOLINT
    std::string::iterator it = find_if(str.begin(), str.end(), std::not1(std::ptr_fun(::isspace)));
    str.erase(str.begin(), it);
    return str;
}

std::string& StringUtility::Rtrim(std::string& str) { // NOLINT
    std::string::reverse_iterator it = find_if(str.rbegin(),
        str.rend(), std::not1(std::ptr_fun(::isspace)));

    str.erase(it.base(), str.end());
    return str;
}

std::string& StringUtility::Trim(std::string& str) { // NOLINT
    return Rtrim(Ltrim(str));
}

void StringUtility::Trim(std::vector<std::string>* str_list) {
    if (NULL == str_list) {
        return;
    }

    std::vector<std::string>::iterator it;
    for (it = str_list->begin(); it != str_list->end(); ++it) {
        *it = Trim(*it);
    }
}

void StringUtility::string_replace(const std::string &sub_str1,
    const std::string &sub_str2, std::string *str) {
    std::string::size_type pos = 0;
    std::string::size_type a = sub_str1.size();
    std::string::size_type b = sub_str2.size();
    while ((pos = str->find(sub_str1, pos)) != std::string::npos) {
        str->replace(pos, a, sub_str2);
        pos += b;
    }
}

std::string StringUtility::IntToString(const int number)
{
    std::stringstream ss;
    std::string str;
    ss << number;
    ss >> str;

    return str;
}

int StringUtility::charsetConvert(const std::string &from_charset,const std::string &to_charset, const std::string &strSrc,  std::string &strDest)
{
    iconv_t cd;

    size_t inlen = strSrc.size();
    size_t outLen = inlen * 4;
    char *pIn = (char *)strSrc.c_str();
    char *pOut = new char[outLen];
    char *p = pOut;

    memset(pOut, 0, outLen);

    cd = iconv_open(to_charset.c_str(),from_charset.c_str());
    if (cd == 0)
    {
        delete []p;
        return -1;
    }

    if (iconv(cd, &pIn, &inlen, &pOut, &outLen) == -1)
    {
        delete []p;
        iconv_close(cd);
        return -1;
    }

    strDest.assign(p);

    delete []p;

    iconv_close(cd);

    return 0;
}


/*
 *  功能     --  ASCII码转换到BCD码,ASCII为奇数时候右补'0'(左靠)
 *  输入参数 --  strAsc	要转换的ASCII码的字符串;
 *  输出参数 --  strBcd	转换后的BCD码的字符串;
 *  return   --  返回BCD码字符串
 */
std::string &StringUtility::asc2bcd_l(const std::string &strAsc, std::string &strBcd)
{
	int i, iLen;
	std::string strTmp(strAsc);

	if ( strAsc.size() % 2 != 0 )//补0
	{
		strTmp.insert(strAsc.size(), 1, '0');
	}
	iLen = strTmp.size();

	for(i = 0; i < iLen; ++i)
	{
		if (strTmp[i] >= 'a')
		{
			strTmp[i] -= 'a';
			strTmp[i] += 10 ;
		}
		else if (strTmp[i] >= 'A')
		{
			strTmp[i] -= 'A';
			strTmp[i] += 10 ;
		}
		else if (strTmp[i] >= '0')
		{
			strTmp[i] -= '0';
		}
		else
		{
			strTmp[i] = 0;
		}
	}
	strBcd = "";
	for (i = 0; i < iLen; i+=2)
	{
		/*BCD码	的高4位存放ASCII的前一个字符,低4位存放ASCII的后一个字符*/
		strBcd.insert( strBcd.size(), 1,  (((strTmp[i]&0x0f)<<4) | (strTmp[i+1]&0x0f)) );
	}

	return strBcd;
}

/*
 *  功能     --  ASCII码转换到BCD码
 *  输入参数 --  strAsc	要转换的ASCII码的字符串;
 *  输出参数 --  strBcd	转换后的BCD码的字符串;
 *  return   --  返回BCD码字符串
 */
void StringUtility::asc2bcd(const char *pstrSou, int nLen, char *pstrDes )
{
    int i, ch;
    char *pstrTmp = new char[nLen+1];

    sprintf( pstrTmp, "%*.*sF", nLen, nLen, pstrSou );
    for( i=0; i<(nLen+1)/2; i++ )
    {
        sscanf( pstrTmp+i*2, "%02X", &ch );
        pstrDes[i] = ch & 0xFF;
    }
    delete pstrTmp;
    pstrTmp = NULL;

    pstrDes[i] = 0;
}

/*
 *  功能     --  ASCII码转换到BCD码,ASCII为奇数时候左补'0'(右靠)
 *  输入参数 --  strAsc	要转换的ASCII码的字符串;
 *  输出参数 --  strBcd	转换后的BCD码的字符串;
 *  return   --  返回BCD码字符串
 */
std::string &StringUtility::asc2bcd_r(const std::string &strAsc, std::string &strBcd)
{
	int i, iLen;
	std::string strTmp(strAsc);

	if ( strAsc.size() % 2 != 0 )//补0
	{
		strTmp.insert((std::string::size_type)0, 1, '0');
	}
	iLen = strTmp.size();

	for(i = 0; i < iLen; ++i)
	{
		if (strTmp[i] >= 'a')
		{
			strTmp[i] -= 'a';
			strTmp[i] += 10 ;
		}
		else if (strTmp[i] >= 'A')
		{
			strTmp[i] -= 'A';
			strTmp[i] += 10 ;
		}
		else if (strTmp[i] >= '0')
		{
			strTmp[i] -= '0';
		}
		else
		{
			strTmp[i] = 0;
		}
	}
	strBcd = "";
	for (i = 0; i < iLen; i+=2)
	{
		/*BCD码	的高4位存放ASCII的前一个字符,低4位存放ASCII的后一个字符*/
		strBcd.insert( strBcd.size(), 1,  (((strTmp[i]&0x0f)<<4) | (strTmp[i+1]&0x0f)) );
	}

	return strBcd;
}

/*
 *  功能     --  BCD码转换到ASCII码
 *  输入参数 --  strBcd	要转换的BCD码字符串;
 *  输出参数 --  strAsc	转换后的ASCII码字符串;
 *  return   --  返回ASCII字符串;
 */
std::string &StringUtility::bcd2asc(const std::string &strBcd, std::string &strAsc)
{
	int i, iLen;
	char c;

	iLen = strBcd.size();
	strAsc = "";
	for (i = 0; i < iLen; ++i)
	{
		c = (strBcd[i] >> 4) & 0x0f;
		if(c < 10)
		{
			c += '0';
		}
		else
		{
			c = c - 10 + 'A';
		}

		strAsc.insert(strAsc.size(), 1, c);
		c = strBcd[i] & 0x0f;
		if(c < 10)
		{
			c += '0';
		}
		else
		{
			c = c - 10 + 'A';
        }
		strAsc.insert(strAsc.size(), 1, c);
	}

	return strAsc;
}
/*
 *  功能     --  BCD码转换到ASCII码
 *  输入参数 --  strBcd	要转换的BCD码字符串;
 *  输出参数 --  strAsc	转换后的ASCII码字符串;
 *  return   --  返回ASCII字符串;
 */
void StringUtility::bcd2asc(const char *soustr, int len, char *desstr )
{
    int i;
    char lData,hData;
    char *pdes;

    *desstr = '\0';
    pdes = desstr;
    for( i=0; i<len; i++)
    {
    	lData=soustr[i] & 0xf;
	    if(lData<0xa)
		{
			lData+=0x30;
		}
		else
		{
			lData+=0x37;
		}
	    hData=((soustr[i] & 0xf0)>>4);
	    if(hData<0xa)
		{
			hData+=0x30;
		}
		else
		{
			hData+=0x37;
		}
        pdes[(i<<1)]=hData;
        pdes[(i<<1)+1]=lData;
        //pdes += 2;
    }
}


void StringUtility::Xor(const std::string &src, std::string &dest)
{
	if ( (src.size() !=8) || (dest.size() !=8) )
	{
		return;
	}
	for (int i=0; i<8; i++)
	{
		dest[i] ^= src[i];
	}
}

unsigned char StringUtility::CalXorCrc(const std::string &src)
{
	unsigned char crc = 0x00;
	const unsigned char *p = (const unsigned char *)src.c_str();

	for (unsigned int i=0; i<src.size(); i++)
	{
		crc ^= p[i];
	}

	return crc;
}

/**
* 功能：IP转换，从字符串转换为整型
* 成功： 返回无符号整数
* 失败： 返回0；
**/
unsigned long StringUtility::IPStringToLong(const std::string &ip)
{
	const char* p = ip.c_str();
	char buf[20] = {0};
	int index = 0;
	while (*p)
	{
		if (*p == '.')
		{
			buf[index ++] = *p;
			p ++;
			continue;
		}
		if (*p >= '0' && *p <= '9')
		{
			buf[index ++] = *p;
			p ++;
			continue;
		}
		if (*p == ' ')
		{
			p ++;
			continue;
		}
		return 0;
	}

	std::string ipstr = buf;
	if (strlen(buf) == 0)
	{
		return 0;
	}

	return ntohl(inet_addr(buf));
}

/**
* 功能：IP转换，从为整型转换字符串
* 成功： ip字符串
* 失败： 返回“”；
**/
std::string StringUtility::IPLongToString(unsigned long IP)
{
	if (IP == 0){
		return "";
	}

	int i1 = ((IP >> 24) & 0xFF) ;
	int i2 = ((IP >> 16) & 0xFF) ;
	int i3 = ((IP >> 8) & 0xFF) ;
	int i4 = (IP & 0xFF);

	char buf[128] = {0};
	sprintf(buf, "%d.%d.%d.%d", i1, i2, i3, i4);

	return std::string(buf);
}

/**
 * long long转换为string
 * 单位为分
 */
std::string StringUtility::ToStringAmount(int64_t d) 
{
    std::ostringstream os;
	
    int prec=12; // 18
    os.precision(prec);
    os.width(12);
    os.fill('0');
    if (os << d)
        return os.str();
	
    return "";
}
