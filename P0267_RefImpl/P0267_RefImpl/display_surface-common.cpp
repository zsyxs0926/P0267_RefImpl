#include "io2d.h"
#include "xcairo.h"

#include <sstream>
#include <string>
#include <iostream>

using namespace std;
using namespace std::experimental;
using namespace std::experimental::io2d;
using namespace std::experimental::io2d::v1::cairo;
using namespace std::chrono;

// Note: cairo_surface_flush(_Native_surface.get()); must be called after calling this function.
void cairo_display_surface::_Render_for_scaling_uniform_or_letterbox(display_point dd) {
	const cairo_filter_t cairoFilter = CAIRO_FILTER_GOOD;
	
// 	static auto previousTime = steady_clock::now();

	if (_Dimensions == dd) {
		cairo_set_source_surface(_Native_context.get(), _Cairo_surface->_Surface.get(), 0.0F, 0.0F);
		cairo_paint(_Native_context.get());
	}
	else {
		const auto whRatio = static_cast<float>(_Dimensions.x) / static_cast<float>(_Dimensions.y);
		const auto displayWHRatio = static_cast<float>(dd.x) / static_cast<float>(dd.y);
		cairo_matrix_t ctm;
		auto nativeContext = _Native_context.get();
		//cairo_save(nativeContext);
		float rectX, rectY, rectWidth, rectHeight;
		if (whRatio < displayWHRatio) {
			cairo_new_path(nativeContext);
			const auto displayHeightAsDouble = static_cast<float>(dd.y);
			rectWidth = trunc(displayHeightAsDouble * whRatio);
			rectHeight = displayHeightAsDouble;
			rectX = trunc(abs(rectWidth - static_cast<float>(dd.x)) / 2.0F);
			rectY = 0.0F;
			cairo_rectangle(nativeContext, rectX, rectY, rectWidth, rectHeight);

			const auto heightRatio = static_cast<float>(_Dimensions.y) / static_cast<float>(dd.y);
			cairo_matrix_init_scale(&ctm, heightRatio, heightRatio);
			cairo_matrix_translate(&ctm, -rectX, 0.0F);
			unique_ptr<cairo_pattern_t, decltype(&cairo_pattern_destroy)> pat(cairo_pattern_create_for_surface(_Cairo_surface->_Surface.get()), &cairo_pattern_destroy);
			cairo_pattern_set_matrix(pat.get(), &ctm);
			cairo_pattern_set_extend(pat.get(), CAIRO_EXTEND_NONE);
			cairo_pattern_set_filter(pat.get(), cairoFilter);
			cairo_set_source(_Native_context.get(), pat.get());
			cairo_fill(_Native_context.get());
			//cairo_restore(nativeContext);
			if (_Scaling == std::experimental::io2d::scaling::letterbox) {
				const auto lboxWidth = trunc((static_cast<float>(dd.x) - rectWidth) / 2.0F);
				cairo_rectangle(nativeContext, 0.0F, 0.0F, lboxWidth, rectHeight);
				cairo_rectangle(nativeContext, rectWidth + lboxWidth, 0.0F, lboxWidth, rectHeight);
				//cairo_pattern_set_extend(_Letterbox_brush.native_handle(), _Extend_to_cairo_extend_t(_Letterbox_brush.wrap_mode()));
				//cairo_pattern_set_filter(_Letterbox_brush.native_handle(), _Filter_to_cairo_filter_t(_Letterbox_brush.filter()));
				//cairo_matrix_t cPttnMatrix;
				//cairo_matrix_init(&cPttnMatrix, _Letterbox_brush.matrix().m00(), _Letterbox_brush.matrix().m01(), _Letterbox_brush.matrix().m10(), _Letterbox_brush.matrix().m11(), _Letterbox_brush.matrix().m20(), _Letterbox_brush.matrix().m21());
				//cairo_pattern_set_matrix(_Letterbox_brush.native_handle(), &cPttnMatrix);
				//cairo_set_source(_Native_context.get(), _Letterbox_brush.native_handle());
				if (_Letterbox_brush == nullopt) {
					cairo_set_source(_Native_context.get(), _Default_brush._Native_handle());
					//cairo_paint(_Native_context.get());
				}
				else {
					auto pttn = _Letterbox_brush.value()._Native_handle();
					if (_Letterbox_brush_props == nullopt) {
						cairo_pattern_set_extend(pttn, CAIRO_EXTEND_NONE);
						cairo_pattern_set_filter(pttn, CAIRO_FILTER_GOOD);
						cairo_matrix_t cPttnMatrix;
						cairo_matrix_init_identity(&cPttnMatrix);
						cairo_pattern_set_matrix(pttn, &cPttnMatrix);
						cairo_set_source(_Native_context.get(), pttn);
						//cairo_paint(_Native_context.get());
					}
					else {
						const brush_props& props = _Letterbox_brush_props.value();
						cairo_pattern_set_extend(pttn, _Extend_to_cairo_extend_t(props.wrap_mode()));
						cairo_pattern_set_filter(pttn, _Filter_to_cairo_filter_t(props.filter()));
						cairo_matrix_t cPttnMatrix;
						const auto& m = props.brush_matrix();
						cairo_matrix_init(&cPttnMatrix, m.m00, m.m01, m.m10, m.m11, m.m20, m.m21);
						cairo_pattern_set_matrix(pttn, &cPttnMatrix);
						cairo_set_source(_Native_context.get(), pttn);
						//cairo_paint(_Native_context.get());
					}
				}
				cairo_fill(_Native_context.get());
			}
		}
		else {
			cairo_new_path(nativeContext);
			const auto displayWidthAsDouble = static_cast<float>(dd.x);
			rectWidth = displayWidthAsDouble;
			rectHeight = trunc(displayWidthAsDouble / whRatio);
			rectX = 0.0F;
			rectY = trunc(abs(rectHeight - static_cast<float>(dd.y)) / 2.0F);
			cairo_rectangle(nativeContext, rectX, rectY, rectWidth, rectHeight);

			const auto widthRatio = static_cast<float>(_Dimensions.x) / static_cast<float>(dd.x);
			cairo_matrix_init_scale(&ctm, widthRatio, widthRatio);
			cairo_matrix_translate(&ctm, 0.0F, -rectY);
			unique_ptr<cairo_pattern_t, decltype(&cairo_pattern_destroy)> pat(cairo_pattern_create_for_surface(_Cairo_surface->_Surface.get()), &cairo_pattern_destroy);
			cairo_pattern_set_matrix(pat.get(), &ctm);
			cairo_pattern_set_extend(pat.get(), CAIRO_EXTEND_NONE);
			cairo_pattern_set_filter(pat.get(), cairoFilter);
			cairo_set_source(_Native_context.get(), pat.get());
			cairo_fill(_Native_context.get());
			//cairo_restore(nativeContext);
			if (_Scaling == std::experimental::io2d::scaling::letterbox) {
				const auto lboxHeight = trunc((static_cast<float>(dd.y) - rectHeight) / 2.0F);
				cairo_rectangle(nativeContext, 0.0F, 0.0F, rectWidth, lboxHeight);
				cairo_rectangle(nativeContext, 0.0F, rectHeight + lboxHeight, rectWidth, lboxHeight);
				//cairo_pattern_set_extend(_Letterbox_brush.native_handle(), _Extend_to_cairo_extend_t(_Letterbox_brush.wrap_mode()));
				//cairo_pattern_set_filter(_Letterbox_brush.native_handle(), _Filter_to_cairo_filter_t(_Letterbox_brush.filter()));
				//cairo_matrix_t cPttnMatrix;
				//cairo_matrix_init(&cPttnMatrix, _Letterbox_brush.matrix().m00(), _Letterbox_brush.matrix().m01(), _Letterbox_brush.matrix().m10(), _Letterbox_brush.matrix().m11(), _Letterbox_brush.matrix().m20(), _Letterbox_brush.matrix().m21());
				//cairo_pattern_set_matrix(_Letterbox_brush.native_handle(), &cPttnMatrix);
				//cairo_set_source(_Native_context.get(), _Letterbox_brush.native_handle());
				if (_Letterbox_brush == nullopt) {
					cairo_set_source(_Native_context.get(), _Default_brush._Native_handle());
					//cairo_paint(_Native_context.get());
				}
				else {
					auto pttn = _Letterbox_brush.value()._Native_handle();
					if (_Letterbox_brush_props == nullopt) {
						cairo_pattern_set_extend(pttn, CAIRO_EXTEND_NONE);
						cairo_pattern_set_filter(pttn, CAIRO_FILTER_GOOD);
						cairo_matrix_t cPttnMatrix;
						cairo_matrix_init_identity(&cPttnMatrix);
						cairo_pattern_set_matrix(pttn, &cPttnMatrix);
						cairo_set_source(_Native_context.get(), pttn);
						//cairo_paint(_Native_context.get());
					}
					else {
						const brush_props& props = _Letterbox_brush_props.value();
						cairo_pattern_set_extend(pttn, _Extend_to_cairo_extend_t(props.wrap_mode()));
						cairo_pattern_set_filter(pttn, _Filter_to_cairo_filter_t(props.filter()));
						cairo_matrix_t cPttnMatrix;
						const auto& m = props.brush_matrix();
						cairo_matrix_init(&cPttnMatrix, m.m00, m.m01, m.m10, m.m11, m.m20, m.m21);
						cairo_pattern_set_matrix(pttn, &cPttnMatrix);
						cairo_set_source(_Native_context.get(), pttn);
						//cairo_paint(_Native_context.get());
					}
				}
				cairo_fill(_Native_context.get());
			}
		}
	}

// 	auto currentTime = steady_clock::now();
// 	auto elapsedTimeIncrement = static_cast<float>(duration_cast<nanoseconds>(currentTime - previousTime).count());
// 	previousTime = currentTime;
// 	stringstream timingStr;
// 	timingStr << "timing: " << elapsedTimeIncrement / 1'000'000.0F << endl;
// 	cerr << timingStr.str().c_str();
}

void cairo_display_surface::_Render_to_native_surface(display_point dd, display_surface<cairo_renderer>& _Display_surface) {
	const cairo_filter_t cairoFilter = CAIRO_FILTER_GOOD;
	cairo_surface_flush(_Cairo_surface->_Surface.get());
// 	cairo_save(_Native_context.get());
	cairo_set_operator(_Native_context.get(), CAIRO_OPERATOR_SOURCE);
	if (_User_scaling_fn != nullptr && _User_scaling_fn != nullptr) {
		bool letterbox = false;
		auto userRect = (*_User_scaling_fn)(_Display_surface, letterbox);
		if (letterbox) {
			if (_Letterbox_brush == nullopt) {
				cairo_set_source(_Native_context.get(), _Default_brush._Native_handle());
				cairo_paint(_Native_context.get());
			}
			else {
				auto pttn = _Letterbox_brush.value()._Native_handle();
				if (_Letterbox_brush_props == nullopt) {
					cairo_pattern_set_extend(pttn, CAIRO_EXTEND_NONE);
					cairo_pattern_set_filter(pttn, CAIRO_FILTER_GOOD);
					cairo_matrix_t cPttnMatrix;
					cairo_matrix_init_identity(&cPttnMatrix);
					cairo_pattern_set_matrix(pttn, &cPttnMatrix);
					cairo_set_source(_Native_context.get(), pttn);
					cairo_paint(_Native_context.get());
				}
				else {
					const brush_props& props = _Letterbox_brush_props.value();
					cairo_pattern_set_extend(pttn, _Extend_to_cairo_extend_t(props.wrap_mode()));
					cairo_pattern_set_filter(pttn, _Filter_to_cairo_filter_t(props.filter()));
					cairo_matrix_t cPttnMatrix;
					const auto& m = props.brush_matrix();
					cairo_matrix_init(&cPttnMatrix, m.m00, m.m01, m.m10, m.m11, m.m20, m.m21);
					cairo_pattern_set_matrix(pttn, &cPttnMatrix);
					cairo_set_source(_Native_context.get(), pttn);
					cairo_paint(_Native_context.get());
				}
			}
		}
		cairo_matrix_t ctm;
		cairo_matrix_init_scale(&ctm, 1.0F / (static_cast<float>(dd.x) / userRect.width()), 1.0F / (static_cast<float>(dd.y) / userRect.height()));
		cairo_matrix_translate(&ctm, -userRect.x(), -userRect.y());
		unique_ptr<cairo_pattern_t, decltype(&cairo_pattern_destroy)> pat(cairo_pattern_create_for_surface(_Cairo_surface->_Surface.get()), &cairo_pattern_destroy);
		cairo_pattern_set_matrix(pat.get(), &ctm);
		cairo_pattern_set_extend(pat.get(), CAIRO_EXTEND_NONE);
		cairo_pattern_set_filter(pat.get(), cairoFilter);
		cairo_set_source(_Native_context.get(), pat.get());
		cairo_paint(_Native_context.get());
		cairo_surface_flush(_Native_surface.get());
	}
	else {

		// Calculate the destRect values.
		switch (_Scaling) {
		case std::experimental::io2d::scaling::letterbox:
		{
			_Render_for_scaling_uniform_or_letterbox(dd);
		} break;

		case std::experimental::io2d::scaling::uniform:
		{
			_Render_for_scaling_uniform_or_letterbox(dd);
		} break;

		case std::experimental::io2d::scaling::fill_uniform:
		{
			// Maintain aspect ratio and center, but overflow if needed rather than letterboxing.
			if (_Dimensions.x == dd.x && _Dimensions.y == dd.y) {
				cairo_set_source_surface(_Native_context.get(), _Cairo_surface->_Surface.get(), 0.0F, 0.0F);
				cairo_paint(_Native_context.get());
			}
			else {
				auto widthRatio = static_cast<float>(dd.x) / static_cast<float>(_Dimensions.x);
				auto heightRatio = static_cast<float>(dd.y) / static_cast<float>(_Dimensions.y);
				if (widthRatio < heightRatio) {
					cairo_set_source_rgb(_Native_context.get(), 0.0F, 0.0F, 0.0F);
					cairo_paint(_Native_context.get());
					cairo_matrix_t ctm;
					cairo_matrix_init_scale(&ctm, 1.0F / heightRatio, 1.0F / heightRatio);
					cairo_matrix_translate(&ctm, trunc(abs(static_cast<float>(dd.x - (_Dimensions.x * heightRatio)) / 2.0F)), 0.0F);
					unique_ptr<cairo_pattern_t, decltype(&cairo_pattern_destroy)> pat(cairo_pattern_create_for_surface(_Cairo_surface->_Surface.get()), &cairo_pattern_destroy);
					cairo_pattern_set_matrix(pat.get(), &ctm);
					cairo_pattern_set_extend(pat.get(), CAIRO_EXTEND_NONE);
					cairo_pattern_set_filter(pat.get(), cairoFilter);
					cairo_set_source(_Native_context.get(), pat.get());
					cairo_paint(_Native_context.get());
				}
				else {
					cairo_set_source_rgb(_Native_context.get(), 0.0F, 0.0F, 0.0F);
					cairo_paint(_Native_context.get());
					cairo_matrix_t ctm;
					cairo_matrix_init_scale(&ctm, 1.0F / widthRatio, 1.0F / widthRatio);
					cairo_matrix_translate(&ctm, 0.0F, trunc(abs(static_cast<float>(dd.y - (_Dimensions.y * widthRatio)) / 2.0F)));
					unique_ptr<cairo_pattern_t, decltype(&cairo_pattern_destroy)> pat(cairo_pattern_create_for_surface(_Cairo_surface->_Surface.get()), &cairo_pattern_destroy);
					cairo_pattern_set_matrix(pat.get(), &ctm);
					cairo_pattern_set_extend(pat.get(), CAIRO_EXTEND_NONE);
					cairo_pattern_set_filter(pat.get(), cairoFilter);
					cairo_set_source(_Native_context.get(), pat.get());
					cairo_paint(_Native_context.get());
				}
			}
		} break;
		case std::experimental::io2d::scaling::fill_exact:
		{
			// Maintain aspect ratio and center, but overflow if needed rather than letterboxing.
			if (_Dimensions.x == dd.x && _Dimensions.y == dd.y) {
				cairo_set_source_surface(_Native_context.get(), _Cairo_surface->_Surface.get(), 0.0F, 0.0F);
				cairo_paint(_Native_context.get());
			}
			else {
				auto widthRatio = static_cast<float>(dd.x) / static_cast<float>(_Dimensions.x);
				auto heightRatio = static_cast<float>(dd.y) / static_cast<float>(_Dimensions.y);
				cairo_matrix_t ctm;
				cairo_matrix_init_scale(&ctm, 1.0F / widthRatio, 1.0F / heightRatio);
				unique_ptr<cairo_pattern_t, decltype(&cairo_pattern_destroy)> pat(cairo_pattern_create_for_surface(_Cairo_surface->_Surface.get()), &cairo_pattern_destroy);
				cairo_pattern_set_matrix(pat.get(), &ctm);
				cairo_pattern_set_extend(pat.get(), CAIRO_EXTEND_NONE);
				cairo_pattern_set_filter(pat.get(), cairoFilter);
				cairo_set_source(_Native_context.get(), pat.get());
				cairo_paint(_Native_context.get());
			}
		} break;
		case std::experimental::io2d::scaling::none:
		{
			cairo_set_source_surface(_Native_context.get(), _Cairo_surface->_Surface.get(), 0.0F, 0.0F);
			cairo_paint(_Native_context.get());
		} break;
		default:
		{
			assert("Unexpected _Scaling value." && false);
		} break;
		}
	}

	// 	cairo_restore(_Native_context.get());
	// This call to cairo_surface_flush is needed for Win32 surfaces to update.
	cairo_surface_flush(_Native_surface.get());
}

void cairo_display_surface::_Make_impl_surface_and_context() {
	// We render to the fixed size surface.
	_Cairo_surface->_Surface = unique_ptr<cairo_surface_t, decltype(&cairo_surface_destroy)>(cairo_image_surface_create(_Format_to_cairo_format_t(_Cairo_surface->_Format), _Dimensions.x, _Dimensions.y), &cairo_surface_destroy);
	_Cairo_surface->_Context = unique_ptr<cairo_t, decltype(&cairo_destroy)>(cairo_create(_Cairo_surface->_Surface.get()), &cairo_destroy);
	_Throw_if_failed_cairo_status_t(cairo_surface_status(_Cairo_surface->_Surface.get()));
	_Throw_if_failed_cairo_status_t(cairo_status(_Cairo_surface->_Context.get()));
}

cairo_display_surface::cairo_display_surface(cairo_surface* cs, int preferredWidth, int preferredHeight, experimental::io2d::format preferredFormat, experimental::io2d::scaling scl)
	: _Cairo_surface(cs)
	, _Scaling(scl)
	, _Dimensions(preferredWidth, preferredHeight)
	, _Draw_fn()
	, _Size_change_fn()
	, _User_scaling_fn()
	, _Auto_clear(false)
	, _Redraw_requested(true)
	, _Native_surface(nullptr, &cairo_surface_destroy)
	, _Native_context(nullptr, &cairo_destroy)
	, _Letterbox_brush()
	, _Default_brush(rgba_color::transparent_black) {

	// Record the desired client window size
	if (preferredWidth <= 0 || preferredHeight <= 0 || preferredFormat == experimental::io2d::format::invalid) {
		throw invalid_argument("Invalid parameter.");
	}
}

void cairo_display_surface::draw_callback(const ::std::function<void(display_surface<cairo_renderer>& sfc)>& fn) {
	_Draw_fn = make_unique<::std::function<void(display_surface<cairo_renderer>& sfc)>>(fn);
}

void cairo_display_surface::invoke_draw_callback(display_point dp, display_surface<cairo_renderer>& ds) {
	// Run user draw function:
	if (_Draw_fn != nullptr) {
		if (auto_clear()) {
			_Cairo_surface->clear();
		}
		(*_Draw_fn)(ds);
	}
	else {
		throw system_error(make_error_code(errc::operation_would_block));
	}
	_Render_to_native_surface(dp, ds);
}

void cairo_display_surface::size_change_callback(const ::std::function<void(display_surface<cairo_renderer>& sfc)>& fn) {
	_Size_change_fn = make_unique<::std::function<void(display_surface<cairo_renderer>& sfc)>>(fn);
}

void cairo_display_surface::invoke_size_change_callback(display_surface<cairo_renderer>& ds) {
	// Run user draw function:
	if (_Size_change_fn != nullptr) {
		(*_Size_change_fn)(ds);
	}
}

void cairo_display_surface::dimensions(display_point dp) {
	bool recreate = false;

	if (_Dimensions != dp) {
		_Dimensions = dp;
		recreate = true;
	}

	if (recreate) {
		// Recreate the render target that is drawn to the displayed surface
		_Cairo_surface->_Surface = unique_ptr<cairo_surface_t, decltype(&cairo_surface_destroy)>(cairo_image_surface_create(_Format_to_cairo_format_t(_Cairo_surface->_Format), _Dimensions.x, _Dimensions.y), &cairo_surface_destroy);
		_Cairo_surface->_Context = unique_ptr<cairo_t, decltype(&cairo_destroy)>(cairo_create(_Cairo_surface->_Surface.get()), &cairo_destroy);
		//_Ensure_state();
	}
}

void cairo_display_surface::scaling(experimental::io2d::scaling scl) noexcept {
	_Scaling = scl;
}

void cairo_display_surface::user_scaling_callback(const function<experimental::io2d::bounding_box(const display_surface<cairo_renderer>&, bool&)>& fn) {
	_User_scaling_fn = make_unique<function<experimental::io2d::bounding_box(const display_surface<cairo_renderer>&, bool&)>>(fn);
}

void cairo_display_surface::letterbox_brush(const optional<cairo_brush>& b, const optional<brush_props> bp) noexcept {
	_Letterbox_brush = b;
	_Letterbox_brush_props = bp;
}

void cairo_display_surface::auto_clear(bool val) noexcept {
	_Auto_clear = val;
}

void cairo_display_surface::redraw_required() noexcept {
	_Redraw_requested = true;
}

format cairo_display_surface::format() const noexcept {
	return _Cairo_surface->_Format;
}

display_point cairo_display_surface::dimensions() const noexcept {
	return _Dimensions;
}

experimental::io2d::scaling cairo_display_surface::scaling() const noexcept {
	return _Scaling;
}

::std::function<::std::experimental::io2d::bounding_box(const display_surface<cairo_renderer>&, bool&)> cairo_display_surface::user_scaling_callback() const {
	return *_User_scaling_fn;
}

::std::function<::std::experimental::io2d::bounding_box(const display_surface<cairo_renderer>&, bool&)> cairo_display_surface::user_scaling_callback(error_code& ec) const noexcept {
	try {
		ec.clear();
		return *_User_scaling_fn;
	}
	catch (const bad_alloc&) {
		ec = make_error_code(errc::not_enough_memory);
		return nullptr;
	}
}

optional<cairo_brush> cairo_display_surface::letterbox_brush() const noexcept {
	return _Letterbox_brush;
}

optional<brush_props> cairo_display_surface::letterbox_brush_props() const noexcept {
	return _Letterbox_brush_props;
}

bool cairo_display_surface::auto_clear() const noexcept {
	return _Auto_clear;
}