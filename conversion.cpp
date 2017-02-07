#include <iostream>

#include <boost/numeric/ublas/assignment.hpp>
#include <boost/numeric/ublas/operations.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <libxml/parser.h>
#include <svgpp/policy/xml/libxml2.hpp>
#include <svgpp/svgpp.hpp>

#include "conversion.h"

typedef boost::numeric::ublas::c_matrix<double, 3, 3> matrix_t;

typedef boost::mpl::set<
        // SVG Structural Elements
        svgpp::tag::element::svg,
        svgpp::tag::element::g //,
        // SVG Shape Elements
//        svgpp::tag::element::circle,
//        svgpp::tag::element::ellipse,
//        svgpp::tag::element::line,
//        svgpp::tag::element::path,
//        svgpp::tag::element::polygon,
//        svgpp::tag::element::polyline,
//        svgpp::tag::element::rect
    >::type processed_elements_t;

typedef boost::mpl::set<
        svgpp::tag::attribute::transform
    >::type processed_attributes_t;

class ConversionContext {
private:
    matrix_t transform_;

public:
    ConversionContext();
    ConversionContext(const ConversionContext& parent);

    void on_exit_element();

    void transform_matrix(const boost::array<double, 6>& numbers);
};

struct ContextFactories {
    template<class ParentContext, class ElementTag>
    struct apply {
        typedef svgpp::factory::context::on_stack<ConversionContext> type;
    };
};

ConversionContext::ConversionContext() {
    transform_ <<=
        1, 0, 0,
        0, 1, 0,
        0, 0, 1;
}

ConversionContext::ConversionContext(const ConversionContext& parent)
        : transform_(parent.transform_) {
}

void ConversionContext::on_exit_element() {
    std::cout << "Leaving element with transform " << transform_ << '\n';
}

void ConversionContext::transform_matrix(const boost::array<double, 6>& numbers) {
    matrix_t matrix;
    matrix <<=
        numbers[0], numbers[2], numbers[4],
        numbers[1], numbers[3], numbers[5],
        0, 0, 1;
    transform_ = boost::numeric::ublas::prod(transform_, matrix);
}

std::string convert(XmlDocument& svgDoc) {
    ConversionContext context;
    svgpp::document_traversal<
            svgpp::processed_elements<processed_elements_t>,
            svgpp::processed_attributes<processed_attributes_t>,
            svgpp::context_factories<ContextFactories>
    >::load_document(svgDoc.root(), context);
    return {};
}
