#pragma once

#include <QtWidgets/QWidget>
#include "ui_TencentScreenShareServer.h"
#include "..\..\Tencent\include\TRTC\ITRTCCloud.h"
#include "IPCClient.h"

class TencentScreenShareServer : public QWidget
{
    Q_OBJECT

public:
    TencentScreenShareServer(QWidget *parent = Q_NULLPTR);

private:
	Ui::TencentScreenShareServerClass ui;

	IPCClient* ipcClient;
};
