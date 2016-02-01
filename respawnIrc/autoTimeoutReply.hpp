#ifndef AUTOTIMEOUTREPLY_HPP
#define AUTOTIMEOUTREPLY_HPP

#include <QtNetwork>

class autoTimeoutReplyClass : public QObject
{
    Q_OBJECT
public:
    autoTimeoutReplyClass(QObject* parent = 0);
    QNetworkReply* resetReply(QNetworkReply* newReply = 0);
public slots:
    void timeoutCurrentReply();
private:
    QNetworkReply* currentReply = 0;
    QTimer* timerForTimeout;
};

#endif
