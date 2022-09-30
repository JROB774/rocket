#include "cs_debug.hpp"
#include "cs_platform.hpp"
#include "cs_graphics.hpp"

#include <stdarg.h>

#include <map>

using namespace cs;

CS_PRIVATE_SCOPE
{
    struct Logger
    {
        FILE* logFile;

        ~Logger()
        {
            if(logFile)
            {
                fclose(logFile);
                logFile = NULL;
            }
        }
    };

    static bool s_debugRender;
    static Logger s_debugLogger;
}

CS_PUBLIC_SCOPE
{
    CS_API void EnableDebugRender(bool enable)
    {
        #if CS_DEBUG
        s_debugRender = enable;
        #endif // CS_DEBUG
    }

    CS_API bool IsDebugRender()
    {
        #if CS_DEBUG
        return s_debugRender;
        #else
        return false;
        #endif // CS_DEBUG
    }

    CS_API void DebugLog(const char* file, const char* format, ...)
    {
        #if CS_DEBUG

        if(!s_debugLogger.logFile)
        {
            std::string logFileName = GetDataPath() + "debug.log";
            s_debugLogger.logFile = fopen(logFileName.c_str(), "w");
        }

        std::string message;
        va_list(args);
        va_start(args, format);
        s32 length = vsnprintf(NULL, 0, format, args);
        message.resize(length);
        vsnprintf(&message[0], message.length()+1, format, args);
        va_end(args);

        if(s_debugLogger.logFile)
            fprintf(s_debugLogger.logFile, "%s\n", message.c_str());
        fprintf(stdout, "%s\n", message.c_str());

        #endif // CS_DEBUG
    }
}
