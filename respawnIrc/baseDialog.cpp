#include <QApplication>
#include <QDesktopWidget>
#include <QRect>

#include "baseDialog.hpp"

baseDialogClass::baseDialogClass(QWidget* parent) :
    QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
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
        static int margin = 15;

        QRect parentWindowGeometry = parentWidget()->window()->geometry();
        QRect screenGeometry = QApplication::desktop()->screenGeometry();
        int dialogWidth = width() + 2;
        int dialogHeight = height() + 2;
        int dialogXPos = parentWindowGeometry.left() + (parentWindowGeometry.width() / 2) - (dialogWidth / 2);
        int dialogYPos = parentWindowGeometry.top() + (parentWindowGeometry.height() / 2) - (dialogHeight / 2);

        if(dialogXPos < margin)
        {
            dialogXPos = margin;
        }
        else if(dialogXPos + dialogWidth + margin > screenGeometry.width())
        {
            dialogXPos = screenGeometry.width() - dialogWidth - margin;
        }

        if(dialogYPos < margin)
        {
            dialogYPos = margin;
        }
        else if(dialogYPos + dialogHeight + margin > screenGeometry.height())
        {
            dialogYPos = screenGeometry.height() - dialogHeight - margin;
        }

        setGeometry(dialogXPos, dialogYPos, dialogWidth, dialogHeight);
        firstTimeDialogIsShowed = false;
    }
}
