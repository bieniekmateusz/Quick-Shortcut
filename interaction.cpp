#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QObject>

#include "interaction.h"

Interaction::Interaction(QWidget *parent, QIcon argIcon)
{
    shortcutEdit = new QLineEdit;
    returnMessage = new QString("abort");
    msgBox = new QMessageBox;
    msgBox->setWindowIcon(argIcon);
    msgBox->setModal(true);
    addButtonClicked = false;
    dialog = new QDialog(parent);
    dialog->setWindowIcon(argIcon);
    setDialog();
}

Interaction::~Interaction(){
    delete returnMessage;
    delete dialog;
    delete msgBox;
}

void Interaction::setDialog(){
    dialog->setModal(true);
    topLabel = new QLabel;
    topLabel->setWordWrap(true);
    addBut = new QPushButton;
    addBut->setDefault(true);
    QPushButton *cancelBut = new QPushButton("Cancel");

    connect(addBut, SIGNAL(clicked()),  SLOT(addSlot()));
    connect(cancelBut, SIGNAL(clicked()),SLOT(cancelSlot()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(topLabel);
    mainLayout->addWidget(shortcutEdit);

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(cancelBut);
    bottomLayout->addWidget(addBut);

    mainLayout->addLayout(bottomLayout);
    dialog->setLayout(mainLayout);
    dialog->setMinimumSize(400,100);
    dialog->setMaximumSize(400,300);
}

QString Interaction::askForShortcut(QWidget *parent, QString parameter, bool edit){
    /* edit value defines whether the dialog asks about replacment or creation
      of the new shortcut.
      */
    if (edit){
        dialog->setWindowTitle("Replace");
        topLabel->setText("Type in a new shortcut to replace \""
                       + parameter + "\"" );
        addBut->setText("Replace shortcut");
        shortcutEdit->setText(parameter);
    }else{
        dialog->setWindowTitle("Create a new shortcut");
        topLabel->setText("Input the command you would like to use to run \""
                        + parameter + "\"" );
        addBut->setText("Add shortcut");
    }

    dialog->show();

    do{
        *returnMessage = "abort";
        dialog->exec();
        // ask again if the text box was not filled in
        if (shortcutEdit->text().isEmpty() && *returnMessage != "abort"){
            errorShortcutEntered();
        }
    }while(shortcutEdit->text().isEmpty() && *returnMessage != "abort");

    // return abort if the shortcut did not changed
    if (edit && *returnMessage == parameter)
        return "abort";

    // change to lowercase and return
    *returnMessage = returnMessage->toLower();
    return *returnMessage;
}

void Interaction::errorShortcutEntered(){
    msgBox->setWindowTitle("Error");
    msgBox->setText("Shortcut not entered");
    msgBox->setInformativeText("The text box can not be left empty");
    msgBox->setStandardButtons(QMessageBox::Ok);
    msgBox->setIcon(QMessageBox::Warning);
    msgBox->show();
    msgBox->exec();
}

void Interaction::cancelSlot(){
    *returnMessage = "abort";
    dialog->close();
}

void Interaction::addSlot(){
    addButtonClicked = true;
    *returnMessage = shortcutEdit->text();
    dialog->close();
}

void Interaction::closeEvent(QCloseEvent *event){
    if (!addButtonClicked)
        *returnMessage = "abort";
}


int Interaction::msgShortcutExists(){
    msgBox->setWindowTitle("Shortcut exists");
    msgBox->setText("This shortcut already exists");
    msgBox->setInformativeText("Do you want to replace it?");
    msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox->setDefaultButton(QMessageBox::Yes);
    msgBox->setIcon(QMessageBox::Question);
    return msgBox->exec();
}

void Interaction::msgShortcutAdded(QString shortcut){
    msgBox->setWindowTitle("Success!");
    msgBox->setText("Shortcut created successfully!");
    msgBox->setInformativeText("Your shortcut \"" + shortcut +
                              "\" can be used via Run");
    msgBox->setStandardButtons(QMessageBox::Ok);
    msgBox->setIcon(QMessageBox::Information);
    msgBox->exec();
}

void Interaction::msgShortcutError(QString shortcut){
    msgBox->setWindowTitle("Error");
    msgBox->setText("Shortcut can not be created");
    msgBox->setInformativeText("The shortcut \"" + shortcut +
                              "\" is reserved by other software and therefore could not be created");
    msgBox->setStandardButtons(QMessageBox::Ok);
    msgBox->setIcon(QMessageBox::Critical);
    msgBox->exec();
}

void Interaction::msgShortcutInUse(){
    msgBox->setWindowTitle("Error");
    msgBox->setText("Shortcut is in use");
    msgBox->setInformativeText("Remove or change the old shortcut");
    msgBox->setStandardButtons(QMessageBox::Ok);
    msgBox->setIcon(QMessageBox::Critical);
    msgBox->exec();
}

bool Interaction::msgDoYouWantToDelete(QString path, QString oldShortcut){
    msgBox->setWindowTitle("Shortcut exists");
    msgBox->setText("This file has a shortcut \"" + oldShortcut + "\"");
    msgBox->setInformativeText("Do you want to delete this shortcut?");
    msgBox->setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    msgBox->setIcon(QMessageBox::Question);
    msgBox->show();
    int result =  msgBox->exec();

    if (result == QMessageBox::Yes)
        return true;
    else
        return false;
}

void Interaction::shortcutUsedByOtherSoftware(QString shortcut){
    msgBox->setWindowTitle("Error");
    msgBox->setText("Shortcut can not be created");
    msgBox->setInformativeText("Other software started using the \"" + shortcut + "\"");
    msgBox->setStandardButtons(QMessageBox::Ok);
    msgBox->setIcon(QMessageBox::Critical);
    msgBox->exec();
}

void Interaction::noShortcutSelected(){
    msgBox->setWindowTitle("No item selected");
    msgBox->setText("Error");
    msgBox->setInformativeText("Please select a shortcut before using this button");
    msgBox->setStandardButtons(QMessageBox::Ok);
    msgBox->setIcon(QMessageBox::Critical);
    msgBox->exec();
}

void Interaction::shortcutIncorrect(){
    msgBox->setWindowTitle("Error");
    msgBox->setText("Special characters not allowed");
    msgBox->setInformativeText("Your shortcut may contain only letters and numbers without any spaces in between.");
    msgBox->setStandardButtons(QMessageBox::Ok);
    msgBox->setIcon(QMessageBox::Critical);
    msgBox->exec();
}


void Interaction::msgChecker(QString message){
    msgBox->setText("Debug-Checker");
    msgBox->setInformativeText("This is your message: " + message);
    msgBox->setStandardButtons(QMessageBox::Ok);
    msgBox->setIcon(QMessageBox::Critical);
    msgBox->exec();
}

void Interaction::msgChecker( int message,QString msg){
    msgBox->setText("Debug-Checker");
    msgBox->setInformativeText("This is your message: " + msg + " " + QString::number(message));
    msgBox->setStandardButtons(QMessageBox::Ok);
    msgBox->setIcon(QMessageBox::Critical);
    msgBox->exec();
}
