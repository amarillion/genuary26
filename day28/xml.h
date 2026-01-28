#pragma once

#include <stack>
#include <map>
#include <string>

class XmlParser {
public:
	enum EventType {
		START_DOCUMENT,
		END_DOCUMENT,
		START_TAG,
		END_TAG,
		TEXT,
		ERROR
	};
	
	XmlParser(std::istream &&is);

	EventType next();

	EventType getEventType() const {
		return currentEvent;
	}

	const std::string& getName() const {
		return currentName;
	}

	const std::string& getText() const {
		return currentText;
	}

	const std::map<std::string, std::string>& getAttributes() const {
		return currentAttributes;
	}

	int getLine() const { return line; }
	int getColumn() const { return column; }

private:
	std::istream &stream;
	EventType currentEvent;
	std::string currentName;
	std::string currentText;
	std::map<std::string, std::string> currentAttributes;
	std::stack<std::string> tagStack;
	int line;
	int column;

	void skipWhitespace();
	void parseStartTag();
	void parseAttribute();
	void parseText();
	std::string parseName();

	char consumeChar();
	bool consumeIf(char c);
	std::string consumeUntil(char c);
	void expect(char c, const std::string &errorMsg);
};