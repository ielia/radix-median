#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "logging.h"

#define LOGIT(fname, level) void fname(const char *format, ...) { va_list args; va_start(args, format); logVA(level, format, args); va_end(args); }
#define SETLOG(str, level) if (!strcmp(name, str)) { LOGLEVEL = level; }

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
    SETLOG("LOGFATAL", LOGFATAL)
    SETLOG("LOGERROR", LOGERROR)
    SETLOG("LOGWARN", LOGWARN)
    SETLOG("LOGTEST", LOGTEST)
    SETLOG("LOGINFO", LOGINFO)
    SETLOG("LOGDEBUG", LOGDEBUG)
    SETLOG("LOGTRACE", LOGTRACE)
    SETLOG("LOGMINUTIA", LOGMINUTIA)
}
