
#ifndef LOGIN_H
#define LOGIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//signin fuction
int signin(char user[30], char pass[30]);

//check sum of hash value with hash value in text file
int Checksum(char user[30], char pass[30]);

//register account
int account_reg(char user[30], char pass[30]);

//convert string to binary
const char* StringToBin(char user[30], char pass[30]);

//convert binary to number
__int32_t BinToNum(char* binary);

//Encrypt user name and password hash function
__int128_t Encrypt(char user[30], char pass[30]);

//Add padding to binary for hashing
void Padding(char *binary, __int64_t len);

#endif