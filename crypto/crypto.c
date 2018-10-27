#include <stdio.h>
#include <stdlib.h>
#include <crypt.h>
#include <sys/random.h>
// Encryption library //
char saltchars[]="abcdefghikjlmnopqrstuvWxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890+@£[]}";
char * makecrypt(passwd)
{
	char salt[13]="$1$abcdefgh$";
	getrandom(salt+3,8,0);
	for(unsigned int i=3;i<11;i++){
		salt[i]=saltchars[salt[i] & 0x3f];
	}
	char* encrypted=crypt(passwd,salt);
	return encrypted;
}
char * makecrypt(passwd,salt) {
	return crypt(passwd,salt);
}






