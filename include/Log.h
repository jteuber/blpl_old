#pragma once

#include "Pipeline_global.h"

#include <fstream>
#include <string>

namespace blpl {

/**
 * @class	Log
 *
 * @brief	A simple logger class with support for multiple logs and stream
 * style input.
 */
class PIPELINE_EXPORT Log
{
public:
    /**
     * @enum	ErrorLevel
     *
     * @brief	Values that represent the importance of the logged message (aka
     * the error level ;) ).
     */
    enum ErrorLevel
    {
        EL_INFO = 0, ///< Information
        EL_WARNING, ///< Problems or errors that were caught and do not have any
                    ///< impact on the execution of the program
        EL_ERROR,   ///< Errors which could not be dealt with and likely altered
                    ///< the programs behaviour
        EL_FATAL_ERROR ///< Errors that crash the program
    };

    explicit Log(const std::string& strLogName);
    ~Log();

    bool logMessage(const std::string& strMessage,
                    ErrorLevel elErrorLevel = EL_INFO);

    bool logInfo(const std::string& strMessage);
    bool logWarning(const std::string& strMessage);
    bool logError(const std::string& strMessage);
    bool logFatalError(const std::string& strMessage);

    Log& operator<<(ErrorLevel eLvl);
    Log& operator<<(Log& (*logmanipulator)(Log&) );

    Log& operator<<(const char* pc);
    Log& operator<<(const std::string& str);

    Log& operator<<(bool b);
    Log& operator<<(float f);
    Log& operator<<(double d);

    Log& operator<<(short int i);
    Log& operator<<(unsigned short int i);
    Log& operator<<(int i);
    Log& operator<<(unsigned int i);
    Log& operator<<(long long i);
    Log& operator<<(unsigned long long i);

    void flush();

    void setLogLevel(ErrorLevel elLowestLoggedLevel);
    void setSilentMode(bool bSilent);

    bool isSilent();

    static Log& endl(Log& log);
    static Log& flush(Log& log);

private:
    ErrorLevel m_elLowestLoggedLevel;
    ErrorLevel m_elLastStreamLogLvl;
    bool m_bSilent;

    std::string m_strLogText;
    std::ofstream m_logFile;
};

}
