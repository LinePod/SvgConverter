#include "xml.h"

void XmlDeleter::operator()(xmlDocPtr doc) const { xmlFreeDoc(doc); }

XmlLoadError::XmlLoadError(xmlErrorPtr errorPtr) {
    xmlCopyError(errorPtr, &error_);
}

XmlLoadError::~XmlLoadError() { xmlResetError(&error_); }

const char* XmlLoadError::what() const noexcept { return error_.message; }

ManagedXmlDoc load_document(const char* filename) {
    ManagedXmlDoc doc{xmlParseFile(filename)};
    if (doc == nullptr) {
        throw XmlLoadError{xmlGetLastError()};
    }

    return doc;
}

xmlNodePtr get_root(const ManagedXmlDoc& doc) {
    return xmlDocGetRootElement(doc.get());
}
