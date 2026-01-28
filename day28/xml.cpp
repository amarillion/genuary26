#include "xml.h"

#include <cctype>
#include <string>
#include <istream>

XmlParser::XmlParser(std::istream &&_stream) : stream(_stream) {
}

char XmlParser::consumeChar() {
	char c = stream.get();
	if (c == '\n') {
		line++;
		column = 1;
	} else {
		column++;
	}
	return c;
}

void XmlParser::expect(char expected, const std::string &errorMessage) {
	char c = stream.peek();
	if (c == expected) {
		consumeChar();
	}
	else {
		throw std::runtime_error(errorMessage + "; Expected '" + expected + "' at " + 
						std::to_string(line) + ", column " + std::to_string(column));
	}
}

bool XmlParser::consumeIf(char expected) {
	char c = stream.peek();
	if (c == expected) {
		consumeChar();
		return true;
	}
	return false;
}

std::string XmlParser::parseName() {
	std::string result;
	while (stream.good()) {
		char c = stream.peek();
		if (std::isspace(c) || c == '>' || c == '/' || c == '=') {
			break;
		}
		result += consumeChar();
	}
	return result;
}

std::string XmlParser::consumeUntil(char quote) {
	std::string result;
	while (stream.good()) {
		char c = stream.peek();
		if (c == quote) {
			break;
		}
		else {
			result += c;
			consumeChar();
		}
	}
	return result;
}

void XmlParser::parseStartTag() {
	currentAttributes.clear();	
	
	currentName = parseName();
	tagStack.push(currentName);

	// Parse attributes
	skipWhitespace();
	while (stream.good()) {
		if (consumeIf('>')) {
			break; // good, we're done.
		} 
		else if (consumeIf('/')) {
			expect('>', "Misformed self-closing tag");
			// Pop from stack since self-closing
			tagStack.pop();
			break;
		} else {
			// Parse attribute
			parseAttribute();
			skipWhitespace();
		}
	}

}

void XmlParser::parseAttribute() {
	std::string attrName;
	std::string attrValue;
	
	// Parse attribute name
	attrName = parseName();
	skipWhitespace();
	expect('=', "Expected '=' after attribute name");
	skipWhitespace();

	// Parse attribute value (expect quotes)
	char quote = stream.peek();
	if (quote != '"' && quote != '\'') {
		throw std::runtime_error("Expected quote for attribute value at line " + 
								std::to_string(line) + ", column " + std::to_string(column));
	}
	consumeChar();
	
	attrValue = consumeUntil(quote);
	expect(quote, "Closing quote missing");
	currentAttributes[attrName] = attrValue;
}

void XmlParser::skipWhitespace() {
	while (stream.good()) {
		char c = stream.peek();
		if (std::isspace(c)) {
			consumeChar();
		} else {
			break;
		}
	}
}

XmlParser::EventType XmlParser::next() {
	if (!stream.good()) {
		currentEvent = END_DOCUMENT;
		return currentEvent;
	}

	// Skip whitespace between elements
	skipWhitespace();

	if (!stream.good()) {
		currentEvent = END_DOCUMENT;
		return currentEvent;
	}

	char c = stream.peek();
	if (consumeIf('<')) {
		if (consumeIf('/')) {
			currentEvent = END_TAG;
			currentName = parseName();
			skipWhitespace();
			expect('>', "Malformed close tag");

			// Pop tag if it matches open tag, otherwise gracefully ignore.
			if (!tagStack.empty() && tagStack.top() == currentName) {
				tagStack.pop();
			}
		} else {
			// Start tag
			currentEvent = START_TAG;
			parseStartTag();
		}
	} else {
		// Text content
		parseText();
		currentEvent = TEXT;
	}

	return currentEvent;
}

void XmlParser::parseText() {
	currentText = consumeUntil('<');
	// Trim whitespace from text
	size_t start = currentText.find_first_not_of(" \t\n\r");
	if (start == std::string::npos) {
		currentText.clear();
	} else {
		size_t end = currentText.find_last_not_of(" \t\n\r");
		currentText = currentText.substr(start, end - start + 1);
	}
}
