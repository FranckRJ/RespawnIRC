#include <QApplication>
#include <QGuiApplication>
#include <QScreen>
#include <QRect>

#include "baseDialog.hpp"

baseDialogClass::baseDialogClass(QWidget* parent, Qt::WindowFlags newWindowFlags) :
    QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | newWindowFlags)
{
    setAttribute(Qt::WA_DeleteOnClose);
}

/* Cette fonction sert à corriger un bug de Qt 5.9.2 qui affiche des warnings dans le setGeometry(),
   le correctif est d'utiliser une plus grande valeur que width() et height() pour le setGeometry(). */
void baseDialogClass::showEvent(QShowEvent* event)
{
    (void) event;

    if(firstTimeDialogIsShowed == true)
    {
        static const int margin = 15;

        QRect parentWindowGeometry = parentWidget()->window()->geometry();
        QRect screenGeometry = QGuiApplication::primaryScreen()->availableGeometry();
        int dialogWidth = width() + 2;
        int dialogHeight = height() + 2;
        int dialogXPos = parentWindowGeometry.left() + (parentWindowGeometry.width() / 2) - (dialogWidth / 2);
        int dialogYPos = parentWindowGeometry.top() + (parentWindowGeometry.height() / 2) - (dialogHeight / 2);

        if(dialogXPos - margin < screenGeometry.left())
        {
            dialogXPos = screenGeometry.left() + margin;
        }
        else if(dialogXPos + dialogWidth + margin > screenGeometry.left() + screenGeometry.width())
        {
            dialogXPos = screenGeometry.left() + screenGeometry.width() - dialogWidth - margin;
        }

        if(dialogYPos - margin < screenGeometry.top())
        {
            dialogYPos = screenGeometry.top() + margin;
        }
        else if(dialogYPos + dialogHeight + margin > screenGeometry.top() + screenGeometry.height())
        {
            dialogYPos = screenGeometry.top() + screenGeometry.height() - dialogHeight - margin;
        }

        setGeometry(dialogXPos, dialogYPos, dialogWidth, dialogHeight);
        firstTimeDialogIsShowed = false;
    }
}
