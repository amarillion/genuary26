#pragma once

#include <vector>
#include <allegro5/allegro_font.h>
#include "data.h"
#include "color.h"
#include <memory>

/**
 * a Span: a stretch of text with a single style,
 * or an inline image.
 *
 * Each stretch can have its own style
 * with different colors and fonts. A style is
 * backed by a parent style for values that are
 * not defined.
 *
 * A stretch doesn't have a fixed layout, it may
 * be wrapped if it doesn't fit in a given
 * horizontal space.
 */
class Span
{
private:
	const std::string elem;
	const std::string style;
	const std::map<std::string, std::string> attributes;
	const bool fImage = false;
	std::shared_ptr<ALLEGRO_BITMAP> image = nullptr;
public:
	Span(const std::string &val, const std::string &style) : elem(val), style(style) {}
	Span(
		const std::string &val, const std::string &style, const std::map<std::string, std::string> attributes
	) : elem(val), style(style), attributes(attributes) {}
	Span(const std::shared_ptr<ALLEGRO_BITMAP> &image) : fImage(true), image(image) {}

	const std::string &getText() const { return elem; }
	const std::string &getStyle() const { return style; }
	const std::string &getAttribute(const std::string &key) const { auto it = attributes.find(key); return it != attributes.end() ? it->second : ""; }
	bool isImage() const { return fImage; }
	ALLEGRO_BITMAP *getImage() const { return image.get(); }
};

class RichTextModel : public DataWrapper
{
private:
	std::vector<Span> spans;
public:
	RichTextModel() : spans() {
	}

	size_t getSize() {
		int result = 0;
		for (auto &i : spans) {
			result += i.getText().size();
		}
		return result;
	}

//	std::string &getText() { return text; }

	size_t getNumSegments() {
		return spans.size();
	}

	const std::string &getSegmentText(int i) {
		return spans[i].getText();
	}

	const Span &getSegment(int i) {
		return spans[i];
	}

	void appendText(const std::string &text, const std::string &style, const std::map<std::string, std::string> &attributes) {
		spans.push_back(Span(text, style, attributes));
		FireEvent(1);
	}

	void appendText(const std::string &text, const std::string &style) {
		spans.push_back(Span(text, style));
		FireEvent(1);
	}

	void appendText(const std::string &text) {
		spans.push_back(Span(text, ""));
		FireEvent(1);
	}

	void appendImage(const std::shared_ptr<ALLEGRO_BITMAP> &bmp) {
		spans.push_back(Span(bmp));
		FireEvent(1);
	}

	virtual ~RichTextModel() {}
};
