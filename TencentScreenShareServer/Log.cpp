#include "Log.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QDateTime>
#include "GlobalUtil.h"

void setLogPath(const QString & path)
{
	s_logPath = path;
}

void setLogLevel(int level)
{
	s_logLevel = level;
}

void customLogMessageHandler(QtMsgType type, const QMessageLogContext& ctx, const QString& msg)
{
	// 只记录info和warn等业务日志
	if (type != 1 && type != 4)
	{
		return;
	}

	QString logInfo;
	QString logTime = QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss");
	switch (type)
	{
	case QtDebugMsg:
		logInfo = QString("%1 [Debug] %2").arg(logTime, msg);
		break;

	case QtWarningMsg:
		logInfo = QString("%1 [Warning] %2").arg(logTime, msg);
		break;

	case QtCriticalMsg:
		logInfo = QString("%1 [Critical] %2").arg(logTime, msg);
		break;

	case QtFatalMsg:
		logInfo = QString("%1 [Fatal] %2").arg(logTime, msg);
		abort();
	case QtInfoMsg:
		logInfo = QString("%1 [Info] %2").arg(logTime, msg);
		break;
	}

	s_logMutex.lock();
	QFile outFile(s_logPath);
	QFileInfo fileInfo(outFile);
	if (!GlobalUtil::ensureDirExist(fileInfo.absoluteDir().absolutePath()))
	{
		s_logMutex.unlock();
		return;
	}
		
	if (!outFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
	{
		s_logMutex.unlock();
		return;
	}

	QTextStream ts(&outFile);
	ts << logInfo << endl;
	outFile.close();
	s_logMutex.unlock();
}
