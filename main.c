#include "config.h"

int
main(int argc, char *argv[])
{
        char section[CONFIG_MAXLINE];
        char name[CONFIG_MAXLINE];
        char value[CONFIG_MAXLINE];

        struct configparser config;
        configparser_open(&config, argv[1]);
        while (configparser_parse(&config, section, name, value)) {
                printf("section: ``%s'', name: ``%s'', value: ``%s''\n", section, name, value);
        }
        configparser_close(&config);
        return 0;
}

