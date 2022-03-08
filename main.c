#include <stdio.h>
#include <string.h>
#include <assert.h>

typedef enum {
	OPEN_CURLY,
	CLOSE_CURLY,
	OPEN_BRACKET,
	CLOSE_BRACKET,
	QUOTE,
	COLON,
	COMMA,
	STRING,
	BOOLEAN,
	BAD_TOKEN
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

// TODO: This doesn't handle escaped quote chars
void LexString(char* text, int text_i, int text_len, TOKEN* token) {
	assert(text);
	assert(-1 < text_i);
	assert(token);


	token->type = BAD_TOKEN;
	token->start_index = text_i;
	while(text_i < text_len)
	{
		if(text[text_i] == '\"') {
			token->type = STRING;
			break;
		}

		++text_i;
	}

	token->length = text_i - token->start_index;
}

// TODO: LexFalse, LexTrue, and LexNull can probably be one function
void LexTrue(char* text, int text_i, int text_len, TOKEN* token) {
	assert(text);
	assert(-1 < text_i);
	assert(token);

	token->type = BOOLEAN;
	token->start_index = text_i;
	token->length = 1;

	// NOTE: Since we only call this function when we see 't'
	//		 we can push text_i forward one and check for 'rue'
	++text_i;
	const char* true_literal = "rue";
	for(int i = 0; i < 3; ++i) {
		if(text_i == text_len) {
			token->type = BAD_TOKEN;
			break;
		}
		else if(text[text_i] != true_literal[i]) {
			token->type = BAD_TOKEN;
		}

		++text_i;
		++token->length;
	}
}

void LexFalse(char* text, int text_i, int text_len, TOKEN* token) {
	assert(text);
	assert(-1 < text_i);
	assert(token);

	token->type = BOOLEAN;
	token->start_index = text_i;
	token->length = 1;

	// NOTE: Since we only call this function when we see 'f'
	//		 we can push text_i forward one and check for 'alse'
	++text_i;
	const char* false_literal = "alse";
	for(int i = 0; i < 4; ++i) {
		if(text_i == text_len) {
			token->type = BAD_TOKEN;
			break;
		}
		else if(text[text_i] != false_literal[i]) {
			token->type = BAD_TOKEN;
		}

		++text_i;
		++token->length;
	}
}

int main() {
	// char* json_string = "{ 	\n\r"
	// 				"\"key\" : flse,"
	// 				"\"key\":[\"val\"]"
	// 			  "}";
	char* json_string = "trueb";
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
				tokens[token_i].type = OPEN_CURLY;
				tokens[token_i].start_index = text_i;
				tokens[token_i].length = 1;
				
				++token_i;
				++text_i;
			} break;

			case '}': {
				tokens[token_i].type = CLOSE_CURLY;
				tokens[token_i].start_index = text_i;
				tokens[token_i].length = 1;
				
				++token_i;
				++text_i;
			} break;

			case '[': {
				tokens[token_i].type = OPEN_BRACKET;
				tokens[token_i].start_index = text_i;
				tokens[token_i].length = 1;
				
				++token_i;
				++text_i;
			} break;

			case ']': {
				tokens[token_i].type = CLOSE_BRACKET;
				tokens[token_i].start_index = text_i;
				tokens[token_i].length = 1;
				
				++token_i;
				++text_i;
			} break;

			case ':': {
				tokens[token_i].type = COLON;
				tokens[token_i].start_index = text_i;
				tokens[token_i].length = 1;
				
				++token_i;
				++text_i;
			} break;

			case ',': {
				tokens[token_i].type = COMMA;
				tokens[token_i].start_index = text_i;
				tokens[token_i].length = 1;
				
				++token_i;
				++text_i;
			} break;

			// WHITESPACE CHARS
			case ' ': {
				++text_i;
			} break;
			case '\t': {
				++text_i;
			} break;
			case '\n': {
				++text_i;
			} break;
			case '\r': {
				++text_i;
			} break;

			// TYPES
			case '\"': {
				// Move past the actual quote
				++text_i;
				LexString(json_string, text_i, text_len, &tokens[token_i]);
				if(tokens[token_i].type == BAD_TOKEN) {
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

			case 'f': {
				LexFalse(json_string, text_i, text_len, &tokens[token_i]);
				if(tokens[token_i].type == BAD_TOKEN) {
					printf("BAD_TOKEN: [%d,%d) ", tokens[token_i].start_index, tokens[token_i].start_index + tokens[token_i].length);
					Token_Print(&tokens[token_i], json_string);
					printf("\n");
					bad_token_found = 1;
				}

				text_i = tokens[token_i].start_index + tokens[token_i].length;
				++token_i;
			} break;

			case 't': {
				LexTrue(json_string, text_i, text_len, &tokens[token_i]);
				if(tokens[token_i].type == BAD_TOKEN) {
					printf("BAD_TOKEN: [%d,%d) ", tokens[token_i].start_index, tokens[token_i].start_index + tokens[token_i].length);
					Token_Print(&tokens[token_i], json_string);
					printf("\n");
					bad_token_found = 1;
				}

				text_i = tokens[token_i].start_index + tokens[token_i].length;
				++token_i;
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