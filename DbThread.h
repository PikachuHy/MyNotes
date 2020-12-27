//
// Created by PikachuHy on 2020/12/27.
//

#ifndef MYNOTES_DBTHREAD_H
#define MYNOTES_DBTHREAD_H
#include <QThread>

class DbThread : public QThread{
Q_OBJECT
public:
    explicit DbThread(const QString &dataPath, QObject *parent = nullptr);

protected:
    void run() override;

private:
    QString m_dataPath;
};


#endif //MYNOTES_DBTHREAD_H
