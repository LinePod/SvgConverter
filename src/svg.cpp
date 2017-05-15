#include "svg.h"

namespace detail {

void Libxml2Deleter::operator()(xmlDocPtr doc) const { xmlFreeDoc(doc); }

void Libxml2Deleter::operator()(xmlErrorPtr error) const {
    xmlResetError(error);
}

void build_id_to_node_map(xmlNodePtr node,
                          std::unordered_map<std::string, xmlNodePtr>& map) {
    // A note on string handling:
    //
    // Libxml2 uses `xmlChar`s (which are an alias for `unsigned char`s)
    // internally, and has custom UTF-8 routines for them. SVG++s policy for
    // libxml2 `reinterpret_cast`s these to `char`s. Because we just want to
    // match with ids parsed from SVG++, we do the same here. If we ever want to
    // export text that might contain unicode, we have to develop another
    // strategy.

    constexpr xmlChar kIdAttribute[] = "id";
    if (const xmlChar* id_attr =
            xmlGetProp(node, static_cast<const xmlChar*>(kIdAttribute))) {
        std::string id{reinterpret_cast<const char*>(id_attr)};  // NOLINT
        map.insert({id, node});
    }

    for (xmlNodePtr child = node->children; child != nullptr;
         child = child->next) {
        build_id_to_node_map(child, map);
    }
}

}  // namespace detail

SvgLoadError::SvgLoadError(xmlErrorPtr errorPtr)
    : error_{std::make_unique<xmlError>()} {
    xmlCopyError(errorPtr, error_.get());
}

SvgDocument::SvgDocument(const std::string& filename)
    : doc_{xmlParseFile(filename.c_str())} {
    if (!doc_) {
        throw SvgLoadError{xmlGetLastError()};
    }

    detail::build_id_to_node_map(root(), nodes_by_id_);
}

xmlNodePtr SvgDocument::root() const {
    return xmlDocGetRootElement(doc_.get());
}

xmlNodePtr SvgDocument::find_by_id(const std::string& id) const {
    auto iter = nodes_by_id_.find(id);
    return iter == nodes_by_id_.end() ? nullptr : iter->second;
}

const char* SvgLoadError::what() const noexcept { return error_->message; }
