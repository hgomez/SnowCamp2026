#ifndef __LOGGER_CPP_

extern void logger_impl(const char *fmt, ...);

// Définition de la macro pour simplifier l'appel
// '##__VA_ARGS__' gère le cas où l'utilisateur n'envoie pas d'arguments variables.
#define logger(format, ...) logger_impl(format, ##__VA_ARGS__)

#endif

#define LOG_LEVEL_FATAL 1
#define LOG_LEVEL_ERROR 2
#define LOG_LEVEL_WARN  3
#define LOG_LEVEL_INFO  4
#define LOG_LEVEL_DEBUG 5
#define LOG_LEVEL_TRACE 6

