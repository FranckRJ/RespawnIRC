#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QMessageBox>

#include "selectTopicWindow.hpp"
#include "parsingTool.hpp"

selectTopicWindowClass::selectTopicWindowClass(QString currentTopic, QWidget* parent) : baseDialogClass(parent)
{
    QLabel* labTopic = new QLabel("Topic :", this);
    QPushButton* buttonSelect = new QPushButton("Choisir ce topic", this);
    QPushButton* buttonCancel = new QPushButton("Annuler", this);
    topicLine = new QLineEdit(this);

    topicLine->setText(currentTopic);
    topicLine->setMinimumWidth(500);

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(buttonSelect);
    buttonLayout->addWidget(buttonCancel);

    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->addWidget(labTopic, 0, 0);
    mainLayout->addWidget(topicLine, 0, 1);
    mainLayout->addLayout(buttonLayout, 1, 0, 1, 2);

    setLayout(mainLayout);
    setWindowTitle("Choisir un topic");
    topicLine->setFocus();

    connect(buttonSelect, &QPushButton::clicked, this, &selectTopicWindowClass::selectThisTopic);
    connect(buttonCancel, &QPushButton::clicked, this, &selectTopicWindowClass::close);
}

QString selectTopicWindowClass::transformLinkIfNeeded(QString link) const
{
    if(link.startsWith("http://") == true)
    {
        link.insert(link.indexOf(":"), 's');
    }

    if(link.startsWith("https://") == false)
    {
        link.insert(0, "https://");
    }

    if(link.startsWith("https://m.") == true)
    {
        link.replace("https://m.", "https://www.");
    }
    else if(link.startsWith("https://jeuxvideo.com/") == true || link.startsWith("https://forumjv.com/") == true)
    {
        link.replace("https://", "https://www.");
    }

    if(link.startsWith("https://www.jeuxvideo.com/forums/") == false && link.startsWith("https://jvforum.fr/") == false
       && link.startsWith("https://www.forumjv.com/forums/") == false)
    {
        return "";
    }

    return link;
}

void selectTopicWindowClass::selectThisTopic()
{
    QString newLink = transformLinkIfNeeded(topicLine->text());

    if(newLink.isEmpty() == false)
    {
        if(newLink.startsWith("https://jvforum.fr/") == true)
        {
            emit newTopicSelected(parsingTool::jvfLinkToJvcLink(newLink));
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
