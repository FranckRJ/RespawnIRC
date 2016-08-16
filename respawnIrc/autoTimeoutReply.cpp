#include "autoTimeoutReply.hpp"
#include "settingTool.hpp"

autoTimeoutReplyClass::autoTimeoutReplyClass(int newTimeoutTimeInSeconds, QObject* parent) : QObject(parent)
{
    timerForTimeout = new QTimer(this);

    timerForTimeout->setTimerType(Qt::CoarseTimer);
    timerForTimeout->setSingleShot(true);
    updateTimeoutTime(newTimeoutTimeInSeconds);
    timerForTimeout->stop();

    connect(timerForTimeout, &QTimer::timeout, this, &autoTimeoutReplyClass::timeoutCurrentReply);
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

void autoTimeoutReplyClass::updateTimeoutTime(int newTimeoutTimeInSeconds)
{
    if(newTimeoutTimeInSeconds == 0)
    {
        timerForTimeout->setInterval(settingToolClass::getThisIntOption("timeoutInSecond").value * 1000);
    }
    else
    {
        timerForTimeout->setInterval(newTimeoutTimeInSeconds * 1000);
    }
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
