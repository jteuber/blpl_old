#include "Log.h"

#include <iostream>

namespace blpl {

std::map<std::string, Log*> Log::sm_mapLogs;
Log* Log::sm_pDefaultLog = nullptr;

const std::string Log::sm_astrLogLevelToPrefix[] = {
    "INFO       : ", "WARNING    : ", "ERROR      : ", "FATAL ERROR: "};

/**
 * @brief	Constructor.
 *
 * @param	strLogName	Name of the log.
 */
Log::Log()
    : m_elLowestLoggedLevel(EL_INFO)
    , m_elLastStreamLogLvl(EL_INFO)
    , m_bSilent(false)
    , m_pLogFile(nullptr)
{}

/**
 * @brief	Destructor.
 */
Log::~Log()
{
    flush();
    if (m_pLogFile)
        fclose(m_pLogFile);
    //	m_fsLogFile.close();
    delete m_pLogFile;
}

void Log::init(const std::string& strLogName)
{
    m_pLogFile = new FILE;
#ifdef WIN32
    fopen_s(&m_pLogFile,
            std::string(strLogName + std::string(".log")).c_str(),
            "w");
#else
    m_pLogFile =
        fopen(std::string(strLogName + std::string(".log")).c_str(), "w");
#endif
}

/**
 * @brief	get the log with the given name.
 *
 * @param	strLogName	Name of the log.
 *
 * @return	null if it fails, else the log.
 */
Log* Log::getLogPtr(const std::string& strLogName)
{
    auto it = sm_mapLogs.find(strLogName);
    if (it == sm_mapLogs.end()) {
        std::pair<std::map<std::string, Log*>::iterator, bool> ret =
            sm_mapLogs.insert(std::make_pair(strLogName, new Log()));
        ret.first->second->init(strLogName);
        return ret.first->second;
    } else
        return it->second;
}

/**
 * @brief	get a reference to the log with the given name.
 *
 * @param	strLogName	Name of the log.
 *
 * @return	null if it fails, else the log.
 */
Log& Log::getLog(const std::string& strLogName)
{
    return *getLogPtr(strLogName);
}

/**
 * @brief	Gets the default log.
 *
 * @return	null if it fails, else the default log.
 */
Log* Log::getDefaultLogPtr()
{
    if (!sm_pDefaultLog)
        sm_pDefaultLog = getLogPtr("default");
    return sm_pDefaultLog;
}

/**
 * @brief	Get a reference to the default log.
 *
 * @return	null if it fails, else the default log.
 */
Log& Log::getDefaultLog()
{
    if (!sm_pDefaultLog)
        sm_pDefaultLog = getLogPtr("default");
    return *sm_pDefaultLog;
}

/**
 * @brief	Sets the name of the default log.
 *
 * @param	strDefaultLogName	The name of the default log.
 */
void Log::setDefaultLog(const std::string& strDefaultLogName)
{
    sm_pDefaultLog = getLogPtr(strDefaultLogName);
}

/**
 * @brief	destroys the multiton, cleaning up all taken memory.
 */
void Log::destroy()
{
    for (auto& it : sm_mapLogs)
        delete it.second;
    sm_mapLogs.clear();
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
bool Log::logMessage(const std::string& strMessage, EErrorLevel elErrorLevel)
{
    if (elErrorLevel < m_elLowestLoggedLevel || elErrorLevel > sm_uiMaxLogLevel)
        return false;

    std::string strPrefix = sm_astrLogLevelToPrefix[elErrorLevel];

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
    if (m_pLogFile) {
        fputs(m_strLogText.c_str(), m_pLogFile);
        fflush(m_pLogFile);

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
void Log::setLogLevel(EErrorLevel elLowestLoggedLevel)
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
Log& Log::operator<<(const EErrorLevel eLvl)
{
    if (eLvl <= sm_uiMaxLogLevel) {
        m_strLogText += sm_astrLogLevelToPrefix[eLvl];
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
    m_strLogText += toString(f);
    return *this;
}

Log& Log::operator<<(const double d)
{
    m_strLogText += toString(d);
    return *this;
}

Log& Log::operator<<(const short int i)
{
    m_strLogText += toString(i);
    return *this;
}

Log& Log::operator<<(const unsigned short int i)
{
    m_strLogText += toString(i);
    return *this;
}

Log& Log::operator<<(const int i)
{
    m_strLogText += toString(i);
    return *this;
}

Log& Log::operator<<(const unsigned int i)
{
    m_strLogText += toString(i);
    return *this;
}

Log& Log::operator<<(const long long i)
{
    m_strLogText += toString(i);
    return *this;
}

Log& Log::operator<<(const unsigned long long i)
{
    m_strLogText += toString(i);
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

template <typename T>
std::string Log::toString(T n)
{
    return std::to_string(n);
}

}
