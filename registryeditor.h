#ifndef REGISTRYEDITOR_H
#define REGISTRYEDITOR_H

#include <QObject>

class QStringList;

/*
  Creates references to files in the registry.
  Registry Editor hides the details about how the key is edited and that
    the .exe is added to the shortcuts in order for them to work.
  */
class RegistryEditor : public QObject
{
    Q_OBJECT

    static const QString registryAppPath;

public:
    explicit RegistryEditor(QObject *parent = 0);

    bool isInRegistry(const QString& shortcut);
    bool isMine(QString shortcut);
    void addShortcut(const QString& shortcut, const QString& file);
    void deleteShortcut(const QString& shortcut);
    void removeAll(QStringList shortcutList);
    void synchronizeRegistry();
    static QString getShortcutAppPath();
};

#endif // REGISTRYEDITOR_H
