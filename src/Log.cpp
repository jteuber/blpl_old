#include "Log.h"

#include <iostream>

namespace blpl {

namespace {
const char* logLevelToPrefix[] = {
    "INFO       : ", "WARNING    : ", "ERROR      : ", "FATAL ERROR: "};
const unsigned int maxLogLevel = 3;
}

/**
 * @brief	Constructor.
 *
 * @param	strLogName	Name of the log.
 */
Log::Log(const std::string& strLogName)
    : m_elLowestLoggedLevel(EL_INFO)
    , m_elLastStreamLogLvl(EL_INFO)
    , m_bSilent(false)
    , m_logFile(strLogName)
{}

/**
 * @brief	Destructor.
 */
Log::~Log()
{
    flush();
    m_logFile.close();
}

/**
 * @brief	Logs a message as info if no second parameter is given.
 *
 * @note	in debug mode every message will be written to the standard output
 * and directly to the logfile.
 *
 * @param	strMessage 	Message to log
 * @param	iErrorLevel	(optional) error level.
 *
 * @return	true if the message was logged, false if not.
 */
bool Log::logMessage(const std::string& strMessage, ErrorLevel elErrorLevel)
{
    if (elErrorLevel < m_elLowestLoggedLevel || elErrorLevel > maxLogLevel)
        return false;

    std::string strPrefix = logLevelToPrefix[elErrorLevel];

    m_strLogText += strPrefix + strMessage + "\n";

    if (!m_bSilent)
        std::cout << m_strLogText;
    flush();

    return true;
}

/**
 * @brief	Same as logMessage(strInfo) but possibly a little bit faster.
 *
 * @param	strInfo	The info.
 *
 * @return	true if the message was logged, false if not.
 */
bool Log::logInfo(const std::string& strInfo)
{
    if (m_elLowestLoggedLevel > EL_INFO)
        return false;

    m_strLogText += "INFO       : " + strInfo + "\n";

    if (!m_bSilent)
        std::cout << m_strLogText;
    flush();

    return true;
}

/**
 * @brief	Same as logMessage(..., EL_WARNING), but faster and lazier.
 *
 * @param	strWarning	The warning.
 *
 * @return	true if the message was logged, false if not.
 */
bool Log::logWarning(const std::string& strWarning)
{
    if (m_elLowestLoggedLevel > EL_WARNING)
        return false;

    m_strLogText += "WARNING    : " + strWarning + "\n";

    if (!m_bSilent)
        std::cout << m_strLogText;
    flush();

    return true;
}

/**
 * @brief	Same as logMessage(..., EL_ERROR), but faster and lazier.
 *
 * @param	strError	The error message.
 *
 * @return	true if the message was logged, false if not.
 */
bool Log::logError(const std::string& strError)
{
    if (m_elLowestLoggedLevel > EL_ERROR)
        return false;

    m_strLogText += "ERROR      : " + strError + "\n";

    if (!m_bSilent)
        std::cout << m_strLogText;
    flush();

    return true;
}

/**
 * @brief	Same as logMessage(..., EL_FATAL_ERROR), but faster and lazier.
 *
 * @param	strError	The error message.
 *
 * @return	true if the message was logged, false if not.
 */
bool Log::logFatalError(const std::string& strError)
{
    if (m_elLowestLoggedLevel > EL_FATAL_ERROR)
        return false;

    m_strLogText += "FATAL ERROR: " + strError + "\n";

    if (!m_bSilent)
        std::cout << m_strLogText;
    flush();

    return true;
}

/**
 * @brief	writes the current log to the file so that it is safe.
 */
void Log::flush()
{
    //	m_fsLogFile << m_strLogText;
    //	m_fsLogFile << m_ssLogStream.str();
    //	m_fsLogFile.flush();
    if (m_logFile.is_open()) {
        m_logFile.write(m_strLogText.c_str(), m_strLogText.length());
        m_logFile.flush();

        m_strLogText.clear();
    }
}

/**
 * @brief	Sets the lowest message level that will still be logged.
 *
 * @note	This call effects all subsequent calls of the logging methods.
 * 			If you do want to switch of logging pass 4 (EL_FATAL_ERROR+1).
 *
 * @param	elLowestLoggedLevel	The lowest logged message level.
 */
void Log::setLogLevel(ErrorLevel elLowestLoggedLevel)
{
    m_elLowestLoggedLevel = elLowestLoggedLevel;
}

/**
 * @brief	Enables or disables the silent mode, i.e. if true, the log won't
 * write anything to stdout, but still to the file.
 *
 * @note	This call effects all subsequent calls of the logging methods.
 *
 * @param	bSilent	True enables silent mode, false disables it.
 */
void Log::setSilentMode(bool bSilent)
{
    m_bSilent = bSilent;
}

/**
 * @brief	Returns whether this log is in silent mode, i.e. whether it writes
 * messages to stdout.
 *
 * @return	True if this log is in silent mode, false if not.
 */
bool Log::isSilent()
{
    return m_bSilent;
}

/**
 * @brief	Inserts the error level prefix into the log "stream".
 *
 * @param	eLvl	error level to log in the subsequent stream operator calls
 *
 * @return	a reference to this log
 */
Log& Log::operator<<(const ErrorLevel eLvl)
{
    if (eLvl <= maxLogLevel) {
        m_strLogText += logLevelToPrefix[eLvl];
    }

    m_elLastStreamLogLvl = eLvl;

    return *this;
}

/**
 * @brief	Stream operator for function pointers for the manipulators
 *
 * @param	logmanipulator	function pointer to the manipulator, i.e. endl
 *
 * @return	a reference to this log
 */
Log& Log::operator<<(Log& (*logmanipulator)(Log&) )
{
    return std::forward<Log&>(logmanipulator(*this));
}

Log& Log::operator<<(const char* pc)
{
    m_strLogText += pc;
    return *this;
}

Log& Log::operator<<(const std::string& str)
{
    m_strLogText += str;
    return *this;
}

Log& Log::operator<<(const bool b)
{
    m_strLogText += b ? "true" : "false";
    return *this;
}

Log& Log::operator<<(const float f)
{
    m_strLogText += std::to_string(f);
    return *this;
}

Log& Log::operator<<(const double d)
{
    m_strLogText += std::to_string(d);
    return *this;
}

Log& Log::operator<<(const short int i)
{
    m_strLogText += std::to_string(i);
    return *this;
}

Log& Log::operator<<(const unsigned short int i)
{
    m_strLogText += std::to_string(i);
    return *this;
}

Log& Log::operator<<(const int i)
{
    m_strLogText += std::to_string(i);
    return *this;
}

Log& Log::operator<<(const unsigned int i)
{
    m_strLogText += std::to_string(i);
    return *this;
}

Log& Log::operator<<(const long long i)
{
    m_strLogText += std::to_string(i);
    return *this;
}

Log& Log::operator<<(const unsigned long long i)
{
    m_strLogText += std::to_string(i);
    return *this;
}

Log& Log::endl(Log& log)
{
    if (log.m_elLastStreamLogLvl >= log.m_elLowestLoggedLevel) {
        if (!log.m_bSilent)
            std::cout << log.m_strLogText << std::endl;
        log.m_strLogText += '\n';

        log.flush();
    }

    return log;
}

Log& Log::flush(Log& log)
{
    if (log.m_elLastStreamLogLvl >= log.m_elLowestLoggedLevel) {
        if (!log.m_bSilent) {
            std::cout << log.m_strLogText;
            std::cout.flush();
        }

        log.flush();
    }

    return log;
}

}
