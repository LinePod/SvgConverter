#include "viewport_establishing.h"

ViewportEstablishingContext::ViewportEstablishingContext(
    const boost::optional<Viewport> inner_viewport)
    : inner_viewport_{inner_viewport} {}

const Viewport& ViewportEstablishingContext::inner_viewport() const {
    return *inner_viewport_;
}

void ViewportEstablishingContext::set_viewport(double, double, double width,
                                               double height) {
    // X and y, as well as all scaling and alignment is handled by SVG++ and
    // passed as a transform due to the viewport policy `as_transform`.
    inner_viewport_ = Viewport{width, height};
}

void ViewportEstablishingContext::set_viewbox_size(double width,
                                                   double height) {
    inner_viewport_ = Viewport{width, height};
}

void ViewportEstablishingContext::disable_rendering() {
    inner_viewport_.reset();
}

bool ViewportEstablishingContext::rendering_disabled() const {
    return !inner_viewport_;
}
