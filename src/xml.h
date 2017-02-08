#ifndef SVG_CONVERTER_XML_H
#define SVG_CONVERTER_XML_H

#include <libxml/xmlerror.h>
#include <libxml/parser.h>

#include <stdexcept>
#include <string>

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

    xmlNodePtr root();
};


#endif  // SVG_CONVERTER_XML_H
