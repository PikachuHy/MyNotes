#ifndef MYNOTES_HTMLEXPORTER_H
#define MYNOTES_HTMLEXPORTER_H

#include <QObject>
#include <QString>
#include "DbModel.h"

class HtmlExporter : public QObject {
    Q_OBJECT
public:
    explicit HtmlExporter(const QString& workshopPath, QObject *parent = nullptr);

    QString generateHtml(const Note& note);
    QString generateHtml(const QString& mdPath, const QString& title);
    void exportToHtml(const Note& note, const QString& dirPath);

private:
    QString noteRealPath(const Note& note) const;
    QString m_workshopPath;
};

#endif
