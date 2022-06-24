#pragma once

#include <QObject>
#include <QList>
#include <QMap>
#include <QLocalSocket>
#include "..\..\Tencent\include\TRTC\ITRTCCloud.h"

class IPCClient : public QObject, public ITRTCCloudCallback
{
	Q_OBJECT

public:
	IPCClient(QObject *parent = nullptr);
	~IPCClient();

protected:
	virtual void onSpeedTest(const TRTCSpeedTestResult& currentResult, uint32_t finishedCount, uint32_t totalCount) {}
	virtual void onUserVoiceVolume(TRTCVolumeInfo* userVolumes, uint32_t userVolumesCount, uint32_t totalVolume) {}
	virtual void onTestMicVolume(uint32_t volume) {}
	// 主播的进入和离开(类似于onJoinChannelSuccess)
	virtual void onRemoteUserEnterRoom(const char* userId) {}
	virtual void onRemoteUserLeaveRoom(const char* userId, int reason) {}
	virtual void onStatistics(const TRTCStatistics& statis) {}
	virtual void onNetworkQuality(TRTCQualityInfo localQuality, TRTCQualityInfo* remoteQuality, uint32_t remoteQualityCount) {}
	virtual void onError(TXLiteAVError, const char*, void*);
	// 设备状态变化(将摄像头、麦克风和扬声器融合到一起)
	virtual void onDeviceChange(const char* deviceId, TRTCDeviceType type, TRTCDeviceState state) {}
	virtual void onFirstVideoFrame(const char* userId, const TRTCVideoStreamType streamType, const int width, const int height) {}
	virtual void onUserAudioAvailable(const char* userId, bool available) {}
	virtual void onUserVideoAvailable(const char* userId, bool available) {}
	// 非必要回调
	virtual void onWarning(TXLiteAVWarning, const char*, void*) {}
	virtual void onEnterRoom(int) {}
	virtual void onExitRoom(int) {}

private slots:
	void readyRead();

private:
	void cmdProcess(QLocalSocket* socket, const QByteArray& cmdData);
	void joinChannel(QLocalSocket* socket, const QByteArray& cmdData);
	void startPreScreenRectShare(QLocalSocket* socket, const QByteArray& content);
	void startScreenRectShare(QLocalSocket* socket, const QByteArray& content);
	void startWindowShare(QLocalSocket* socket, const QByteArray& content);
	void updateShareParams(QLocalSocket* socket, const QByteArray& content);
	void stopShare(QLocalSocket* socket, const QByteArray& content);
	void leaveChannel(QLocalSocket* socket, const QByteArray& content);
	void updateLoopback(QLocalSocket* socket, const QByteArray& content);
	void shareReply(QLocalSocket* socket, QString message);

private:
	QLocalSocket* m_socket;
	QByteArray m_buffer;
	bool m_isConnected = false;

	ITRTCCloud* m_trctClound;
};
