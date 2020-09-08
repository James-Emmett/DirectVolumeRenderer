#pragma once
#include <set>
#include <string>

enum class LogType
{
	Info,
	Warning,
	Error,
	Assert, // special case, only for assert code!
	Fatal,
};

// Log implementation iherit this to subscribe for message notifications
class LogObserver
{
public:
	// Which log level this responds up too, -1 is all
	int logLevel = -1;
	virtual void OutputLog(const std::string& message) = 0;
};

// Handles low-level long handling
class LogHandler
{
private:
	std::set<LogObserver*> m_Observers;

public:
	static LogHandler& Instance();
	void NotifyObservers(LogType type, const char* message);
	static void Subscribe(LogObserver* observer);
	static void Unsubscribe(LogObserver* observer);
	static void WriteMessage(LogType type, char const* file, int line, const std::string& message);
	static void WriteMessage(LogType type, const std::string& message);
};

#if defined(DEBUG)
	#define LogInfo(message)	LogHandler::WriteMessage(LogType::Info, message);
	#define LogWarning(message)	LogHandler::WriteMessage(LogType::Warning, __FILE__, __LINE__, message);
	#define LogError(message)	LogHandler::WriteMessage(LogType::Error, __FILE__, __LINE__, message);
	#define LogFatal(message)	LogHandler::WriteMessage(LogType::Fatal, __FILE__, __LINE__, message);
#else
	#define  LogWarning(message);
	#define  LogError(message);
	#define  LogInfo(message);
#endif


//--Built in Log to File--
class LogFile : public LogObserver
{
private:
	const char* m_FilePath = "LogOutput.txt";
	int m_BufferSize = 5120;
	int m_BufferHead = 0;
	char m_Buffer[5120] = {};

public:
	LogFile(const char* destination = "LogOutput.txt");
	~LogFile();

public:
	void OutputLog(const std::string& message);
	void SetDestination(const char* destination);
	void Flush();
};

//--Built in Log to Message Box--
class LogMessageBox : public LogObserver
{
public:
	LogMessageBox();
	void OutputLog(const std::string& message);
};