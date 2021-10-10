//
// Created by pikachu on 2021/6/24.
//

#include "BeanFactory.h"
#include <QDebug>
BeanFactory *BeanFactory::instance() {
    static BeanFactory singleton;
    return &singleton;
}
