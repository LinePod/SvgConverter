#ifndef SVG_CONVERTER_XML_H
#define SVG_CONVERTER_XML_H

#include <stdexcept>
#include <string>
#include <libxml/xmlerror.h>
#include <libxml/parser.h>

class XmlLoadError : public std::runtime_error {
private:
    xmlError error_;

public:
    explicit XmlLoadError(xmlErrorPtr errorPtr);
};

class XmlDocument {
private:
    xmlDocPtr doc_;

public:
    explicit XmlDocument(const char* filename);
    ~XmlDocument();

    xmlNodePtr root() const;
};


#endif //SVG_CONVERTER_XML_H
