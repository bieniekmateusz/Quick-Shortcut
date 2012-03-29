#ifndef INTERACTION_H
#define INTERACTION_H

#include <QDialog>
class QIcon;
class QLineEdit;
class QMessageBox;
class QLabel;

/*
  Main interaction with the user,
  TODO This class should be changed as it's messy,
  */
class Interaction:QObject
{
    Q_OBJECT
public:
    explicit Interaction(QWidget *parent, QIcon icon);
    ~Interaction();
    void closeEvent(QCloseEvent *event);



    QString askForShortcut(QWidget *parent,QString pathToFile, bool edit = false);


    int msgShortcutExists();
    void msgShortcutAdded(QString shortcut);
    void msgShortcutError(QString shortcut);
    void msgShortcutInUse();
    bool msgDoYouWantToDelete(QString path, QString oldShortcut);
    void shortcutUsedByOtherSoftware(QString path);
    void noShortcutSelected();
    void shortcutIncorrect();
    void msgChecker(QString message);
    void msgChecker( int message, QString msg = "");
signals:

private slots:
    void cancelSlot();
    void addSlot();

private:
    void errorShortcutEntered();
    void setDialog();

    QMessageBox *msgBox;
    QString *returnMessage;
    bool addButtonClicked;
    QLineEdit *shortcutEdit;
    QDialog *dialog;

    // dialog's widgets:
    QLabel *topLabel;
    QPushButton *addBut;
};

#endif // INTERACTION_H
