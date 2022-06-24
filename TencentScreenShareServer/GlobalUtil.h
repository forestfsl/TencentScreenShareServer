
#ifndef WCFPC_GLOBAL_UTIL_H
#define WCFPC_GLOBAL_UTIL_H

#include <QImage>
#include <QDir>
#include <QLayout>
//#include <windows.h>
//#include <stdio.h>

class GlobalUtil
{
public:

	// ÎÄ¼þ²Ù×÷
	static bool ensureDirExist(const QString &dirPath);
};

#endif // !WCFPC_GLOBAL_UTIL_H