/*
 * Tencent is pleased to support the open source community by making Pebble available.
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.
 * Licensed under the MIT License (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT
 * Unless required by applicable law or agreed to in writing, software distributed under the License
 * is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
 * or implied. See the License for the specific language governing permissions and limitations under
 * the License.
 *
 */


#ifndef _COMMON_STRING_UTILITY_H_
#define _COMMON_STRING_UTILITY_H_

#include <string>
#include <vector>

class StringUtility {
public:
    static void Split(const std::string& str,
        const std::string& delim,
        std::vector<std::string>* result);

    // 判断字符串str是否是以prefix开头
    static bool StartsWith(const std::string& str, const std::string& prefix);

    static bool EndsWith(const std::string& str, const std::string& suffix);

    static std::string& Ltrim(std::string& str); // NOLINT

    static std::string& Rtrim(std::string& str); // NOLINT

    static std::string& Trim(std::string& str); // NOLINT

    static void Trim(std::vector<std::string>* str_list);

    static std::string IntToString(const int number);

    // 子串替换
    static void string_replace(const std::string &sub_str1,
        const std::string &sub_str2, std::string *str);

    static int charsetConvert(const std::string &from_charset,const std::string &to_charset,
                              const std::string &strSrc, std::string &strDest);

	static void asc2bcd(const char *pstrSou, int nLen, char *pstrDes );

	static std::string &asc2bcd_l(const std::string &strAsc, std::string &strBcd);

	static std::string &asc2bcd_r(const std::string &strAsc, std::string &strBcd);

	static void bcd2asc(const char *pstrSou, int nLen, char *pstrDes );

 	static std::string &bcd2asc(const std::string &strBcd, std::string &strAsc);

	static void Xor(const std::string &src, std::string &dest);

	static unsigned char CalXorCrc(const std::string &src);
};

#endif // _COMMON_STRING_UTILITY_H_
