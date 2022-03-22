#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

#define ASCII_OFFSET_NUMBERS 48
#define MAX_INT_64_DIV_10 922337203685477580LL
#define MAX_INT_64 9223372036854775800LL

typedef enum {
	TOKEN_OPEN_CURLY,
	TOKEN_CLOSE_CURLY,
	TOKEN_OPEN_BRACKET,
	TOKEN_CLOSE_BRACKET,
	TOKEN_COLON,
	TOKEN_COMMA,
	TOKEN_STRING,
	TOKEN_BOOLEAN,
	TOKEN_NULL,
	TOKEN_INT_64,
	TOKEN_BAD
} TOKEN_TYPE;

typedef struct {
	TOKEN_TYPE type;	
	unsigned int start_index;
	unsigned int length;
	union {
		long long int numi;
		double numf;
	};
} TOKEN;

void Token_Print(TOKEN* t, char* string) {
	assert(t);
	if(t->type == TOKEN_BAD) {
		printf("XXX---%.*s---XXX", t->length, string + t->start_index);
	}
	else {
		printf("%.*s", t->length, string + t->start_index);
	}
}

// TODO: Handle escaped chars
void LexString(char* text, int text_i, int text_len, TOKEN* token) {
	assert(text);
	assert(-1 < text_i);
	assert(token);

	token->type = TOKEN_BAD;
	token->start_index = text_i;
	while(text_i < text_len) {
		if(text[text_i] == '\"') {
			token->type = TOKEN_STRING;
			break;
		}

		++text_i;
	}

	token->length = text_i - token->start_index;
}

// NOTE: This just finds the end of the literal
void LexLiteral(char* text, int text_i, int text_len, TOKEN* token) {
	assert(text);
	assert(-1 < text_i);
	assert(token);

	token->type = TOKEN_BAD;
	token->start_index = text_i;
	while(text_i < text_len    &&
		  text[text_i] != ' '  &&
		  text[text_i] != ','  &&
		  text[text_i] != '\r' &&
		  text[text_i] != '\n' &&
		  text[text_i] != '\t')
	{
		++text_i;
	}

	token->length = text_i - token->start_index;
}

inline int CharIsNum(char c) {
	return (int)('0' <= c && c <= '9');
}

void LexNumber(char* text, int text_i, int text_len, TOKEN* token) {
	assert(text);
	assert(token);

	int8_t factor = 1;
	if(text[text_i] == '-') {
		factor = -1;
		++text_i;
	}

	long long int number = 0;
	while(text_i < token->start_index + token->length) {
		char c = text[text_i++];
		uint8_t digit = c - ASCII_OFFSET_NUMBERS;

		if(!CharIsNum(c)) {
			return;
		}
		else {
			// Check if multiplying by 10 will overflow
			// Ex. 922337203685477581
			if(number > MAX_INT_64_DIV_10) {
				printf("Overflow!!\n");
				return;
			}
			number *= 10;

			if(number == MAX_INT_64) {
				if(factor == 1 && digit > 7) {
					printf("Overflow!!\n");
					return;
				}
				else if(factor == -1 && digit > 8) {
					printf("Underflow!!\n");
					return;
				}
			}
			number += digit;
		}
	}

	token->numi = number * factor;
	token->type = TOKEN_INT_64;
}

inline void LexTrue(char* text, TOKEN* token) {
	assert(text);
	assert(token);

	// NOTE: We already should have checked the first letter so we can just
	// check the next letters
	int i = token->start_index + 1;
	if(token->length == 4 &&
	   text[i]     == 'r' &&
	   text[i + 1] == 'u' &&
	   text[i + 2] == 'e')
	{
		token->type = TOKEN_BOOLEAN;
	}
}

inline void LexFalse(char* text, TOKEN* token) {
	assert(text);
	assert(token);

	// NOTE: We already should have checked the first letter so we can just
	// check the next letters
	int i = token->start_index + 1;
	if(token->length == 5 &&
	   text[i]     == 'a' &&
	   text[i + 1] == 'l' &&
	   text[i + 2] == 's' &&
	   text[i + 3] == 'e')
	{
		token->type = TOKEN_BOOLEAN;
	}
}

inline void LexNull(char* text, TOKEN* token) {
	assert(text);
	assert(token);

	// NOTE: We already should have checked the first letter so we can just
	// check the next letters
	int i = token->start_index + 1;
	if(token->length == 4 &&
	   text[i]     == 'u' &&
	   text[i + 1] == 'l' &&
	   text[i + 2] == 'l')
	{
		token->type = TOKEN_NULL;
	}
}

int main() {
	char* json_string = "9223372036854775808";
	printf("** JSON **\n%s\n", json_string);
	int text_len = strlen(json_string); 

	TOKEN tokens[500];
	int token_i = 0;

	int bad_token_found = 0;
	int text_i = 0;
	while((text_i < text_len) && (!bad_token_found)) {
		char c = json_string[text_i];
		switch (c) {
			// STRUCTURAL CHARACTERS
			case '{': {
				tokens[token_i].type = TOKEN_OPEN_CURLY;
				tokens[token_i].start_index = text_i;
				tokens[token_i].length = 1;
				
				++token_i;
				++text_i;
			} break;

			case '}': {
				tokens[token_i].type = TOKEN_CLOSE_CURLY;
				tokens[token_i].start_index = text_i;
				tokens[token_i].length = 1;
				
				++token_i;
				++text_i;
			} break;

			case '[': {
				tokens[token_i].type = TOKEN_OPEN_BRACKET;
				tokens[token_i].start_index = text_i;
				tokens[token_i].length = 1;
				
				++token_i;
				++text_i;
			} break;

			case ']': {
				tokens[token_i].type = TOKEN_CLOSE_BRACKET;
				tokens[token_i].start_index = text_i;
				tokens[token_i].length = 1;
				
				++token_i;
				++text_i;
			} break;

			case ':': {
				tokens[token_i].type = TOKEN_COLON;
				tokens[token_i].start_index = text_i;
				tokens[token_i].length = 1;
				
				++token_i;
				++text_i;
			} break;

			case ',': {
				tokens[token_i].type = TOKEN_COMMA;
				tokens[token_i].start_index = text_i;
				tokens[token_i].length = 1;
				
				++token_i;
				++text_i;
			} break;

			// WHITESPACE CHARS
			case ' ':
			case '\r':
			case '\n':
			case '\t': {
				++text_i;
			} break;

			// TYPES
			case '\"': {
				// Move past the actual quote
				++text_i;
				LexString(json_string, text_i, text_len, &tokens[token_i]);
				// Add one to move past end quote
				text_i = tokens[token_i].start_index + tokens[token_i].length + 1;

				if(tokens[token_i].type == TOKEN_BAD) {
					printf("BAD_TOKEN [%d,%d): ", tokens[token_i].start_index, tokens[token_i].start_index + tokens[token_i].length);
					Token_Print(&tokens[token_i], json_string);
					printf("\n");
					bad_token_found = 1;
				}
				++token_i;
			} break;

			// LITERALS
			default: {
				LexLiteral(json_string, text_i, text_len, tokens + token_i);

				// Check which literal it is
				if(c == 't') {
					LexTrue(json_string, tokens + token_i);
				}
				else if(c == 'f') {
					LexFalse(json_string, tokens + token_i);
				}
				else if(c == 'n') {
					LexNull(json_string, tokens + token_i);
				}
				else if(c == '-' || CharIsNum(c)) {
					LexNumber(json_string, text_i, text_len, tokens + token_i);
				}

				text_i = tokens[token_i].start_index + tokens[token_i].length;

				if(tokens[token_i].type == TOKEN_BAD) {
					printf("BAD_TOKEN [%d,%d): ", tokens[token_i].start_index, tokens[token_i].start_index + tokens[token_i].length);
					Token_Print(&tokens[token_i], json_string);
					printf("\n");
					bad_token_found = 1;
				}
				++token_i;
			} break;
		}
	}

	// Print Tokens
	if(!bad_token_found) {
		printf("\n** TOKENS **\n[");
		for (int i = 0; i < token_i; i++) {
			if(tokens[i].type == TOKEN_INT_64) {
				printf("%lld", tokens[i].numi);
			}
			else {
				Token_Print(&tokens[i], json_string);
			}
			printf(", ");
		}
		printf("]\n");
	}
	
	return 0;
}