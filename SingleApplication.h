//
// Created by PikachuHy on 2021/4/2.
//

#ifndef MYNOTES_SINGLEAPPLICATION_H
#define MYNOTES_SINGLEAPPLICATION_H

#include <QApplication>
#include <QSharedMemory>
#include <QLocalServer>

#include <QApplication>
#include <QSharedMemory>

class SingleApplication : public QApplication {
Q_OBJECT
public:
    SingleApplication(int &argc, char *argv[], const QString &uniqueKey);

    bool isRunning() const;

    bool sendMessage(const QString &message);

public slots:

    void checkForMessage();

signals:

    void messageAvailable(QString &message);

private:
    bool _isRunning;
    QSharedMemory sharedMemory;
};


#endif //MYNOTES_SINGLEAPPLICATION_H
