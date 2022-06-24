
#ifndef TM_DATA_OPERATION_H
#define TM_DATA_OPERATION_H

#include <QByteArray>
#include <QStringList>


class DataOperation
{
public:
	static QByteArray intToData(int value);
	static int intFromData(const QByteArray &data, int index = 0);

	static QString stringFromData(const QByteArray &data, int index = 0);
	static QByteArray stringToData(const QString &data);

	static QByteArray stringListToData(const QStringList &strList);
	static QStringList stringListFromData(const QByteArray &data, int index = 0);

	static QByteArray boolToData(bool value);
	static bool boolFromData(const QByteArray &data, int index = 0);
};

#endif // !TM_DATA_OPERATION_H