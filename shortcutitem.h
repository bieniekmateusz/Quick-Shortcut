#ifndef SHORTCUTITEM_H
#define SHORTCUTITEM_H

class QString;


class ShortcutItem
{
public:
    ShortcutItem(const QString &Shortcut = "",const QString &Path = "");

    QString getPath();
    QString getShortcut();
    void setPath(const QString& path);
    void setShortcut(const QString& shortcut);
    bool isFolderShortcut();

private:
    QString path;
    QString shortcut;
};

#endif // SHORTCUTITEM_H
