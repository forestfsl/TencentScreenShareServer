#include "GlobalUtil.h"

bool GlobalUtil::ensureDirExist(const QString &dirPath)
{
	QDir dir(dirPath);
	if (dir.exists())
	{
		return true;
	}

	return dir.mkpath(dirPath);
}