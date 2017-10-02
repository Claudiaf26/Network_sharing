/********************************************************************************
** Form generated from reading UI file 'settings.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGS_H
#define UI_SETTINGS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Settings
{
public:
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_4;
    QVBoxLayout *verticalLayout_2;
    QLabel *connectedLabel;
    QListWidget *listWidget;
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    QLabel *usernameLabel;
    QLineEdit *usernameEdit;
    QLabel *directoryLabel;
    QHBoxLayout *horizontalLayout;
    QLineEdit *directoryEdit;
    QPushButton *fileButton;
    QCheckBox *privateBox;
    QCheckBox *notificationBox;
    QCheckBox *automaticBox;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *okButton;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QWidget *Settings)
    {
        if (Settings->objectName().isEmpty())
            Settings->setObjectName(QStringLiteral("Settings"));
        Settings->resize(628, 264);
        Settings->setLayoutDirection(Qt::LeftToRight);
        verticalLayout_3 = new QVBoxLayout(Settings);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        connectedLabel = new QLabel(Settings);
        connectedLabel->setObjectName(QStringLiteral("connectedLabel"));

        verticalLayout_2->addWidget(connectedLabel);

        listWidget = new QListWidget(Settings);
        listWidget->setObjectName(QStringLiteral("listWidget"));
        listWidget->setSelectionMode(QAbstractItemView::MultiSelection);

        verticalLayout_2->addWidget(listWidget);


        horizontalLayout_4->addLayout(verticalLayout_2);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        usernameLabel = new QLabel(Settings);
        usernameLabel->setObjectName(QStringLiteral("usernameLabel"));

        gridLayout->addWidget(usernameLabel, 0, 0, 1, 1);

        usernameEdit = new QLineEdit(Settings);
        usernameEdit->setObjectName(QStringLiteral("usernameEdit"));
        usernameEdit->setMaxLength(256);
        usernameEdit->setReadOnly(false);

        gridLayout->addWidget(usernameEdit, 0, 1, 1, 1);

        directoryLabel = new QLabel(Settings);
        directoryLabel->setObjectName(QStringLiteral("directoryLabel"));

        gridLayout->addWidget(directoryLabel, 1, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        directoryEdit = new QLineEdit(Settings);
        directoryEdit->setObjectName(QStringLiteral("directoryEdit"));
        directoryEdit->setReadOnly(true);

        horizontalLayout->addWidget(directoryEdit);

        fileButton = new QPushButton(Settings);
        fileButton->setObjectName(QStringLiteral("fileButton"));

        horizontalLayout->addWidget(fileButton);


        gridLayout->addLayout(horizontalLayout, 1, 1, 1, 1);


        verticalLayout->addLayout(gridLayout);

        privateBox = new QCheckBox(Settings);
        privateBox->setObjectName(QStringLiteral("privateBox"));
        privateBox->setEnabled(true);
        privateBox->setTristate(false);

        verticalLayout->addWidget(privateBox);

        notificationBox = new QCheckBox(Settings);
        notificationBox->setObjectName(QStringLiteral("notificationBox"));

        verticalLayout->addWidget(notificationBox);

        automaticBox = new QCheckBox(Settings);
        automaticBox->setObjectName(QStringLiteral("automaticBox"));

        verticalLayout->addWidget(automaticBox);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        horizontalLayout_4->addLayout(verticalLayout);


        verticalLayout_3->addLayout(horizontalLayout_4);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        okButton = new QPushButton(Settings);
        okButton->setObjectName(QStringLiteral("okButton"));
        okButton->setMaximumSize(QSize(80, 23));

        horizontalLayout_2->addWidget(okButton);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        verticalLayout_3->addLayout(horizontalLayout_2);


        retranslateUi(Settings);

        QMetaObject::connectSlotsByName(Settings);
    } // setupUi

    void retranslateUi(QWidget *Settings)
    {
        Settings->setWindowTitle(QApplication::translate("Settings", "FileSender", Q_NULLPTR));
        connectedLabel->setText(QApplication::translate("Settings", "Utenti connessi:", Q_NULLPTR));
        usernameLabel->setText(QApplication::translate("Settings", "Nome Utente:", Q_NULLPTR));
        directoryLabel->setText(QApplication::translate("Settings", "Cartella  di destinazione:", Q_NULLPTR));
        fileButton->setText(QApplication::translate("Settings", "F", Q_NULLPTR));
        privateBox->setText(QApplication::translate("Settings", "Modalit\303\240 privata", Q_NULLPTR));
        notificationBox->setText(QApplication::translate("Settings", "Non mostrare notifiche", Q_NULLPTR));
        automaticBox->setText(QApplication::translate("Settings", "Ricezione automatica", Q_NULLPTR));
        okButton->setText(QApplication::translate("Settings", "Start", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Settings: public Ui_Settings {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGS_H
