#include "System/Logger.h"
#include "System/Types.h"
#include "System/Assert.h"
#include "System/Time.h"
#include "System/File.h"

std::string logTable[5] =
{
	"[Info]: ",
	"[Warning]: ",
	"[Error]: ",
	"[Assert]: ",
	"[Fatal]: "
};


LogHandler& LogHandler::Instance()
{
	static LogHandler logger;
	return logger;
}

void LogHandler::NotifyObservers(LogType type, const char* message)
{
	for (LogObserver* observer : m_Observers)
	{
		// Only output if you have correct level output!
		if (observer->logLevel == -1 || observer->logLevel >= (int)type)
		{
			observer->OutputLog(message);
		}
	}

	// Special case if its fatal
	if (type == LogType::Fatal)
	{
		// Force crash, although it ends up debug breaking in here :/
		debugBreak();
	}
}

void LogHandler::Subscribe(LogObserver* observer)
{
	if (observer)
	{
		Instance().m_Observers.insert(observer);
	}
}

void LogHandler::Unsubscribe(LogObserver* observer)
{
	if (observer)
	{
		Instance().m_Observers.erase(observer);
	}
}

void LogHandler::WriteMessage(LogType type, char const* file, int line, const std::string& message)
{
	std::string shortFile = file;
	Uint32 start = (Uint32)shortFile.find_last_of("\\");
	shortFile = shortFile.substr((size_t)start + 1);

	std::string outMessage;
	outMessage += " [Date " + Time::CurrentTimeAndDate() + " ]: ";
	outMessage += " " + logTable[(size_t)type];
	outMessage += " [" + shortFile + "] ";
	outMessage += " [Line: " + std::to_string(line) + "]:";
	outMessage += " [Message: " + message + " ]";

	Instance().NotifyObservers(type, outMessage.c_str());
}

void LogHandler::WriteMessage(LogType type, const std::string& message)
{
	std::string outMessage;
	outMessage += " [Date]: " + Time::CurrentTimeAndDate();
	outMessage += " " + logTable[(size_t)type];
	outMessage += " " + message;
	Instance().NotifyObservers(type, outMessage.c_str());
}

void LogHandler::FlushObervers()
{
	std::set<LogObserver*>& observerSet = Instance().m_Observers;

	for (LogObserver* observer : observerSet)
	{
		observer->Flush();
	}
}

LogFile::LogFile(const char* destination)
{
	m_FilePath = destination;
}

LogFile::~LogFile()
{
}

void LogFile::OutputLog(const std::string& message)
{
	int length = (Uint32)message.size() + 1;
	// Fulsh buffer if not enough room
	if (m_BufferHead + length + 1 >= m_BufferSize)
	{
		Flush();
	}

	// Append message
	memcpy(m_Buffer + m_BufferHead, message.c_str(), length);
	m_BufferHead += length;
	m_Buffer[m_BufferHead - 1] = '\n';
}

void LogFile::SetDestination(const char* destination)
{
	m_FilePath = destination;
}

void LogFile::Flush()
{
	File file;
	file.Open(m_FilePath, FileMode::Write);
	file.Write(m_Buffer, m_BufferHead);
	m_BufferHead = 0;
}

LogMessageBox::LogMessageBox()
{
	logLevel = (int)LogType::Error;
}

void LogMessageBox::OutputLog(const std::string& message)
{
	//Window::ShowMessageBox(message);
}
