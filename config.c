#include "config.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int
configparser_open(struct configparser *config, char *filename)
{
	config->fp = fopen(filename, "r");
	memset(config->section, 0, sizeof(config->section));
	return 1;
}

int
configparser_close(struct configparser *config)
{
	fclose(config->fp);
	return 1;
}

int
configparser_parse(struct configparser *config, char *section, char *name, char *value)
{
	int  len;
	char buf[CONFIG_MAXLINE];

	memset(section, 0, sizeof(config->section));
	while (fgets(buf, CONFIG_MAXLINE, config->fp) != NULL) {
		if (strlen(buf) == 0)
			continue;

		if (buf[strlen(buf)-1] != '\n')	/* MAXLINE Exceed */
			break;

		/* [section] */
		if (buf[0] == '[') {
			sscanf(buf, "[%[^]]", config->section);
			continue;
		}

		if (!isalpha(buf[0]))		/* ignore line */
			continue;

		/* name:value or name=value */
		if (sscanf(buf, "%[^:=\t ]%*[:=\t ]%[^\n]s", name, value) != 2)
			break;

		/* right trim value */
		for (len = strlen(value); len > 0 && isspace(value[len-1]); len--)
			;
		value[len] = '\0';
		memcpy(section, config->section, CONFIG_MAXLINE);

		return 1;
	}
	return 0;
}
