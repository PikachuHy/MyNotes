//
// Created by PikachuHy on 2021/1/9.
//

#include "MdToHtml.h"
#include "maddy/parser.h"
#include <iostream>
#include <memory>
#include <string>

QString MdToHtml::toHtml(const QString &mdText) {
    std::stringstream markdownInput(mdText.toStdString());
    // config is optional
    std::shared_ptr<maddy::ParserConfig> config = std::make_shared<maddy::ParserConfig>();
    config->isEmphasizedParserEnabled = true; // default
    config->isHTMLWrappedInParagraph = true; // default

    std::shared_ptr<maddy::Parser> parser = std::make_shared<maddy::Parser>(config);
    std::string htmlOutput = parser->Parse(markdownInput);

    return QString::fromStdString(htmlOutput);
}
