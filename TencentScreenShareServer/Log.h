#pragma once

#include <QMutex>
#include <QString>

static int s_logLevel = 0;
static QMutex s_logMutex;
static QString s_logPath;

void setLogPath(const QString &path);
void setLogLevel(int level);
void customLogMessageHandler(QtMsgType type, const QMessageLogContext& ctx, const QString& msg);

