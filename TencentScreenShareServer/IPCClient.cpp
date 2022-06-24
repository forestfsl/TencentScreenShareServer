#include "IPCClient.h"
#include <QCoreApplication>
#include <QProcess>
#include <QLocalServer>
#include "DataOperation.h"
#include "..\..\..\HappyDrawing\HappyDrawing\EventDef.h"
#include <QJsonDocument>
#include <QApplication>
#include <QDesktopWidget>

IPCClient::IPCClient(QObject *parent)
	: QObject(parent)
{
	qRegisterMetaType<void*>("void*");

	// 初始化腾讯
	m_trctClound = getTRTCShareInstance();
	if (m_trctClound)
	{
		m_trctClound->addCallback(this);
		m_trctClound->muteLocalAudio(true);
		m_trctClound->muteAllRemoteAudio(true);
		m_trctClound->muteAllRemoteVideoStreams(true);
		m_trctClound->stopLocalPreview();
	}

	m_socket = new QLocalSocket(this);
	connect(m_socket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
	connect(m_socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
	connect(m_socket, &QLocalSocket::connected, [this]()
		{
			m_isConnected = true;
		});
	connect(m_socket, &QLocalSocket::errorOccurred, [this](QLocalSocket::LocalSocketError socketError)
		{
			m_isConnected = false;
		});
	connect(m_socket, &QLocalSocket::disconnected, [this]()
		{
			m_isConnected = false;
		});
	m_socket->abort();
	m_socket->connectToServer("RTC_ScreenShare");
}

IPCClient::~IPCClient()
{
	m_socket->abort();
	m_socket->disconnectFromServer();
}

void IPCClient::onError(TXLiteAVError errCode, const char* errMsg, void* extraInfo)
{
	shareReply(m_socket, QString("TencentScreenShareServer : joinChannel result : %1, errMsg: %2").arg(errCode).arg(QString::fromUtf8(errMsg)));
}

void IPCClient::readyRead()
{
	m_buffer += m_socket->readAll();
	while (m_buffer.count() > 4)
	{
		int cmdLen = DataOperation::intFromData(m_buffer);
		if (m_buffer.count() >= cmdLen)
		{
			QByteArray cmdData = m_buffer.left(cmdLen);
			// 命令处理
			if (cmdData.count() > 0)
			{
				cmdProcess(m_socket, cmdData);
			}
			m_buffer = m_buffer.right(m_buffer.count() - cmdLen);
		}
		else
		{
			break;
		}
	}
}

void IPCClient::cmdProcess(QLocalSocket* socket, const QByteArray& cmdData)
{
	int cmdType = DataOperation::intFromData(cmdData, 4);
	switch (cmdType)
	{
	case CMD_JOIN_SCREEN_SHARE_CHANNEL:
		joinChannel(socket, cmdData.right(cmdData.count() - 8));
		break;

	case CMD_LEAVE_SCREEN_SHARE_CHANNEL:
		leaveChannel(socket, cmdData.right(cmdData.count() - 8));
		break;

	case CMD_PRE_START_SCREEN_SHARE_AGORA:
		startPreScreenRectShare(socket, cmdData.right(cmdData.count() - 8));
		break;

	case CMD_START_SCREEN_SHARE:
		startScreenRectShare(socket, cmdData.right(cmdData.count() - 8));
		break;

	case CMD_START_WINDOW_SHARE:
		startWindowShare(socket, cmdData.right(cmdData.count() - 8));
		break;

	case CMD_UPDATE_SHARE_PARAMS:
		updateShareParams(socket, cmdData.right(cmdData.count() - 8));
		break;

	case CMD_STOP_SCREEN_SHARE:
		stopShare(socket, cmdData.right(cmdData.count() - 8));
		break;

	case CMD_LOOPBACK_RECORDING:
		updateLoopback(socket, cmdData.right(cmdData.count() - 8));
		break;
	default:
		break;
	}
}

void IPCClient::joinChannel(QLocalSocket* socket, const QByteArray& content)
{
	QString data = DataOperation::stringFromData(content);
	QVariantMap dataMap = QJsonDocument::fromJson(data.toLocal8Bit()).toVariant().toMap();

	int type = dataMap["type"].toInt();
	QString token = dataMap["token"].toString();
	QString channelId = dataMap["channelId"].toString();
	QString info = dataMap["info"].toString();
	int appId = dataMap["appId"].toInt();
	QString uid = dataMap["uid"].toString();
	TRTCParams params;
	params.sdkAppId = appId;
	params.userId = uid.toUtf8();
	params.role = TRTCRoleAnchor;
	params.roomId = 0;
	params.strRoomId = channelId.toUtf8();
	params.userSig = token.toUtf8();
	m_trctClound->enterRoom(params, type == 0 ? TRTCAppSceneVideoCall : TRTCAppSceneLIVE);

	shareReply(socket, QString("TencentScreenShareServer : joinChannel result : 0"));
}

void IPCClient::startPreScreenRectShare(QLocalSocket* socket, const QByteArray& content)
{
	QString data = DataOperation::stringFromData(content);
	QVariantMap dataMap = QJsonDocument::fromJson(data.toLocal8Bit()).toVariant().toMap();

	// 设置窗口信息
	QDesktopWidget* desktopWidget = QApplication::desktop();
	TRTCScreenCaptureSourceInfo source;
	source.type = TRTCScreenCaptureSourceTypeScreen;
	source.sourceId = (HWND)desktopWidget->winId();
	RECT captureRect;
	captureRect.left = dataMap["x"].toInt();
	captureRect.top = dataMap["y"].toInt();
	captureRect.right = dataMap["x"].toInt() + dataMap["width"].toInt();
	captureRect.bottom = dataMap["y"].toInt() + dataMap["height"].toInt();
	TRTCScreenCaptureProperty property;
	property.enableCaptureMouse = true;
	property.enableHighLight = false;
	property.enableHighPerformance = false;
	m_trctClound->selectScreenCaptureTarget(source, captureRect, property);
	// 开启推流
	TRTCVideoEncParam* params = new TRTCVideoEncParam;
	params->enableAdjustRes = true;
	params->videoFps = dataMap["frameRate"].toInt();
	m_trctClound->startScreenCapture(nullptr, TRTCVideoStreamTypeBig, params);

	shareReply(socket, QString("TencentScreenShareServer : startPreScreenCaptureByScreenRect result : 0"));
}

void IPCClient::startScreenRectShare(QLocalSocket* socket, const QByteArray& content)
{
	QString data = DataOperation::stringFromData(content);
	QVariantMap dataMap = QJsonDocument::fromJson(data.toLocal8Bit()).toVariant().toMap();

	// 设置窗口信息
	QDesktopWidget* desktopWidget = QApplication::desktop();
	TRTCScreenCaptureSourceInfo source;
	source.type = TRTCScreenCaptureSourceTypeScreen;
	source.sourceId = (HWND)desktopWidget->winId();
	RECT captureRect;
	captureRect.left = dataMap["x"].toInt();
	captureRect.top = dataMap["y"].toInt();
	captureRect.right = dataMap["x"].toInt() + dataMap["width"].toInt();
	captureRect.bottom = dataMap["y"].toInt() + dataMap["height"].toInt();
	TRTCScreenCaptureProperty property;
	property.enableCaptureMouse = true;
	property.enableHighLight = false;
	property.enableHighPerformance = false;
	m_trctClound->selectScreenCaptureTarget(source, captureRect, property);
	// 开启推流
	TRTCVideoEncParam* params = new TRTCVideoEncParam;
	params->enableAdjustRes = true;
	params->videoFps = dataMap["frameRate"].toInt();
	m_trctClound->startScreenCapture(nullptr, TRTCVideoStreamTypeBig, params);

	shareReply(socket, QString("TencentScreenShareServer : startScreenCaptureByScreenRect result : 0"));
}

void IPCClient::startWindowShare(QLocalSocket* socket, const QByteArray& content)
{
	QString data = DataOperation::stringFromData(content);
	QVariantMap dataMap = QJsonDocument::fromJson(data.toLocal8Bit()).toVariant().toMap();
	
	// 设置窗口信息
	TRTCScreenCaptureSourceInfo source;
	source.type = TRTCScreenCaptureSourceTypeWindow;
	source.sourceId = (HWND)dataMap["HWND"].toLongLong();
	RECT captureRect;
	captureRect.left = dataMap["xOffset"].toInt();
	captureRect.top = dataMap["yOffset"].toInt();
	captureRect.right = dataMap["xOffset"].toInt() + dataMap["shareWidth"].toInt();
	captureRect.bottom = dataMap["yOffset"].toInt() + dataMap["shareHeight"].toInt();
	TRTCScreenCaptureProperty property;
	property.enableCaptureMouse = true;
	property.enableHighLight = false;
	property.enableHighPerformance = false;
	m_trctClound->selectScreenCaptureTarget(source, captureRect, property);
	// 开启推流
	TRTCVideoEncParam params;
	params.enableAdjustRes = true;
	params.videoFps = dataMap["frameRate"].toInt();

	m_trctClound->startScreenCapture(nullptr, TRTCVideoStreamTypeBig, &params);

	shareReply(socket, QString("TencentScreenShareServer : startScreenCaptureByWindowId result : 0"));
}

void IPCClient::updateShareParams(QLocalSocket* socket, const QByteArray& content)
{
	QString data = DataOperation::stringFromData(content);
	QVariantMap dataMap = QJsonDocument::fromJson(data.toLocal8Bit()).toVariant().toMap();
	
	TRTCVideoEncParam* params = new TRTCVideoEncParam;
	params->enableAdjustRes = true;
	params->videoFps = dataMap["frameRate"].toInt();
	m_trctClound->startScreenCapture(nullptr, TRTCVideoStreamTypeBig, params);

	shareReply(socket, QString("TencentScreenShareServer : updateScreenCaptureParameters result : 0"));
}

void IPCClient::stopShare(QLocalSocket* socket, const QByteArray& content)
{
	m_trctClound->stopScreenCapture();
	shareReply(socket, QString("TencentScreenShareServer : stopScreenCapture result : 0"));
}

void IPCClient::leaveChannel(QLocalSocket* socket, const QByteArray& content)
{
	if (m_trctClound)
	{
		m_trctClound->stopScreenCapture();
		m_trctClound->exitRoom();
	}
	shareReply(socket, QString("TencentScreenShareServer : leaveChannel result : 0"));
}

void IPCClient::updateLoopback(QLocalSocket* socket, const QByteArray& content)
{
	bool enable = DataOperation::boolFromData(content);
	if (m_trctClound)
	{
		if (enable)
		{
			m_trctClound->startSystemAudioLoopback();
		}
		else
		{
			m_trctClound->stopSystemAudioLoopback();
		}
	}
	shareReply(socket, QString("TencentScreenShareServer : enableLoopbackRecording result : 0"));
}

void IPCClient::shareReply(QLocalSocket* socket, QString message)
{
	if ((nullptr != socket) && (QAbstractSocket::ConnectedState == socket->state()))
	{
		QByteArray data = DataOperation::stringToData(message);
		data = DataOperation::intToData(RESPONSE_SHARE) + data;
		data = DataOperation::intToData(data.count() + 4) + data;
		socket->write(data);
	}
}
