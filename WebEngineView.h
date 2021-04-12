//
// Created by pikachu on 2021/4/12.
//

#ifndef MYNOTES_WEBENGINEVIEW_H
#define MYNOTES_WEBENGINEVIEW_H

#include <QWebEngineView>
class WebEngineView : public QWebEngineView{
Q_OBJECT
public:
protected:
    QWebEngineView *createWindow(QWebEnginePage::WebWindowType type) override;

private:
};


#endif //MYNOTES_WEBENGINEVIEW_H
