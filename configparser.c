#include "configparser.h"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

enum {
	STATE_INIT,
	STATE_IN_IDENT,
	STATE_IN_INTEGER,
	STATE_IN_STRING,
	STATE_IN_STRING_ESCAPE,
	STATE_IN_COMMENT,
	STATE_DONE
};

enum {
	TOKEN_INVALID,
	TOKEN_IDENT,
	TOKEN_STRING,
	TOKEN_INTEGER,
	TOKEN_LBRACKET,
	TOKEN_RBRACKET,
	TOKEN_EQUAL,
	TOKEN_EOL,
	TOKEN_EOF
};

int
get_token(FILE *file, char buf[], int len)
{
	int i;
	int ch;
	int state;
	int token;

	i     = 0;
	len   = len - 1;
	state = STATE_INIT;
	token = TOKEN_INVALID;
	while (state != STATE_DONE) {
		if (buf != NULL && i >= len) {
			token = TOKEN_INVALID;
			break;
		}

		ch = fgetc(file);

		if (state == STATE_INIT) {
			if (ch == ' ' || ch == '\t') {
				continue;
			}

			if (isalpha(ch)) {
				state = STATE_IN_IDENT;
			} else if (isdigit(ch)) {
				state = STATE_IN_INTEGER;
			} else if (ch == '"') {
				state = STATE_IN_STRING;
				continue;		/* ignore first '"' */
			} else if (ch == '#') {
				state = STATE_IN_COMMENT;
			} else if (ch == '[') {
				token = TOKEN_LBRACKET;
				state = STATE_DONE;
			} else if (ch == ']') {
				token = TOKEN_RBRACKET;
				state = STATE_DONE;
			} else if (ch == '=') {
				token = TOKEN_EQUAL;
				state = STATE_DONE;
			} else if (ch == '\n') {
				token = TOKEN_EOL;
				state = STATE_DONE;
			} else if (ch == EOF) {
				token = TOKEN_EOF;
				state = STATE_DONE;
			} else {
				state = STATE_DONE;
			}
		}
		if (state == STATE_IN_IDENT) {
			if (!isalnum(ch)) {
				ungetc(ch, file);
				token = TOKEN_IDENT;
				state = STATE_DONE;
			} else if (buf != NULL) {
				buf[i++] = ch;
			}
		}
		if (state == STATE_IN_INTEGER) {
			if (!isdigit(ch)) {
				ungetc(ch, file);
				token = TOKEN_INTEGER;
				state = STATE_DONE;
			} else if (buf != NULL) {
				buf[i++] = ch;
			}
		}
		if (state == STATE_IN_STRING) {
			if (ch == '"') {
				token = TOKEN_STRING;
				state = STATE_DONE;
			} else if (ch == '\\') {
				state = STATE_IN_STRING_ESCAPE;
				continue;		/* ignore '\\' */
			} else if (buf != NULL) {
				buf[i++] = ch;
			}
		}
		if (state == STATE_IN_STRING_ESCAPE) {
			state = STATE_IN_STRING;

			if (ch == 't') {
				ch = '\t';
			} else if (ch == '\\') {
				ch = '\\';
			} else if (ch == 'r') {
				ch = '\r';
			} else if (ch == 'n') {
				ch = '\n';
			} else if (ch == '\"') {
				ch = '"';
			} else {
				token = TOKEN_INVALID;
				state = STATE_DONE;
			}
			if (state == STATE_IN_STRING && buf != NULL)
				buf[i++] = ch;
		}
		if (state == STATE_IN_COMMENT) {
			if (ch == '\n') {
				token = TOKEN_EOL;
				state = STATE_DONE;
			}
		}
	}
	if (buf != NULL)
		buf[i] = '\0';
	return token;
}

static void
syntax_error(char *filename, int lineno)
{
	fprintf(stderr, "%s:%d syntax error\n", filename, lineno);
}

int
configparser_parse(char *filename, configparser_section_t *insections)
{
	int token;
	int error;
	int lineno;
	char buf[4096];

	char n[4096];
	char v[4096];
	int  vlen;

	FILE *file;

        configparser_section_t *sections = NULL;
        configparser_option_t  *options  = NULL;

        file = fopen(filename, "r");
        if (file == NULL) {
                fprintf(stderr, "open file %s error\n", filename);
                return -1;
        }

	error = 0;
        lineno = 1;
	while (error == 0) {
		token = get_token(file, buf, sizeof(buf));
		if (token == TOKEN_INVALID) {
			error = 1;
			break;
		}
		if (token == TOKEN_EOF) {
			break;
		}
		if (token == TOKEN_EOL) {
			lineno++;
			continue;
		}
		if (token == TOKEN_LBRACKET) {
			char section[4096];

			token = get_token(file, buf, sizeof(buf));
			if (token == TOKEN_IDENT) {
				token = get_token(file, NULL, 0);
				if (token == TOKEN_RBRACKET) {
					memset(section, 0, sizeof(section));
					memcpy(section, buf, strlen(buf));
				} else {
					error = 1;
					break;
				}
			} else {
				error = 1;
				break;
			}
                        sections = insections;
                        while (sections->section != NULL) {
                                if (strcmp(sections->section, section) == 0)
                                        break;
                                sections++;
                        }
			continue;
		}
		if (token == TOKEN_IDENT) {
			vlen = 0;
			memset(n, 0, sizeof(n));
			memcpy(n, buf, strlen(buf));
			token = get_token(file, buf, sizeof(buf));
			if (token != TOKEN_EQUAL) {
				error = 1;
				break;
			}
		}
		if (token == TOKEN_INTEGER) {
			if (vlen != 0) {
				error = 1;
				break;
			}
			memcpy(v + vlen, buf, strlen(buf));
			vlen += strlen(buf);
			n[0] = 0;
		}
		if (token == TOKEN_STRING) {
			if (n[0] == 0) {
				error = 1;
				break;
			}
			memcpy(v + vlen, buf, strlen(buf));
			vlen += strlen(buf);
		}
		if (sections == NULL || sections->section == NULL) {
			continue;
		}
		options = sections->options;
		while (options->name != NULL) {
			if (strcmp(options->name, n) == 0) {
				memcpy(options->value, v, vlen);
			}
			options++;
		}
	}

	fclose(file);

	return error;
}
