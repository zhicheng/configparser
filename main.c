#include "config.h"

int
main(int argc, char *argv[])
{
        char section[CONFIG_MAXLINE];
        char name[CONFIG_MAXLINE];
        char value[CONFIG_MAXLINE];

        struct config config;
        config_open(&config, argv[1]);
        while (config_parse(&config, section, name, value)) {
                printf("section: ``%s'', name: ``%s'', value: ``%s''\n", section, name, value);
        }
        config_close(&config);
        return 0;
}

