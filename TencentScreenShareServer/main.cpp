#include "TencentScreenShareServer.h"
#include <QtWidgets/QApplication>
#include <QDateTime>
#include "Log.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	HANDLE hModule = CreateMutex(NULL, TRUE, L"RTCTencentScreenShare");
	int nError = GetLastError();
	if (ERROR_ALREADY_EXISTS == nError) {
		return FALSE;
	}

    TencentScreenShareServer w;
    w.show();
    return a.exec();
}
