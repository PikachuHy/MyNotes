#ifndef MYNOTES_TROJANTHREAD_H
#define MYNOTES_TROJANTHREAD_H

#include <QThread>

class Service;

class Config;

class TrojanThread : public QThread {
Q_OBJECT
private:
    Service *service;
    Config *_config;

    void cleanUp();

public:
    TrojanThread(QObject *parent = nullptr);

    bool loadConfig(const QString &path);

    Config &config();

    int localPort();

    ~TrojanThread();

protected:
    void run();

public slots:

    void stop();

signals:

    void started(const bool &sucess);

    void stopped(const bool sucess);

    void exception(const QString &what);
};

#endif // MYNOTES_TROJANTHREAD_H
