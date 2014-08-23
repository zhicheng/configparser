#include "configparser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
usage()
{
        fprintf(stderr, "usage ./configparser config.ini\n");
        exit(0);
}

int
main(int argc, char *argv[])
{
        static char bar[4096];
        static char world[4096];
	static char world2[4096];
	static long num = 0;

        static struct configparser_option options[] = {
                {"foo", CONFIGPARSER_TYPE_STR, bar},
                {"hello", CONFIGPARSER_TYPE_STR, world},
                {"num", CONFIGPARSER_TYPE_INT, &num},
                {NULL, 0, NULL}
        };

        static struct configparser_option options2[] = {
                {"hello", CONFIGPARSER_TYPE_STR, world2},
                {NULL, 0, NULL}
        };

        struct configparser_section sections[] = {
                {"hello", options},
                {"world", options2},
                {NULL, NULL},
        };

        if (argc != 2)
                usage();
	memset(bar, 0, sizeof(bar));
	memset(world, 0, sizeof(world));

        if (configparser_parse(argv[1], sections) != 0) {
		fprintf(stderr, "parsing error\n");
		return 0;
	}

        printf("val: `%s`\n", bar);
        printf("val: `%s`\n", world);
        printf("val: `%s`\n", world2);
	printf("num: %s\n", num);

        return 0;
}
