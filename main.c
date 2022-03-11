#include <stdio.h>
#include <string.h>
#include <assert.h>

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
	TOKEN_BAD
} TOKEN_TYPE;

typedef struct {
	TOKEN_TYPE type;	
	unsigned int start_index;
	unsigned int length;
} TOKEN;

void Token_Print(TOKEN* t, char* string) {
	assert(t);
	printf("%.*s", t->length, string + t->start_index);
}

// TODO: Handle escaped chars
void LexString(char* text, int text_i, int text_len, TOKEN* token) {
	assert(text);
	assert(-1 < text_i);
	assert(token);


	token->type = TOKEN_BAD;
	token->start_index = text_i;
	while(text_i < text_len)
	{
		if(text[text_i] == '\"') {
			token->type = TOKEN_STRING;
			break;
		}

		++text_i;
	}

	token->length = text_i - token->start_index;
}

void LexLiteral(char* text, int text_i, int text_len, TOKEN* token, const char* literal) {
	assert(text);
	assert(-1 < text_i);
	assert(token);
	assert(literal);

	token->type = literal[0] == 'n' ? TOKEN_NULL : TOKEN_BOOLEAN;
	token->start_index = text_i;
	token->length = 1;

	++text_i;
	int literal_i = 1;
	while(literal[literal_i]) {
		if(text_i == text_len) {
			token->type = TOKEN_BAD;
			break;
		}
		else if(text[text_i] != literal[literal_i]) {
			token->type = TOKEN_BAD;
		}

		++text_i;
		++literal_i;
		++token->length;
	}
}

int main() {
	char* json_string = "true false null";
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
				if(tokens[token_i].type == TOKEN_BAD) {
					printf("BAD_TOKEN: [%d,%d) ", tokens[token_i].start_index, tokens[token_i].start_index + tokens[token_i].length);
					Token_Print(&tokens[token_i], json_string);
					printf("\n");
					bad_token_found = 1;
				}

				text_i = tokens[token_i].start_index + tokens[token_i].length;
				// Move past end quote
				++text_i;
				++token_i;
			} break;

			// VALID LITERALS
			case 't': {
				LexLiteral(json_string, text_i, text_len, &tokens[token_i], "true");
				
				text_i = tokens[token_i].start_index + tokens[token_i].length;
				++token_i;
			} break;

			case 'f': {
				LexLiteral(json_string, text_i, text_len, &tokens[token_i], "false");
				
				text_i = tokens[token_i].start_index + tokens[token_i].length;
				++token_i;
			} break;

			case 'n': {
				LexLiteral(json_string, text_i, text_len, &tokens[token_i], "null");
				
				text_i = tokens[token_i].start_index + tokens[token_i].length;
				++token_i;
			} break;

			// INVALID TOKENS
			default: {
				LexInvalid(json_string, text_i, text_len, &tokens[token_i]);
				bad_token_found = 1;
			} break;
		}
	}

	// Print Tokens
	printf("\n** TOKENS **\n[");
	for (int i = 0; i < token_i; i++) {
		Token_Print(&tokens[i], json_string);
		printf(", ");
	}
	printf("]\n");
	
	return 0;
}