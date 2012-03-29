#ifndef SHORTCUT_H
#define SHORTCUT_H

#include <QMainWindow>

#define TEST true

class QLineEdit;
class QTableWidget;
class QPushButton;
class QWidget;
class QIcon;
class QHBoxLayout;
class QAction;
class ShortcutList;
class QLocalServer;
class QLocalSocket;
class QSystemSemaphore;
class QTableWidgetItem;
class Interaction;

/*
  This is the main class that uses other classes to manage shortcuts.
  It also displayes the user interface.
  */
class Shortcut : public QMainWindow
{
    Q_OBJECT

    // static variables
    static const QString Connection;

public:
    explicit Shortcut(int argc, char* argv[]);
    ~Shortcut();
    void createWindow();


private slots:
    void slotFind();
    void slotEditShortcutButton();
    void slotEditShortcutTable(QTableWidgetItem *item);
    void slotDeleteShortcut();
    void slotExit();
    void slotCreateMenu();
    void slotShortcutListChanged();
    void keyPressEvent(QKeyEvent *event);
    void slotSearchBoxTextChanged();

private:
    void createWidgets();
    void stealthInteraction(QString path);
    QHBoxLayout* setUpBottomPanel();
    void setUpCentralList();
    void loadListContent();
    void addShortcut();
    bool setUpServer();
    void setUpSocket();
    void signalServer();

    bool containsLetterAndNumbersOnly(QString name);


    QWidget *window;
    QLineEdit *searchBox;
    QTableWidget *shortcutTable;
    QPushButton *editBut;
    QPushButton *delBut;
    QPushButton *exitBut;

    //  Menu Items
    QAction *aboutAction;

    ShortcutList *shortcutList;

    QLocalSocket *socket;
    QLocalServer *server;
    Interaction *interaction;
};

#endif // SHORTCUT_H
