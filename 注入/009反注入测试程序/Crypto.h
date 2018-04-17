#pragma  once

#include "botan_all.h"
#include <vector>
#include <string>

/*
参数：
	char* inBuff：原文
	int inLen ：原文长度
*/
void Md5Hash(const char* inBuff, int inLen, Botan::SecureVector<unsigned char> &out);

/*
参数：
       std::string passphrase ：密码
	   const char* inBuff：原文
	   int inLen ：原文长度
	   Botan::SecureVector<unsigned char> &out：加密后的数据
*/
void Aes128Crypto(std::string passphrase, const char* inBuff, int inLen, Botan::SecureVector<unsigned char> &out);



/*
参数：
	std::string passphrase ：密码
	const char* inBuff：密文
	int inLen ：密文长度
	Botan::SecureVector<unsigned char> &out：解密后的数据
*/
void Aes128UnCrypto(std::string passphrase, const char* inBuff, int inLen, Botan::SecureVector<unsigned char> &out);

/*
参数:
	char* buff,明文长度
	int nLen, 明文长度
返回值:
	base64后的数据
*/
std::string Base64Encode(const char* buff, int nLen);


/*
参数:
	const char* inputBuff, 密文
	int nInputLen 密文长度
	Botan::SecureVector<unsigned char> &out 原文
*/
void Base64Decode(const char* inputBuff, int nInputLen, Botan::SecureVector<unsigned char> &out);

/*
参数：
	Botan::SecureVector<unsigned char> &publicKey, 生成公钥
	Botan::SecureVector<unsigned char> &privateKey， 生成私钥
*/
void GenRsaKey(Botan::SecureVector<unsigned char> &publicKey, Botan::SecureVector<unsigned char> &privateKey);


/*
参数:
	Botan::SecureVector<unsigned char> &out:在控制台展示的字节
*/
void ShowSecureVector(Botan::SecureVector<unsigned char> &out);


/*
	功能：展示使用
*/
void Useage();
