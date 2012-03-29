#include <QFile>
#include <QTextStream>
#include <QStringList>

#include <QList>
#include "shortcutitem.h"
#include "fileeditor.h"
#include "constants.h"
#include "registryeditor.h"

QStringList FileEditor::readData(){
    QStringList list;
    QFile file(RegistryEditor::getShortcutAppPath() + "/data.run");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        return list;
    }

    QTextStream in(&file);
    while(!in.atEnd()){
        QString item = in.readLine();
        list.append(item);
    }
    file.close();

    return list;
}

void FileEditor::writeData(QList<ShortcutItem> theList){
    QFile file(RegistryEditor::getShortcutAppPath() + "/data.run");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);

    for(int i=0 ; i < theList.size() ; i++){
        out <<theList[i].getShortcut() <<"\?" <<theList[i].getPath();
        out <<"\n";
    }
    file.close();
}


