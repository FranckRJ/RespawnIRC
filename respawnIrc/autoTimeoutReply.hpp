#ifndef AUTOTIMEOUTREPLY_HPP
#define AUTOTIMEOUTREPLY_HPP

#include <QtNetwork>

class autoTimeoutReplyClass : public QObject
{
    Q_OBJECT
public:
    autoTimeoutReplyClass();
    QNetworkReply* resetReply(QNetworkReply* newReply = 0);
public slots:
    void timeoutCurrentReply();
private:
    QNetworkReply* currentReply;
    QTimer timerForTimeout;
};

#endif
