#include <QList>
#include <QStringList>

#include "shortcutlist.h"
#include "shortcutitem.h"
#include "fileeditor.h"

ShortcutList::ShortcutList(QObject *parent) :
    QObject(parent)
{
    readFileShortcuts();
    registryEditor = new RegistryEditor;
}

ShortcutList::~ShortcutList(){
    delete registryEditor;
}

/*
  Loads in the shortcuts from the text file.
  */
void ShortcutList::readFileShortcuts(){
    shortcuts.clear();

    QStringList list = FileEditor::readData();


    for (int i = 0 ; i< list.size() ; i++){
        QStringList splited = list.at(i).split("?");
        ShortcutItem item(splited[0],splited[1]);
        shortcuts.append(item);
    }
}

/*
  Adds a shortcut or modifies if one exists
  */
int ShortcutList::addShortcut(const QString &shortcut,QString path){
    path = "\"" + path + "\"";

    // check if the shortcut is in your list,
    // memorize its position
    int index = indexOfItemWithShortcut(shortcut);

    /*if the value is not in your database, check if it exists in the registry
    if it does, then return false as you can not override someone's data.
    If the value is in your database, check if it was not overriden by other software.
    */
    if (index == -1){
        if (registryEditor->isInRegistry(shortcut))
            return -1;
    }else if (!registryEditor->isMine(shortcut)){
            return -2;
    }


    /*modify the shortcut if it exists in the list and
      delete the old registry shortcut.
      if it's a new shortcut, add a new one */
    if(index != -1){
        registryEditor->deleteShortcut(shortcut);
        shortcuts[index].setShortcut(shortcut);
        shortcuts[index].setPath(path);
    }else{
        ShortcutItem item(shortcut,path);
        // the new shortcut must be added in alphabetical order

        if (shortcuts.empty())
            shortcuts.append(item);
        else{
            bool created = false;
            for (int j = 0 ; j < shortcuts.size() ; j++){
                if (shortcuts[j].getShortcut().compare(shortcut) > 0){
                    shortcuts.insert(j,item);
                    created = true;
                    break;
                }
            }

            if(!created)
                shortcuts.append(item);
        }
    }
    // now add the shortcut to the registry
    registryEditor->addShortcut(shortcut,path);

    FileEditor::writeData(shortcuts);
    return 0;
}

int ShortcutList::indexOfItemWithPath(QString path){
    path = "\"" + path + "\"";
    int index = -1;
    for (int i = 0 ; i < shortcuts.count() ; i++){
        if (shortcuts[i].getPath().compare(path) == 0){
            index = i;
            break;
        }
    }

    return index;
}

int ShortcutList::indexOfItemWithShortcut(QString shortcut){
    int index = -1;
    for (int i = 0 ; i < shortcuts.count() ; i++){
        ShortcutItem& ref = shortcuts[i];
        if (ref.getShortcut().compare(shortcut) == 0){
            index = i;
            break;
        }
    }
    return index;
}

bool ShortcutList::replaceItemByPath(QString path, QString newShortcut){
    int itemIndex = indexOfItemWithPath(path);
    if (itemIndex == -1)
        return false;

    if (registryEditor->isInRegistry(newShortcut))
        return false;

    if (registryEditor->isMine(shortcuts[itemIndex].getShortcut()))
        registryEditor->deleteShortcut(shortcuts[itemIndex].getShortcut());

    shortcuts[itemIndex].setShortcut(newShortcut);
    path.append("\"");
    path.insert(0,"\"");
    registryEditor->addShortcut(newShortcut,path);
    FileEditor::writeData(shortcuts);
    return true;
}

bool ShortcutList::delShortcut(int index){
    if (index < 0 || index >= shortcuts.size())
        return false;

    if (registryEditor->isMine(shortcuts[index].getShortcut()))
        registryEditor->deleteShortcut(shortcuts[index].getShortcut());

    shortcuts.removeAt(index);
    FileEditor::writeData(shortcuts);
    return true;
}



/*
  Deletes the shortcut from the registry and then from the list
  */
bool ShortcutList::delShortcut(const QString& shortcut){

    // find in your list and remove
    int index = 0;
    for (int i = 0 ; i < shortcuts.count() ; i++){
        ShortcutItem& ref = shortcuts[i];
        if (ref.getShortcut().compare(shortcut) == 0){
            if (registryEditor->isMine(shortcut))
                registryEditor->deleteShortcut(shortcut);
            shortcuts.removeAt(index);
            FileEditor::writeData(shortcuts);
            return true;
        }
    }


    // the objects was not found in the list
    return false;
}

void ShortcutList::removeAll(){
    QStringList toDel;

    QString elem;
    for(int i = shortcuts.count()-1 ; i>=0 ; i--){
        ShortcutItem item = shortcuts.at(i);
        elem = item.getShortcut();
        toDel.append(elem);
        shortcuts.removeLast();
    }

    registryEditor->removeAll(toDel);
    FileEditor::writeData(shortcuts);
}

int ShortcutList::getShortcutCount(){
    return shortcuts.count();
}

/*
  Searches through the list of shortcuts to find if
  the destination owns one.
  */
int ShortcutList::isInTheList(QString path){
    int count = 0;
    for (int i = 0 ; i < shortcuts.count() ; i++){
        ShortcutItem& ref = shortcuts[i];
        if (ref.getPath().compare(path) == 0){
            count++;
            break;
        }
    }

    return count;
}

/*
  Checks if the shortcuts exists in the list
  */
int ShortcutList::getIndex(QString shortcut){
    int count = 0;
    for (int i = 0 ; i < shortcuts.count() ; i++){
        ShortcutItem& ref = shortcuts[i];
        if (ref.getShortcut().compare(shortcut) == 0){
            count++;
            break;
        }
    }

    return count;
}

/*
  Returns the shortcut at given index
  */
QString ShortcutList::getShortcut(int index){
    if (index < 0 || index > shortcuts.count())
        return NULL;

    return shortcuts[index].getShortcut();
}

/*
  Returns the path at given index
  */
QString ShortcutList::getPath(int index){
    if (index < 0 || index > shortcuts.count())
        return NULL;

    return shortcuts[index].getPath();
}

/*
  Synchronises the registry with the changes in the list
  */
void ShortcutList::syncRegistry(){
    registryEditor->synchronizeRegistry();
}
