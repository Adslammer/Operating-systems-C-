#include <errno.h>

#define DAT_DNEVNIKA "/home/ubuntu/Desktop/dnevnik"
#define LOGIRAJ_GRESKU(format, ...)                                     \
{                                                                       \
        FILE *fp;                                                       \
        int br_greske;                                                  \
                                                                        \
        br_greske = errno;   /* zapamti broj greške */                  \
        if ( ( fp = fopen ( DAT_DNEVNIKA, "a" ) ) != NULL )             \
        {                                                               \
                fprintf ( fp, "Greska: %s\n", strerror ( br_greske ) ); \
                fprintf ( fp, "Datoteka: %s:%d\n" format, __FILE__, __LINE__, ##__VA_ARGS__ );          \
                fclose ( fp );                                          \
        }                                                               \
}

#define LOGIRAJ(format, ...)                                \
{                                                           \
        FILE *fp;                                           \
                                                            \
        if ( ( fp = fopen ( DAT_DNEVNIKA, "a" ) ) != NULL ) \
        {                                                   \
                fprintf ( fp, format, ##__VA_ARGS__ );      \
                fclose ( fp );                              \
        }                                                   \
}
