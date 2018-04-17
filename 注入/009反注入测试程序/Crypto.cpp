#include "Crypto.h"
#include <iostream>

void Md5Hash(const char* inBuff, int inLen, Botan::SecureVector<unsigned char> &out)
{
	Botan::HashFunction* hash = Botan::get_hash("MD5");
	out = hash->process((unsigned char*)inBuff, inLen);
}

void Aes128Crypto(std::string passphrase, const char* inBuff, int inLen, Botan::SecureVector<unsigned char> &out)
{
	Botan::HashFunction* hash = Botan::get_hash("MD5");
	Botan::SymmetricKey key = hash->process(passphrase);
	Botan::SecureVector<unsigned char> raw_iv = hash->process('0' + passphrase);
	Botan::InitializationVector iv(raw_iv, 16);
	Botan::Pipe pipe(get_cipher("AES-128/CBC", key, iv, Botan::ENCRYPTION));
	pipe.process_msg((unsigned char*)inBuff, inLen);
	out = pipe.read_all();
}

void  Aes128UnCrypto(std::string passphrase, const char* inBuff, int inLen, Botan::SecureVector<unsigned char> &out)
{
	Botan::HashFunction* hash = Botan::get_hash("MD5");
	Botan::SymmetricKey key = hash->process(passphrase);
	Botan::SecureVector<unsigned char> raw_iv = hash->process('0' + passphrase);
	Botan::InitializationVector iv(raw_iv, 16);
	Botan::Pipe pipe(get_cipher("AES-128/CBC", key, iv, Botan::DECRYPTION));
	pipe.process_msg((unsigned char*)inBuff, inLen);
	out = pipe.read_all();
}


std::string Base64Encode(const char* buff, int nLen)
{
	return Botan::base64_encode((unsigned char*)buff, nLen);
}



void Base64Decode(const char* inputBuff, int nInputLen, Botan::SecureVector<unsigned char> &out)
{
	out =Botan::base64_decode(inputBuff, nInputLen,  true);
}

void GenRsaKey(Botan::SecureVector<unsigned char> &publicKey, Botan::SecureVector<unsigned char> &privateKey)
{

}

void ShowKey(Botan::SymmetricKey &key);

//使用说明
void Useage()
{
	//测试md5
	std::cout << "MD5测试" << std::endl;
	std::string strTipMsg = "what can i do for you";
	std::cout << "orgin str:" << strTipMsg << std::endl;
	Botan::SecureVector<unsigned char> md5Out;
	Md5Hash(strTipMsg.c_str(), strTipMsg.length(), md5Out);
	std::cout << "md5 strTipMsg:" << std::endl;
	ShowSecureVector(md5Out);
	std::cout << "_______________________________________" << std::endl;

	//测试base64
	std::cout << "base64测试" << std::endl;
	std::cout << "orgin str:" << strTipMsg << std::endl;
	std::string strEncodeBase64Name = Base64Encode(strTipMsg.c_str(), strTipMsg.length() + 1);
	Botan::SecureVector<unsigned char> decodeBase64;
	Base64Decode(strEncodeBase64Name.c_str(), strEncodeBase64Name.length(), decodeBase64);
	std::string strDecodeBase64Name = (char*)decodeBase64.begin();
	std::cout << "encode base64 name:" << strEncodeBase64Name << std::endl;
	std::cout << "decode base64 name:" << strDecodeBase64Name << std::endl;
	std::cout << "_______________________________________" << std::endl;

	//测试AES-128/CBC
	std::cout << "AES-128/CBC 测试" << std::endl;
	std::string strAesMsg = "cc101";
	std::cout << "orgin str:" << strAesMsg << std::endl;
	Botan::SecureVector<unsigned char> cryAesCode;
	//执行加密
	Aes128Crypto("hzh01", strAesMsg.c_str(), strAesMsg.length() + 1, cryAesCode);
	//加密后执行base64
	std::string strAesBase64 = Base64Encode((const char*)cryAesCode.begin(), cryAesCode.size());
	std::cout << "crypto by aes and base64:" << strAesBase64 << std::endl;
	//反base64编码，获取密文放到decodeAesBase64
	Botan::SecureVector<unsigned char> decodeAesBase64;
	Base64Decode(strAesBase64.c_str(), strAesBase64.length(), decodeAesBase64);
	//根据密文求原文
	Botan::SecureVector<unsigned char> aesOrignCode;
	Aes128UnCrypto("hzh01", (char*)decodeAesBase64.begin(), decodeAesBase64.size(), aesOrignCode);
	//获取字符串
	std::string strOrignStr = (char*)aesOrignCode.begin();
	std::cout << "un crypto aes:" << strOrignStr << std::endl;
	std::cout << "_______________________________________" << std::endl;

}

void ShowSecureVector(Botan::SecureVector<unsigned char> &out)
{
	for (auto ite = out.begin(); ite < out.end(); ite++)
	{
		if ((unsigned int)(*ite) < 0x10)
		{
			std::cout << std::hex << 0;
			std::cout << std::hex << (unsigned int)(*ite);
		}
		else
		{
			std::cout << std::hex << (unsigned int)(*ite);
		}
	}
	std::cout << std::endl;
}


