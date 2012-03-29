#include <QString>
#include "shortcutitem.h"

ShortcutItem::ShortcutItem(const QString &shortcut, const QString &path)
    :path(path), shortcut(shortcut)
{

}


QString ShortcutItem::getPath(){
    return path;
}

QString ShortcutItem::getShortcut(){
    return shortcut;
}

void ShortcutItem::setPath(const QString& path){
    this->path = path;
}

void ShortcutItem::setShortcut(const QString& shortcut){
    this->shortcut = shortcut;
}

bool ShortcutItem::isFolderShortcut(){
    /*
      CHECK WHETHER THE EXTENSION IS .BAT, IF YES THEN RETURN TRUE,
      ... OTHERWISE FALSE
      */
    return false;
}
