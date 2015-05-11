#ifndef YVRSA
#define YVRSA

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <openssl/rsa.h>
#include <openssl/aes.h>

class yvRSA {
private:
	RSA *r;
public:
	yvRSA() {
		srand(time(0));
		r=RSA_new();
	}
	void GenKey() {
		Close();
		r=RSA_generate_key(2048,0x10001,NULL,NULL);
	}
	unsigned char* Encrypt(const char* msg) {
		unsigned char *to=new unsigned char[RSA_size(r)];
		memset(to,0,RSA_size(r));
		RSA_public_encrypt(strlen(msg),(const unsigned char*)msg,to,r,RSA_PKCS1_PADDING);
		return to;
	}
	char* Decrypt(const unsigned char* code) {
		char *to=new char[256];
		memset(to,0,256);
		RSA_private_decrypt(RSA_size(r),code,(unsigned char*)to,r,RSA_PKCS1_PADDING);
		return to;
	}
	unsigned char* GetPubKey() {
		unsigned char *PublicKey=new unsigned char[280];
		memset(PublicKey,0,280);
		unsigned char *Pkey=PublicKey;
		i2d_RSAPublicKey(r,&Pkey);
		return PublicKey;
	}
	void SetPubKey(unsigned char* Pkey) {
		Close();
		r=d2i_RSAPublicKey(NULL,(const unsigned char**)&Pkey,270);
	}
	int GetKeySize() {return RSA_size(r);}
	void Close() {RSA_free(r);}
};

#endif

