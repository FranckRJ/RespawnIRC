#ifndef SELECTTOPICWINDOW_HPP
#define SELECTTOPICWINDOW_HPP

#include <QtWidgets>
#include <QtCore>

class selectTopicWindowClass : public QDialog
{
    Q_OBJECT
public:
    selectTopicWindowClass(QString currentTopic, QWidget* parent);
    bool linkIsValid(QString link);
public slots:
    void selectThisTopic();
signals:
    void newTopicSelected(QString newTopic);
private:
    QLineEdit topicLine;
};

#endif
