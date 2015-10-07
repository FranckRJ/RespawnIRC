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

    QObject::connect(buttonSelect, &QPushButton::pressed, this, &selectTopicWindowClass::selectThisTopic);
    QObject::connect(buttonCancel, &QPushButton::pressed, this, &selectTopicWindowClass::close);
}

bool selectTopicWindowClass::linkIsValid(QString link)
{
    if(link.startsWith("http://www.jeuxvideo.com/") == false && link.startsWith("http://jvforum.fr/") == false)
    {
        return false;
    }

    return true;
}

void selectTopicWindowClass::selectThisTopic()
{
    if(linkIsValid(topicLine.text()) == true)
    {
        if(topicLine.text().startsWith("http://jvforum.fr/") == true)
        {
            emit newTopicSelected(parsingToolClass::jvfLinkToJvcLink(topicLine.text()));
        }
        else
        {
            emit newTopicSelected(topicLine.text());
        }
        close();
    }
    else
    {
        QMessageBox messageBox;
        messageBox.warning(this, "Erreur", "Le lien du topic n'est pas valide !");
    }
}
