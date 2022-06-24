#include "DataOperation.h"
#include <QDataStream>


QByteArray DataOperation::intToData(int value)
{
	QByteArray data;
	QDataStream dataStream(&data, QIODevice::WriteOnly);
	dataStream << value;
	return data;
}

int DataOperation::intFromData(const QByteArray &data, int index)
{
	QByteArray tempData = data.right(data.count() - index);
	QDataStream dataStream(&tempData, QIODevice::ReadOnly);
	int result;
	dataStream >> result;
	return result;
}

QString DataOperation::stringFromData(const QByteArray &data, int index /*= 0*/)
{
	QByteArray tempData = data.right(data.count() - index);
	QDataStream dataStream(&tempData, QIODevice::ReadOnly);
	QString result;
	dataStream >> result;
	return result;
}

QByteArray DataOperation::stringToData(const QString &str)
{
	QByteArray data;
	QDataStream dataStream(&data, QIODevice::WriteOnly);
	dataStream << str;
	return data;
}

QByteArray DataOperation::stringListToData(const QStringList &strList)
{
	QByteArray data;
	QDataStream dataStream(&data, QIODevice::WriteOnly);
	dataStream << strList;
	return data;
}

QStringList DataOperation::stringListFromData(const QByteArray &data, int index /* = 0 */)
{
	QByteArray tempData = data.right(data.count() - index);
	QDataStream dataStream(&tempData, QIODevice::ReadOnly);
	QStringList result;
	dataStream >> result;
	return result;
}

QByteArray DataOperation::boolToData(bool value)
{
	QByteArray data;
	QDataStream dataStream(&data, QIODevice::WriteOnly);
	dataStream << value;
	return data;
}

bool DataOperation::boolFromData(const QByteArray &data, int index /* = 0 */)
{
	QByteArray tempData = data.right(data.count() - index);
	QDataStream dataStream(&tempData, QIODevice::ReadOnly);
	bool result;
	dataStream >> result;
	return result;
}