#pragma once

class logger
{
private:
    const char *name;
public:
    logger(const char *l);
    void info(const char *fmt, ...);
    void warning(const char *fmt, ...);
    void error(const char *fmt, ...);
    void debug(const char *fmt, ...);
    void critical(const char *fmt, ...);
};