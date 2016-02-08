#ifndef AUTOTIMEOUTREPLY_HPP
#define AUTOTIMEOUTREPLY_HPP

#include <QtNetwork>

class autoTimeoutReplyClass : public QObject
{
    Q_OBJECT
public:
    autoTimeoutReplyClass(QObject* parent = 0);
    QNetworkReply* resetReply(QNetworkReply* newReply = nullptr);
public slots:
    void timeoutCurrentReply();
private:
    QNetworkReply* currentReply = nullptr;
    QTimer* timerForTimeout;
};

#endif
