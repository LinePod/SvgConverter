#include "svg.h"

const CoordinateSystem& SvgContext::inner_coordinate_system() const {
    return coordinate_system();
}

const Viewport& SvgContext::inner_viewport() const { return inner_viewport_; }

void SvgContext::set_viewport(double, double, double width, double height) {
    // X and y, as well as all scaling and aligning is handled by SVG++ and
    // passed as a transform due to the viewport policy `as_transform`
    inner_viewport_.set_size(width, height);
}

void SvgContext::set_viewbox_size(double width, double height) {
    inner_viewport_.set_size(width, height);
}

void SvgContext::disable_rendering() { rendering_disabled_ = true; }

bool SvgContext::rendering_disabled() const { return rendering_disabled_; }
