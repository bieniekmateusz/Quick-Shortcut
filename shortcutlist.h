#ifndef SHORTCUTLIST_H
#define SHORTCUTLIST_H

#include <QObject>

#include "registryeditor.h"
#include "shortcutitem.h"
#include "shortcutitem.h"

template <class T> class QList;

/*
  This class manages the shortcuts and updates the registry
  and the file with shortcuts

  Keeps records of the shortcuts created and allows retriving them.
  Updates the registry whenever a key is modified, created or deleted.
  */
class ShortcutList : public QObject
{
    Q_OBJECT
public:
    explicit ShortcutList(QObject *parent = 0);
    ~ShortcutList();

    void readFileShortcuts();
    int addShortcut(const QString& shortcut,QString path);
    bool replaceItemByPath(QString path, QString newShortcut);
    int indexOfItemWithPath(QString path);
    int indexOfItemWithShortcut(QString shortcut);
    bool delShortcut(const QString& shortcut);
    bool delShortcut(int index);
    void removeAll();
    int getShortcutCount();
    int isInTheList(QString path);
    int getIndex(QString shortcut);
    QString getShortcut(int index);
    QString getPath(int index);
    void syncRegistry();

private:

    QList<ShortcutItem> shortcuts;
    RegistryEditor *registryEditor;
};

#endif // SHORTCUTLIST_H
