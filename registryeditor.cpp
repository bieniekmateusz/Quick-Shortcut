#include <QSettings>
#include <QStringList>

#include "registryeditor.h"

const QString RegistryEditor::registryAppPath = "HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\App Paths";

RegistryEditor::RegistryEditor(QObject *parent) :
    QObject(parent)
{}

/*
  Check if there is such a key in the "App Paths" key(a.k.a. group)
  */
bool RegistryEditor::isInRegistry(const QString& shortcut)
{
    QSettings settings(registryAppPath , QSettings::NativeFormat);

    QString shortcutExe = shortcut;
    shortcutExe.append(".exe");
    if (settings.childGroups().contains(shortcutExe,Qt::CaseInsensitive))
        return true;
    else
        return false;
}

/*
  Checks whether the shortcut was not overriden by other software.
  */
bool RegistryEditor::isMine(QString shortcut){
    QString shortcutTemp = registryAppPath;
    shortcutTemp.append("\\").append(shortcut).append(".exe");
    QSettings settings(shortcutTemp, QSettings::NativeFormat);
    QString string = settings.value("Default").toString();
    if (string.startsWith("\""))
        return true;


    return false;
}

/*
  Add a key with extension .exe to the registry under the key "App Paths".
  This function also modifies the key if one exists.
  */
void RegistryEditor::addShortcut(const QString& shortcut, const QString& file){
    QString AppPath = registryAppPath;
    QSettings settings(AppPath.append("\\").append(shortcut).append(".exe"),
                       QSettings::NativeFormat);

    settings.setValue("Default",file);
}

/*
  Delete the shortcut's key from under the key "App Paths".
  */
void RegistryEditor::deleteShortcut(const QString& shortcut){
    QString shortcutTemp = shortcut;

    // return if the shortcut does not exist
    if(!isInRegistry(shortcutTemp))
        return;

    QSettings settings(registryAppPath,
                       QSettings::NativeFormat);

    settings.remove(shortcutTemp.append(".exe"));
}

/*
  This is a safe function that removes only the shortcuts
  which belong the this particular application (which were
  not overriden by other software).
  */
void RegistryEditor::removeAll(QStringList shortcutList){
    QString shortcutTemp = registryAppPath;
    shortcutTemp.append("\\");

    QSettings settings(registryAppPath,
                       QSettings::NativeFormat);

    QString temp;
    QString elem;
    for (int i = shortcutList.size()-1 ; i >= 0 ; i--){
        elem = shortcutList.at(i);
        elem = elem.append(".exe");
        temp = shortcutTemp + elem;
        QSettings checker(temp,QSettings::NativeFormat);
        QString string = checker.value("Default").toString();
        if(string.startsWith("\"")){
            // this shortcut belongs to me (starts with " sign)
            // so I can delete it
            settings.remove(elem);
        }
    }
}

/*
  Update the changes to the registry.
  */
void RegistryEditor::synchronizeRegistry(){
    QSettings settings(registryAppPath,
                       QSettings::NativeFormat);
    settings.sync();
}

/*
  Retrieves the path to the file where the shortcuts are stored.
  */
QString RegistryEditor::getShortcutAppPath(){
    QSettings settings("Quick Shortcut","Shortcut Visual Editor");
    return settings.value("Path").toString();
}


