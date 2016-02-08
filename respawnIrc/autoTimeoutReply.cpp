#include "autoTimeoutReply.hpp"

autoTimeoutReplyClass::autoTimeoutReplyClass(QObject* parent) : QObject(parent)
{
    timerForTimeout = new QTimer(this);

    timerForTimeout->setTimerType(Qt::CoarseTimer);
    timerForTimeout->setSingleShot(true);
    timerForTimeout->setInterval(10000);
    timerForTimeout->stop();

    QObject::connect(timerForTimeout, &QTimer::timeout, this, &autoTimeoutReplyClass::timeoutCurrentReply);
}

QNetworkReply* autoTimeoutReplyClass::resetReply(QNetworkReply* newReply)
{
    currentReply = newReply;

    if(currentReply != nullptr)
    {
        timerForTimeout->start();
    }
    else
    {
        timerForTimeout->stop();
    }

    return currentReply;
}

void autoTimeoutReplyClass::timeoutCurrentReply()
{
    if(currentReply != nullptr)
    {
        if(currentReply->isRunning() == true)
        {
            currentReply->abort();
        }
    }
}
