#include "TencentScreenShareServer.h"

TencentScreenShareServer::TencentScreenShareServer(QWidget *parent)
    : QWidget(parent)
{
	setWindowFlags(Qt::Tool);
	ui.setupUi(this);
	setFixedSize(1, 1);
	move(0, 0);

	ipcClient = new IPCClient(this);
}