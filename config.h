#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdio.h>

#define CONFIG_MAXLINE 4096

struct config {
        FILE *fp;
        char  section[CONFIG_MAXLINE];
};

int
config_open(struct config *config, char *filename);

int
config_close(struct config *config);

int
config_parse(struct config *config, char *section, char *name, char *value);


#endif /* __CONFIG_H__ */
