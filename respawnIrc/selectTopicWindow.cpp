#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QMessageBox>

#include "selectTopicWindow.hpp"
#include "parsingTool.hpp"

selectTopicWindowClass::selectTopicWindowClass(QString currentTopic, QWidget* parent) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
    setAttribute(Qt::WA_DeleteOnClose);

    QLabel* labTopic = new QLabel("Topic :", this);
    QPushButton* buttonSelect = new QPushButton("Choisir ce topic", this);
    QPushButton* buttonCancel = new QPushButton("Annuler", this);

    topicLine.setText(currentTopic);
    topicLine.setMinimumWidth(500);

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(buttonSelect);
    buttonLayout->addWidget(buttonCancel);

    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->addWidget(labTopic, 0, 0);
    mainLayout->addWidget(&topicLine, 0, 1);
    mainLayout->addLayout(buttonLayout, 1, 0, 1, 2);

    setLayout(mainLayout);
    setWindowTitle("Choisir un topic");
    topicLine.setFocus();

    connect(buttonSelect, &QPushButton::pressed, this, &selectTopicWindowClass::selectThisTopic);
    connect(buttonCancel, &QPushButton::pressed, this, &selectTopicWindowClass::close);
}

QString selectTopicWindowClass::transformLinkIfNeeded(QString link)
{
    if(link.startsWith("https://") == true)
    {
        link.remove(link.indexOf("s"), 1);
    }

    if(link.startsWith("http://") == false)
    {
        link.insert(0, "http://");
    }

    if(link.startsWith("http://m.jeuxvideo.com/") == true)
    {
        link.replace("http://m.jeuxvideo.com/", "http://www.jeuxvideo.com/");
    }
    else if(link.startsWith("http://jeuxvideo.com/") == true)
    {
        link.replace("http://jeuxvideo.com/", "http://www.jeuxvideo.com/");
    }

    if(link.startsWith("http://www.jeuxvideo.com/") == false && link.startsWith("http://jvforum.fr/") == false)
    {
        return "";
    }

    return link;
}

void selectTopicWindowClass::selectThisTopic()
{
    QString newLink = transformLinkIfNeeded(topicLine.text());

    if(newLink.isEmpty() == false)
    {
        if(newLink.startsWith("http://jvforum.fr/") == true)
        {
            emit newTopicSelected(parsingToolClass::jvfLinkToJvcLink(newLink));
        }
        else
        {
            emit newTopicSelected(newLink);
        }
        close();
    }
    else
    {
        QMessageBox::warning(this, "Erreur", "Le lien du topic n'est pas valide !");
    }
}
