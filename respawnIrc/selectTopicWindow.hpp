#ifndef SELECTTOPICWINDOW_HPP
#define SELECTTOPICWINDOW_HPP

#include <QWidget>
#include <QDialog>
#include <QLineEdit>
#include <QString>

class selectTopicWindowClass : public QDialog
{
    Q_OBJECT
public:
    explicit selectTopicWindowClass(QString currentTopic, QWidget* parent);
    QString transformLinkIfNeeded(QString link);
public slots:
    void selectThisTopic();
signals:
    void newTopicSelected(QString newTopic);
private:
    QLineEdit topicLine;
};

#endif
