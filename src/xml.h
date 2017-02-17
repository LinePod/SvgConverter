#ifndef SVG_CONVERTER_XML_H
#define SVG_CONVERTER_XML_H

#include <libxml/parser.h>
#include <libxml/xmlerror.h>

#include <memory>
#include <stdexcept>
#include <string>

/**
 * `std::unique_ptr` Deleter for libxml2 elements.
 */
struct XmlDeleter {
    void operator()(xmlDocPtr doc) const;
};

using ManagedXmlDoc = std::unique_ptr<xmlDoc, XmlDeleter>;

class XmlLoadError : public std::exception {
 private:
    xmlError error_;

 public:
    explicit XmlLoadError(xmlErrorPtr errorPtr);
    ~XmlLoadError() override;

    // xmlError contains allocated memory (message, etc.), therefore
    // this exception should not be copyable.
    XmlLoadError(const XmlLoadError&) = delete;
    XmlLoadError(XmlLoadError&&) = default;
    XmlLoadError& operator=(const XmlLoadError&) = delete;
    XmlLoadError& operator=(XmlLoadError&&) = default;

    const char* what() const noexcept override;
};

ManagedXmlDoc loadDocument(const char* filename);

xmlNodePtr getRoot(const ManagedXmlDoc& doc);

#endif  // SVG_CONVERTER_XML_H
