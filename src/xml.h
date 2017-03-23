#ifndef SVG_CONVERTER_XML_H
#define SVG_CONVERTER_XML_H

#include <libxml/parser.h>
#include <libxml/xmlerror.h>

#include <memory>
#include <stdexcept>
#include <string>

namespace detail {

/**
 * `std::unique_ptr` Deleter for libxml2 elements.
 */
struct XmlDeleter {
    void operator()(xmlDocPtr doc) const;
};

}  // namespace detail

class XmlDocument {
 private:
    std::unique_ptr<xmlDoc, detail::XmlDeleter> doc_;

 public:
    /**
     * Loads an XML document from the given file.
     */
    explicit XmlDocument(const std::string& filename);

    /**
     * Pointer to the root node of the document.
     *
     * Only valid for the lifetime of the document.
     */
    xmlNodePtr root() const;
};

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

#endif  // SVG_CONVERTER_XML_H
