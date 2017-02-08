#include "xml.h"

XmlDocument::XmlDocument(const char* filename)
        : doc_(xmlParseFile(filename)) {
    if (doc_ == nullptr) {
        throw XmlLoadError(xmlGetLastError());
    }
}

XmlDocument::~XmlDocument() {
    xmlFreeDoc(doc_);
    doc_ = nullptr;
}

xmlNodePtr XmlDocument::root() {
    return xmlDocGetRootElement(doc_);
}

XmlLoadError::XmlLoadError(xmlErrorPtr errorPtr)
        : std::runtime_error(std::string(errorPtr->message))  {
}
