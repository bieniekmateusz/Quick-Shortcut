#include <QtGui>
#include <QLocalServer>
#include <QLocalSocket>
#include <QRegExp>

#include "shortcut.h"
#include "shortcutlist.h"
#include "interaction.h"
#include "registryeditor.h"
#include "constants.h"


const QString Shortcut::Connection = "shortcutapp";

Shortcut::Shortcut(int argc, char* argv[]): QMainWindow(0){
    QDir::addSearchPath("icons",RegistryEditor::getShortcutAppPath() + "/graphic");
    shortcutList = new ShortcutList();
    QIcon icon = QIcon("icons:logo.png");
    interaction = new Interaction(this, icon);

    if(argc == 2){
        QString path = argv[1];
        // if the second argument is -removeAll then clean the data
        if (path.compare("-removeAll") == 0){
            shortcutList->removeAll();
        }else
            stealthInteraction(path);

        return;
    }


    // the GUI works so run the server,
    createWindow();
    setUpServer();
}

Shortcut::~Shortcut(){
    delete shortcutList;
    delete interaction;
}


void Shortcut::createWindow(){
    QIcon icon("icons:logo.png");
    setWindowIcon(icon);
    setWindowTitle(SOFTWARE);

    window = new QWidget();
    setCentralWidget(window);

    createWidgets();
    loadListContent();
    setMinimumSize(500,300);
    show();
}

void Shortcut::createWidgets(){

    QVBoxLayout *mainLayout = new QVBoxLayout;
    setUpCentralList();
    mainLayout->addWidget(shortcutTable);
    mainLayout->addLayout(setUpBottomPanel());

    window->setLayout(mainLayout);
}

QHBoxLayout* Shortcut::setUpBottomPanel(){
    QHBoxLayout *bottomPanel = new QHBoxLayout;
    editBut = new QPushButton("Edit Shortcut");
    delBut = new QPushButton("Delete");
    exitBut = new QPushButton("Exit");
    searchBox = new QLineEdit;
    searchBox->setFocus();
    searchBox->setMaximumWidth(120);
    QLabel *searchLabel = new QLabel(tr("Search:"));

    bottomPanel->addWidget(searchLabel);
    bottomPanel->addWidget(searchBox);
    bottomPanel->addStretch(1);
    bottomPanel->addWidget(editBut);
    bottomPanel->addWidget(delBut);
    bottomPanel->addWidget(exitBut);

    bottomPanel->setAlignment(Qt::AlignLeft);
    connect(delBut,SIGNAL(clicked()),this, SLOT(slotDeleteShortcut()));
    connect(exitBut,SIGNAL(clicked()),this, SLOT(slotExit()));
    connect(editBut,SIGNAL(clicked()),this, SLOT(slotEditShortcutButton()));
    connect(searchBox,SIGNAL(textEdited(QString)),this,SLOT(slotFind()));
    connect(searchBox,SIGNAL(textChanged(QString)),this,SLOT(slotSearchBoxTextChanged()));

    return bottomPanel;
}

void Shortcut::setUpCentralList(){
    shortcutTable = new QTableWidget(0,3);

    QStringList columneTitles;
    columneTitles << "Type" << "Shortcut" <<"Path";
    shortcutTable->setHorizontalHeaderLabels(columneTitles);

    shortcutTable->setColumnWidth(0,40);
    shortcutTable->setColumnWidth(1,100);
    shortcutTable->setColumnWidth(2,300);

    shortcutTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    shortcutTable->setSelectionMode(QAbstractItemView::SingleSelection);
    shortcutTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(shortcutTable,SIGNAL(itemDoubleClicked(QTableWidgetItem*)),
            this, SLOT(slotEditShortcutTable(QTableWidgetItem*)));
}

void Shortcut::loadListContent(){
    shortcutTable->clearContents();

    shortcutTable->setRowCount(shortcutList->getShortcutCount());

    QTableWidgetItem *shortcut;
    QTableWidgetItem *path;
    QTableWidgetItem *type;

    for (int i = 0 ; i<shortcutTable->rowCount() ; i++){
        type = new QTableWidgetItem;
        shortcut = new QTableWidgetItem;
        path = new QTableWidgetItem;
        type->setText("File");
        shortcut->setText(shortcutList->getShortcut(i));
        path->setText(shortcutList->getPath(i));

        shortcutTable->setItem(i,0,type);
        shortcutTable->setItem(i,1, shortcut);
        shortcutTable->setItem(i,2, path);
    }

}




void Shortcut::slotCreateMenu()
{
    aboutAction = new QAction(tr("&About"),this);
    aboutAction->setShortcut(tr("Ctrl + A"));
    aboutAction->setStatusTip(tr("More about the application"));


    QLabel label("my label");
    label.setAlignment(Qt::AlignCenter);
    label.setMinimumSize(label.sizeHint());
    statusBar()->addPermanentWidget(&label);
    statusBar()->show();

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAction);
}

void Shortcut::slotShortcutListChanged(){
    // the connection to the server has been made, so the
    // file with shortcuts must be reloaded and displayed
    // which means that the QTableWidget must be refreshed
    shortcutList->readFileShortcuts();
    loadListContent();
}

void Shortcut::keyPressEvent(QKeyEvent *event){
    if (event->key() != Qt::Key_Return)
        return;

    // return if the search box has no focus
    if(!searchBox->hasFocus())
        return;

    QModelIndexList index = shortcutTable->selectionModel()->selectedIndexes();
    if (index.size() == 0)
        return;

    slotEditShortcutButton();
}

void Shortcut::slotSearchBoxTextChanged(){
    if (searchBox->text() != ""){
        return;
    }
    shortcutTable->setCurrentCell(-1,-1);
}

void Shortcut::stealthInteraction(QString path){
    // using the socket we will inform the main application the shortcuts have changed
    setUpSocket();
    int pathIndex = shortcutList->indexOfItemWithPath(path);

    // if there exist a shortcut to the file ask if the user wants to delete it
    bool toDelete = false;
    if (pathIndex != -1)
        toDelete = interaction->msgDoYouWantToDelete
                (path,shortcutList->getShortcut(pathIndex));


    if (toDelete){
        shortcutList->delShortcut(pathIndex);
        signalServer();
        return;
    }
    // do not allow creating shortcuts if one exists
    else if(toDelete == false && pathIndex != -1)
        return;

    // if the user inputs a wrong shortcut, ask again
    QString shortcut;
    do{
        shortcut = interaction->askForShortcut(this,path);
        if (!containsLetterAndNumbersOnly(shortcut)){
            interaction->shortcutIncorrect();
            continue;
        }

        break;
    }while(true);

    if (shortcut.compare("abort") == 0)
        return;


    bool shortcutExist = shortcutList->getIndex(shortcut);
    int answer = 0;
    if (shortcutExist == true)
        answer = interaction->msgShortcutExists();

    // the yes answer means that the shortcut should be replaced
    int addedSuccessfully;
    if (answer == QMessageBox::Yes || answer == 0){
        addedSuccessfully = shortcutList->addShortcut(shortcut, path);
    }
    else if(answer == QMessageBox::Cancel)
        return;

    if (addedSuccessfully == 0){
        interaction->msgShortcutAdded(shortcut);
        signalServer();
        return;
    }else if(addedSuccessfully == -1){
        interaction->msgShortcutError(shortcut);
        return;
    }

    // is this needed here?
    close();
}


/*
  This function starts a server that waits for signals from other proccesses
    of this application.
  If anyone connects to it, it means that the shortcut list was updated.
  */
bool Shortcut::setUpServer(){
    server = new QLocalServer(this);
    if (!server->listen(Connection)){
        QMessageBox::critical(this,tr("Synchronization problem"),
                              tr("Unable to start server : %1.")
                              .arg(server->errorString()));
        close();
        return false;
    }

    connect(server,SIGNAL(newConnection()),this, SLOT(slotShortcutListChanged()));
    return true;
}

/*
  Create a socket but do not make a connection.
  */
void Shortcut::setUpSocket(){
    socket = new QLocalSocket(this);
}

void Shortcut::signalServer(){
    socket->abort();
    socket->connectToServer(Connection);
}

/*
  Check if the input contains number and letter only.
  */
bool Shortcut::containsLetterAndNumbersOnly(QString name){
    QRegExp exp("[a-zA-Z0-9]*");
    return exp.exactMatch(name);
}

void Shortcut::slotFind(){
    if (shortcutTable->rowCount() == 0)
        return;

    for (int i = 0 ; i < shortcutTable->rowCount() ; i++ ){
        QTableWidgetItem *item = shortcutTable->item(i,1);
        // if the item in the table is shorter then the
        // item that the user is looking for,
        // then no element should be selected
        if(searchBox->text().length() > item->text().length()){
            shortcutTable->setCurrentCell(-1,-1);
            return;
        }
        else if (item->text().startsWith(searchBox->text(),Qt::CaseInsensitive)){
            shortcutTable->setCurrentItem(item);
            return;
        }
    }
}

void Shortcut::slotEditShortcutButton(){
    QModelIndexList index = shortcutTable->selectionModel()->selectedIndexes();

    if (index.size() == 0){
        interaction->noShortcutSelected();
        return;
    }

    QTableWidgetItem* shortcutName = shortcutTable->item(index.at(0).row(),1);
    QTableWidgetItem* shortcutPath= shortcutTable->item(index.at(0).row(),2);

    QString path = shortcutPath->text();
    QString oldShortcut = shortcutName->text();

    // remove " from front and end of the string,
    path.remove(0,1);
    path.remove(path.length()-1,1);

    // if the user inputs a wrong shortcut, ask again
    QString shortcut;
    do{
        shortcut = interaction->askForShortcut(this,oldShortcut,true);
        if (!containsLetterAndNumbersOnly(shortcut)){
            interaction->shortcutIncorrect();
            continue;
        }

        break;
    }while(true);

    if (shortcut.compare("abort") == 0)
        return;




    // check if this shortcut exists, inform the user that the shortcut exists,
    if (shortcutList->indexOfItemWithShortcut(shortcut) != -1){
        interaction->msgShortcutInUse();
        return;
    }

    // the yes answer means that the shortcut should be replaced
    int addedSuccessfully = shortcutList->replaceItemByPath(path,shortcut);

    if (addedSuccessfully){
        interaction->msgShortcutAdded(shortcut);
        loadListContent();
        return;
    }else{
        interaction->msgShortcutError(shortcut);
        return;
    }
}

void Shortcut::slotEditShortcutTable(QTableWidgetItem *item){
    // if the user inputs a wrong shortcut, ask again
    QString shortcut;
    do{
        shortcut = interaction->askForShortcut(this,
                shortcutList->getShortcut(item->row()),true);
        if (!containsLetterAndNumbersOnly(shortcut)){
            interaction->shortcutIncorrect();
            continue;
        }

        break;
    }while(true);

    if (shortcut.compare("abort") == 0)
        return;


    // check if this shortcut exists, inform the user that the shortcut exists,
    if (shortcutList->indexOfItemWithShortcut(shortcut) != -1){
        interaction->msgShortcutInUse();
        return;
    }

    // the yes answer means that the shortcut should be replaced
    QString path = shortcutList->getPath(item->row());
    path.remove(0,1);
    path.remove(path.length()-1,1);
    int addedSuccessfully = shortcutList->replaceItemByPath(path,shortcut);

    if (addedSuccessfully){
        interaction->msgShortcutAdded(shortcut);
        loadListContent();
        return;
    }else{
        interaction->msgShortcutError(shortcut);
        return;
    }
}

/*
  Delete a shortcut from the list and the registry
  */
void Shortcut::slotDeleteShortcut(){
    QModelIndexList indexes = shortcutTable->selectionModel()->selectedIndexes();
    if (indexes.size() == 0){
        interaction->noShortcutSelected();
        return;
    }

    shortcutList->delShortcut(indexes.at(0).row());
    shortcutTable->removeRow(indexes.at(0).row());
}

/*
  Exits the program
  */
void Shortcut::slotExit(){
    close();
}
