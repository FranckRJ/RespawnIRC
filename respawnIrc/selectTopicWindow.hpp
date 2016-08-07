#ifndef SELECTTOPICWINDOW_HPP
#define SELECTTOPICWINDOW_HPP

#include <QDialog>
#include <QLineEdit>

class selectTopicWindowClass : public QDialog
{
    Q_OBJECT
public:
    selectTopicWindowClass(QString currentTopic, QWidget* parent);
    QString transformLinkIfNeeded(QString link);
public slots:
    void selectThisTopic();
signals:
    void newTopicSelected(QString newTopic);
private:
    QLineEdit topicLine;
};

#endif
