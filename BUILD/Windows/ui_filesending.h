/********************************************************************************
** Form generated from reading UI file 'filesending.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FILESENDING_H
#define UI_FILESENDING_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_FileSending
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *firstLabel;
    QLabel *fileLabel;
    QLabel *secondLabel;
    QListWidget *listWidget;
    QPushButton *sendButton;

    void setupUi(QDialog *FileSending)
    {
        if (FileSending->objectName().isEmpty())
            FileSending->setObjectName(QStringLiteral("FileSending"));
        FileSending->resize(400, 300);
        verticalLayout = new QVBoxLayout(FileSending);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        firstLabel = new QLabel(FileSending);
        firstLabel->setObjectName(QStringLiteral("firstLabel"));

        verticalLayout->addWidget(firstLabel);

        fileLabel = new QLabel(FileSending);
        fileLabel->setObjectName(QStringLiteral("fileLabel"));

        verticalLayout->addWidget(fileLabel);

        secondLabel = new QLabel(FileSending);
        secondLabel->setObjectName(QStringLiteral("secondLabel"));

        verticalLayout->addWidget(secondLabel);

        listWidget = new QListWidget(FileSending);
        listWidget->setObjectName(QStringLiteral("listWidget"));
        listWidget->setSelectionMode(QAbstractItemView::MultiSelection);

        verticalLayout->addWidget(listWidget);

        sendButton = new QPushButton(FileSending);
        sendButton->setObjectName(QStringLiteral("sendButton"));
        sendButton->setMaximumSize(QSize(100, 16777215));

        verticalLayout->addWidget(sendButton, 0, Qt::AlignHCenter);


        retranslateUi(FileSending);

        QMetaObject::connectSlotsByName(FileSending);
    } // setupUi

    void retranslateUi(QDialog *FileSending)
    {
        FileSending->setWindowTitle(QApplication::translate("FileSending", "Dialog", Q_NULLPTR));
        firstLabel->setText(QApplication::translate("FileSending", "Hai scelto di inviare il file", Q_NULLPTR));
        fileLabel->setText(QString());
        secondLabel->setText(QApplication::translate("FileSending", "Selezionare gli utenti a cui inviare il file", Q_NULLPTR));
        sendButton->setText(QApplication::translate("FileSending", "Invia", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class FileSending: public Ui_FileSending {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FILESENDING_H
