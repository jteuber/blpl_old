#ifndef LOG_H
#define LOG_H

#include "Pipeline_global.h"

#include <cstdio> // for FILE
#include <map>
#include <string>

/**
 * @class	Log
 *
 * @brief	A simple logger class with support for multiple logs and stream
 * style input.
 */
class PIPELINE_EXPORT Log {

public:
  /**
   * @enum	EErrorLevel
   *
   * @brief	Values that represent the importance of the logged message (aka
   * the error level ;) ).
   */
    enum EErrorLevel
    {
        EL_INFO = 0, ///< Information
        EL_WARNING, ///< Problems or errors that were caught and do not have any
                    ///< impact on the execution of the program
        EL_ERROR,   ///< Errors which could not be dealt with and likely altered
                  ///< the programs behaviour
        EL_FATAL_ERROR ///< Errors that crash the program
    };

  static Log *getLogPtr(const std::string &strLogName);
  static Log &getLog(const std::string &strLogName);

  static Log *getDefaultLogPtr();
  static Log &getDefaultLog();

  static void setDefaultLog(const std::string &strDefaultLogName);
  static void destroy();

  bool logMessage(const std::string &strMessage,
                  EErrorLevel elErrorLevel = EL_INFO);

  bool logInfo(const std::string &strMessage);
  bool logWarning(const std::string& strMessage);
  bool logError(const std::string& strMessage);
  bool logFatalError(const std::string& strMessage);

  Log& operator<<(EErrorLevel eLvl);
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

  void setLogLevel(EErrorLevel elLowestLoggedLevel);
  void setSilentMode(bool bSilent);

  bool isSilent();

  ~Log();

  static Log &endl(Log &log);
  static Log &flush(Log &log);

private:
  explicit Log();

  void init(const std::string& strLogName);

  template <typename T> static std::string toString(T n);

private:
  EErrorLevel m_elLowestLoggedLevel;
  EErrorLevel m_elLastStreamLogLvl;
  bool m_bSilent;

  std::string m_strLogText;
  FILE *m_pLogFile;

  static std::map<std::string, Log *> sm_mapLogs;
  static Log *sm_pDefaultLog;

  static const std::string sm_astrLogLevelToPrefix[];
  static const unsigned int sm_uiMaxLogLevel = 3;
};

#ifdef __GNUG__
#define LOGFILELINEFUNC                                                        \
  "function \"" << __PRETTY_FUNCTION__ << "\" in file \"" __FILE__             \
                << "\", line " << __LINE__
#else
#define LOGFILELINEFUNC                                                        \
  "function \"" << __func__ << "\" in file \"" __FILE__ << "\", line "         \
                << __LINE__
#endif

#endif // LOG_H
