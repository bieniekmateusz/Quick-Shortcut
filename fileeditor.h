#ifndef FILEEDITOR_H
#define FILEEDITOR_H

#include <QList>
#include "shortcutitem.h"

/*
   SAVING AND LOADING SHORTCUTS TO THE PROGRAM,
   IT GETS THE ADDRESS OF THE FILE FROM WHICH TO READ FROM REGISTRY
   THIS CLASS WILL ALSO MANAGE BATCH FILES IN THE FUTURE,
  */
class FileEditor
{

public:

    static QStringList readData();
    static void writeData(QList<ShortcutItem> theList);

};

#endif // FILEEDITOR_H
