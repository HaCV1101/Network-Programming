#ifndef __LIB_H__
#define __LIB_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define shortS 20
#define longS 60
#define MAX_L 1024
char emptyString[longS] = "NON-STR";
char errorString[shortS] = "ERR-STR";
char okString[shortS] = "OK-STR";
char sentDone[shortS] = "MSG-SENT";
char sentError[shortS] = "MSG-ERR";

int checkString(char* string) {			// check if a valid password (only numbers and letters) or not
	int l = strlen(string);
	for(int i = 0; i < l; i++) {
		if(string[i] < '0' || string[i] > '9') 		// numbers
			if(tolower(string[i]) < 'a' || tolower(string[i]) > 'z')	// letters
				return 0;	// invalid string
	}
	return 1;	// valid string
}

long long charToNum(char* s) {        // convert string to int
    int p = 1, res = 0;
    for(int i = strlen(s) - 1; i >= 0; i--) {
        res += (s[i] - '0') * p;
        p *= 10;
    }
    return res;
}

void revStr(char* str) {        // reverse a string
    int l = strlen(str);
    char tmp;
    for(int i = 0; i < l / 2; i++) {
        tmp = str[i];
        str[i] = str[l - i - 1];
        str[l - i - 1] = tmp;
    }
}

void numToString(int x, char* res) {        /// convert number to string
    // char res[10];
    int c = 0;
    while (x != 0)
    {
        res[c++] = (x % 10) + '0';
        x /= 10;
    }
    revStr(res);
}

void splitString(char* s, char* charString, char* numString) {        // split string into number string letter string
    memset(charString, '\0', sizeof(charString));
    memset(numString, '\0', sizeof(numString));
    int lchar = 0;
    int lnum = 0;

    for(int i = 0; i < strlen(s); i++) {
        if(s[i] >= '0' && s[i] <= '9') {        // is a number
            numString[lnum] = s[i];
            lnum++;
        }
        else {              // is a letter
            charString[lchar] = s[i];
            lchar++;
        }
    }
    numString[lnum] = '\0';
    charString[lchar] = '\0';
}

#endif