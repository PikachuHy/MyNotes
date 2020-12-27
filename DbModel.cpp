//
// Created by PikachuHy on 2020/12/27.
//

#include "DbModel.h"

Note::Note() {}

Note::Note(const QString &strId, const QString &title, int pathId) : m_strId(strId), m_title(title),
                                                                     m_pathId(pathId) {}
