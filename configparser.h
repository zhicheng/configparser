#ifndef __CONFIGPARSER_H__
#define __CONFIGPARSER_H__

enum {CONFIGPARSER_TYPE_STR, CONFIGPARSER_TYPE_INT, CONFIGPARSER_TYPE_BOOL};

typedef struct configparser_option {
        char *name;			/* 1024 */
	int   type;
        void *value;			/* 4096 */
} configparser_option_t;

typedef struct configparser_section {
        char *section;			/* 1024 */
        struct configparser_option *options;
} configparser_section_t;

int
configparser_parse(char filename[], configparser_section_t *insections);


#endif /* __CONFIGPARSER_H__ */
