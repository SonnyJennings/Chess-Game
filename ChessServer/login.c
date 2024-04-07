#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#include "login.h"

//default 32 bit hash values
#define h0 0x67452301
#define h1 0xEFCDAB89
#define h2 0x98BADCFE
#define h3 0x10325476
#define h4 0xC3D2E1F0

__int128_t Encrypt(char user[30], char pass[30]) //inspired from SHA1
{
    char* hash = StringToBin(user, pass); //convert user + password to binary

    Padding(hash, strlen(user) + strlen(pass)); //Add padding

    __int32_t word [80]; //word array of 32 bits

    int index = 0;
    for(int i = 511; i > 0 ; i-=32) //512 bit = 16 times 32 bits
    {
      __int32_t sum = 0;
     for(int j = 0; j < 32; j++) //add chunks of 32 bits from 512 bit chunck
     {
        sum += (pow(2, j) * (hash[i-j]-48));
     }
     word[index] = sum; //add sum to word array
     index++;
    }

    for(int i = 16; i < 80; i++) //expand to 80 32bit 
    {
        word[i] = (word[i-3] ^ word[i-8] ^ word[i-14] ^ word[i-16]) << 1;
    }

    __int32_t a = h0;
    __int32_t b = h1;
    __int32_t c = h2;
    __int32_t d = h3;
    __int32_t e = h4;

    __int32_t f = 0;
    __int32_t k = 0;
   
    //Scrambling based on SHA1 algorithm
    for(int i = 0; i < 80; i++)
    {
        if((i >= 0) && (i <= 19))
        {
            f = (b & c) | ((!b) & d);
            k = 0x5A827999;
        }
        else if ((i >= 20) && (i <= 39))
        {
            f = (b ^ c ^ d);
            k = 0x6ED9EBA1;
        }
        else if ((i >= 40) && (i <= 59))
        {
            f = (b & c) | (b & d) | (c & d);
            k = 0x8F1BBCDC;
        }
        else if ((i >= 60) && (i <= 79))
        {
            f = b ^ c ^ d;
            k = 0xCA62C1D6;
        }
        __int32_t RotateLeft = (a << 5) | (a >> (32 - 5)); //rotate left by 5
        __int32_t temp = RotateLeft + f + e + k + word[i]; 
        e = d;
        d = c;
        RotateLeft = (a << 30) | (a >> (32 - 30)); //rotate right by 30
        c = RotateLeft;
        b = a;
        a = temp;
    }


    __int128_t hash0 = h0 + a;
    __int128_t hash1 = h1 + b;
    __int128_t hash2 = h2 + c;
    __int128_t hash3 = h3 + d;
    __int128_t hash4 = h4 + e;

    __int128_t hh = (hash0 << 128) | (hash1 << 96) | (hash2 << 64) | (hash3 <<32) | hash4; //get final 128 bit hash value

    free(hash);

    return hh;
    
}


const char* StringToBin(char user[30], char pass[30])
{
    char len = strlen(user) + strlen(pass);  //length of string user + pass
    char* str = (char*) malloc(len + 1); //stores user + pass 
    strcpy(str,user);
    strcat(str,pass); //combine user and pass to one string

    char *bin = (char*)malloc(512 * 8 + 1); //stores final binary as long string
    bin[0] = '\0';
    for(int i = 0; i < len; ++i) {
        char ch = str[i];  //char i of user + pass string
        for(int j = 7; j >= 0; --j) //check every bit of char
        { 
            if(ch & 1<<j) //and 1 for every bit
            {
                strcat(bin,"1"); //add one to binary num
            } else {
                strcat(bin,"0"); //add zero to binary num
            }
        }
    }
    free(str); //free str
 return bin; //return binary num
}

void Padding(char *binary, __int64_t len)
{ 
    strcat(binary,"1"); //add 1 to end of binary char/num
    
    while(strlen(binary) % 512 != 448) //while until binary is 448 bits since last 64 bits are for add lenght of message
    {
        strcat(binary,"0"); //keep adding 0s
    }
    
    for(int i = 63; i >= 0 ; i--) //Add lenghth of message to end of padding
    {
      __int64_t temp = len >> i; //shift len by i bits
      if(temp & 1) // & to get 1 or 0
      {
          strcat(binary, "1"); //add 1 to binary string/num
      }
      else
      {
          strcat(binary, "0"); //add 0 to binary string/num
      }
    }

  
}
__int32_t BinToNum(char* binary) //convert binary to num
{
    __int32_t sum = 0;
    for(int i =0; i< strlen(binary); i++)
    {
     sum+=(pow(2,i) * binary[i]); //multiply bit by power of 2
    }
    return sum;
}

int signin(char user[30], char pass[30]){
    FILE *f;
    char *line = NULL;
    size_t len = 0;
    size_t read;

    int checksum = 0;

    f= fopen("users.txt", "r"); //open users.txt for reading

    while((read = getline(&line, &len, f)) != -1 ) //get one line from file and store at line
    {
     checksum =0;

     for(int i = 0; i< strlen(line);i++) //Add each char read from line and add to checkSum
     {
         __uint8_t temp =line[i]; //Add "char" to temp, 8 bit since char is size of 1 byte
         checksum += temp;
         //printf("Result:%d\n", checksum);
         
     }
     
     if((checksum == Checksum(user,pass))) //If checksum equal to value from hash function
     {
       printf("\nUser login successful\n"); //print login successful
       free(line); //free line
       fclose(f); //close file
       return 1; //success login
     }
     
    }

    free(line); //free line

    printf("\nUser login unsucessful\n");
    fclose(f); //close file
    return 0; //failed login
}

int account_reg(char user[30], char pass[30])
{
 FILE *f;
 f = fopen("users.txt", "a"); //open file for adding

 __int128_t hashed = Encrypt(user, pass); //hash user and pass

 for(int i = 120; i>0; i-=8)  //add byte by byte char to file since a char is 1 byte
 {
     __uint8_t temp = hashed >> i; //shift by multiple of 8bits
     fprintf(f, "%c",temp); //print char to file
 }

 fprintf(f,"\n"); //print new line to file
 fclose(f); //close file
 return 0;
}

/*int main(){
    char user[30] = ""; 
    char pass[30] = "";

    printf("\nUsername:");
    scanf("%s",user);
    printf("\nPassword:");
    scanf("%s",pass);

    if(signin(user, pass) == 0)
    {
     account_reg(user,pass);
    }

    printf("\n%d\n", Encrypt(user,pass));


}*/

int Checksum(char user[30], char pass[30])
{
 int sum = 0;

 __int128_t hashed = Encrypt(user, pass); //get hash value using user and pass

 for(int i = 120; i>0; i-=8)
 {
     __uint8_t temp = hashed >> i;  //add char value to sum byte by byte
     sum += temp;
     //printf("Check:%d\n",sum);
 }

 sum += 10; //extra 10 to sum to accout for new line
 return sum; //return check sum value
}