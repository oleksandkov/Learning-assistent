#include <iostream>
#include <string>
#include <assert.h>

using namespace std;


int strToFloat(char *str, float *n) {
	char *endptr;
	float val = strtof(str, &endptr);
	if (endptr == str || *endptr != '\0') return -1;
	*n = val;
	return 0;
}

char topChar(char *str) {
	int freq[256] = {0};
	for (int i = 0; str[i]; i++) freq[(unsigned char)str[i]]++;
	int maxf = 0;
	char res = 0;
	for (int i = 0; i < 256; i++) {
		if (freq[i] > maxf) {
			maxf = freq[i];
			res = (char)i;
		}
	}
	return res;
}

char missingLetter(char *str) {
	int present[26] = {0};
	for (int i = 0; str[i]; i++) {
		char c = str[i];
		if (c >= 'A' && c <= 'Z') c = c - 'A' + 'a';
		if (c >= 'a' && c <= 'z') present[c - 'a'] = 1;
	}
	for (int i = 0; i < 26; i++) {
		if (!present[i]) return 'a' + i;
	}
	return 0;
}

void textStat(char *str, int *words, int *sentences) {
	*words = 0;
	*sentences = 0;
	int in_word = 0;
	for (int i = 0; str[i]; i++) {
		char c = str[i];
		if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
			if (!in_word) {
				(*words)++;
				in_word = 1;
			}
		} else {
			in_word = 0;
		}
		if (c == '.' || c == '!' || c == '?') (*sentences)++;
	}
}

int extractNumbers(char *str, int pos, int *numbers, int size) {
	int count = 0;
	int i = pos;
	while (str[i] && count < size) {
		while (str[i] && (str[i] < '0' || str[i] > '9') && str[i] != '-' && str[i] != '+') i++;
		if (!str[i]) break;
		char *endptr;
		int val = (int)strtol(&str[i], &endptr, 10);
		if (endptr == &str[i]) { i++; continue; }
		numbers[count++] = val;
		i = endptr - str;
	}
	return i;
}

void test_functions() {
	// strToFloat
	float n;
	assert(strToFloat((char*)"123.45", &n) == 0 && n == 123.45f);
	assert(strToFloat((char*)"abc", &n) == -1);
	// topChar
	assert(topChar((char*)"aabbc") == 'a' || topChar((char*)"aabbc") == 'b');
	assert(topChar((char*)"xyzxyzx") == 'x');
	// missingLetter
	assert(missingLetter((char*)"bcdefghijklmnopqrstuvwxyz") == 'a');
	assert(missingLetter((char*)"abcdefghijklmnopqrstuvwxyz") == 0);
	// textStat
	int w, s;
	textStat((char*)"Hello world. How are you?", &w, &s);
	assert(w == 5 && s == 2);
	// extractNumbers
	int arr[3];
	int idx = extractNumbers((char*)"abc 12 def -7 99", 0, arr, 3);
	assert(arr[0] == 12 && arr[1] == -7 && arr[2] == 99);
}

int main() {
	test_functions();
	printf("All tests passed!\n");
	return 0;
}