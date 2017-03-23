#include "xml.h"

namespace detail {

void XmlDeleter::operator()(xmlDocPtr doc) const { xmlFreeDoc(doc); }

}  // namespace detail

XmlDocument::XmlDocument(const std::string& filename)
    : doc_{xmlParseFile(filename.c_str())} {
    if (!doc_) {
        throw XmlLoadError{xmlGetLastError()};
    }
}

XmlLoadError::XmlLoadError(xmlErrorPtr errorPtr) {
    xmlCopyError(errorPtr, &error_);
}

xmlNodePtr XmlDocument::root() const {
    return xmlDocGetRootElement(doc_.get());
}

XmlLoadError::~XmlLoadError() { xmlResetError(&error_); }

const char* XmlLoadError::what() const noexcept { return error_.message; }
