#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "logging.h"

#define LOGIT(fname, level) void fname(const char *format, ...) { va_list args; va_start(args, format); logVA(level, format, args); va_end(args); }
#define SETLOG(str, level) if (found || !strcmp(name, str)) { LOGLEVEL |= level; found = 1; }

// unsigned int LOGLEVEL = LOGMINUTIA | LOGTRACE | LOGDEBUG | LOGINFO | LOGTEST | LOGWARN | LOGERROR | LOGFATAL;
// unsigned int LOGLEVEL = LOGTRACE | LOGDEBUG | LOGINFO | LOGTEST | LOGWARN | LOGERROR | LOGFATAL;
// unsigned int LOGLEVEL = LOGDEBUG | LOGINFO | LOGTEST | LOGWARN | LOGERROR | LOGFATAL;
// unsigned int LOGLEVEL = LOGINFO | LOGTEST | LOGWARN | LOGERROR | LOGFATAL;
unsigned int LOGLEVEL = LOGTEST | LOGWARN | LOGERROR | LOGFATAL;

void logVA(unsigned int level, const char *format, va_list args) {
    if (level & LOGLEVEL) {
        vprintf(format, args);
    }
}

LOGIT(logFatal, LOGFATAL)
LOGIT(logError, LOGERROR)
LOGIT(logWarn, LOGWARN)
LOGIT(logTest, LOGTEST)
LOGIT(logInfo, LOGINFO)
LOGIT(logDebug, LOGDEBUG)
LOGIT(logTrace, LOGTRACE)
LOGIT(logMinutia, LOGMINUTIA)

void setLogLevelByName(const char *name) {
    unsigned char found = 0;

    LOGLEVEL = 0;
    SETLOG("LOGMINUTIA", LOGMINUTIA)
    SETLOG("LOGTRACE", LOGTRACE)
    SETLOG("LOGDEBUG", LOGDEBUG)
    SETLOG("LOGINFO", LOGINFO)
    SETLOG("LOGTEST", LOGTEST)
    SETLOG("LOGWARN", LOGWARN)
    SETLOG("LOGERROR", LOGERROR)
    SETLOG("LOGFATAL", LOGFATAL)

    if (!LOGLEVEL) {
        LOGLEVEL = LOGTEST | LOGWARN | LOGERROR | LOGFATAL;
    }
}
