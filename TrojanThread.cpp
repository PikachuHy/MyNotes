#include "TrojanThread.h"
#include <string>
#include "trojan/src/core/log.h"
#include "trojan/src/core/service.h"
#include "trojan/src/core/config.h"
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QDir>

void TrojanThread::cleanUp() {
    delete service;
    service = nullptr;
    delete _config;
    _config = new Config();
}

TrojanThread::TrojanThread(QObject *parent) :
        QThread(parent),
        service(nullptr),
        _config(new Config()) {

#ifndef _DEBUG
    auto docPath = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).first();
    auto logPath = docPath + "/MyNotes/logs";
    QString logFile = logPath + "/trojan.txt";
    Log::redirect(logFile.toStdString());
#endif
}

Config &TrojanThread::config() {
    return *_config;
}

int TrojanThread::localPort() {
    return _config->local_port;
}

TrojanThread::~TrojanThread() {
    stop();
    delete _config;
}

void TrojanThread::stop() {
    qInfo() << "stop trojan thread";
    if (isRunning() && service) {
        service->stop();
        wait();
        cleanUp();
        emit stopped(true);
    } else {
        emit stopped(false);
    }
}

void TrojanThread::run() {
    try {
        qInfo() << "run trojan thread";
        service = new Service(*_config);
        emit started(true);
        service->run();
    } catch (const std::exception &e) {
        qCritical() << e.what();
        // Log::log_with_date_time(std::string("fatal: ") + e.what(), Log::FATAL);
        cleanUp();
        emit exception(QString(e.what()));
    }
}

bool TrojanThread::loadConfig(const QString &path) {
    QFile file(path);
    if (!file.exists()) {
        qWarning() << "trojan config file not exist:" << path;
        return false;
    }
    bool ok = file.open(QIODevice::ReadOnly);
    if (!ok) {
        qWarning() << "file open fail:" << path;
        return false;
    }
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isEmpty()) {
        qWarning() << "trojan config file is empty:" << path;
        return false;
    }
    QJsonObject o = doc.object();
    QString runType = o.value("run_type").toString();
    QString localAddr = o.value("local_addr").toString();
    int localPort = o.value("local_port").toInt();
    QString remoteAddr = o.value("remote_addr").toString();
    int remotePort = o.value("remote_port").toInt();
    if (localAddr.isEmpty()) {
        qWarning() << "local address is empty.";
        return false;
    }
    if (localPort == 0) {
        qWarning() << "local port is 0.";
        return false;
    }
    if (remoteAddr.isEmpty()) {
        qWarning() << "remote address is empty.";
        return false;
    }
    if (remotePort == 0) {
        qWarning() << "remote port is 0.";
        return false;
    }
    _config->load(path.toStdString());
}
