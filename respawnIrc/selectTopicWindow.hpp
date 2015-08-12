#ifndef SELECTTOPICWINDOW_HPP
#define SELECTTOPICWINDOW_HPP

#include <QtGui>
#include <QtCore>

class selectTopicWindow : public QDialog
{
    Q_OBJECT
public:
    selectTopicWindow(QString currentTopic, QWidget* parent);
    bool linkIsValid(QString link);
public slots:
    void selectThisTopic();
signals:
    void newTopicSelected(QString newTopic);
private:
    QLineEdit topicLine;
};

#endif
