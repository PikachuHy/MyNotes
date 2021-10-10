//
// Created by pikachu on 2021/6/24.
//

#ifndef MYNOTES_BEANFACTORY_H
#define MYNOTES_BEANFACTORY_H
#include <QMap>
#include <QString>
class BeanFactory {
public:
    static BeanFactory *instance();

    template<typename T>
    void registerBean(QString name, T* bean) {
        if (m_beanMap.find(name) != m_beanMap.end()) {
            qFatal("bean %s has been registered", name.toStdString().c_str());
        }
        m_beanMap[name] = static_cast<void*>(bean);
    }

    template<typename T>
    T* getBean(QString name) {
        if (m_beanMap.find(name) == m_beanMap.end()) {
            qFatal("bean %s has not been registered", name.toStdString().c_str());
        }
        return static_cast<T*>(m_beanMap[name]);
    }
private:
    BeanFactory() = default;
    QMap<QString, void*> m_beanMap;
};


#endif //MYNOTES_BEANFACTORY_H
