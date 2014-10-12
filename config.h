#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdio.h>

#define CONFIG_MAXLINE 4096

struct configparser {
        FILE *fp;
        char  section[CONFIG_MAXLINE];
};

int
configparser_open(struct configparser *config, char *filename);

int
configparser_close(struct configparser *config);

int
configparser_parse(struct configparser *config, char *section, char *name, char *value);


#endif /* __CONFIG_H__ */
