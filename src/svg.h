#ifndef SVG_CONVERTER_SVG_H
#define SVG_CONVERTER_SVG_H

#include <libxml/parser.h>
#include <libxml/xmlerror.h>

#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>

namespace detail {

/**
 * `std::unique_ptr` deleter for libxml2 elements.
 */
struct Libxml2Deleter {
    void operator()(xmlDocPtr doc) const;
};

}  // namespace detail

class SvgDocument {
 private:
    std::unique_ptr<xmlDoc, detail::Libxml2Deleter> doc_;
    std::unordered_map<std::string, xmlNodePtr> nodes_by_id_;

 public:
    /**
     * Loads an XML document from the given file.
     */
    explicit SvgDocument(const std::string& filename);

    /**
     * Pointer to the root node of the document.
     *
     * Only valid for the lifetime of the document.
     */
    xmlNodePtr root() const;

    /**
     * Finds a node by its `id` attribute.
     */
    xmlNodePtr find_by_id(const std::string& id) const;
};

class SvgLoadError : public std::exception {
 private:
    xmlError error_;

 public:
    explicit SvgLoadError(xmlErrorPtr errorPtr);
    ~SvgLoadError() override;

    // xmlError contains allocated memory (message, etc.), therefore
    // this exception should not be copyable.
    SvgLoadError(const SvgLoadError&) = delete;
    SvgLoadError(SvgLoadError&&) = default;
    SvgLoadError& operator=(const SvgLoadError&) = delete;
    SvgLoadError& operator=(SvgLoadError&&) = default;

    const char* what() const noexcept override;
};

#endif  // SVG_CONVERTER_SVG_H
