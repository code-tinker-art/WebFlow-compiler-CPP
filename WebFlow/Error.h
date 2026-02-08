#pragma once
#include <iostream>
#include <string>

typedef std::string str;
using std::cerr;
using std::abort;

class Err {
public:
	str error;
	Err(const str& errMsg);
};

class EmptySource : public Err {
public:
	EmptySource(const str& errMsg);
};

class UnclosedError : public Err {
public:
	UnclosedError(const str& errMsg);
};

class UnexpectedError : public Err {
public:
	UnexpectedError(const str& errMsg);
};

class InvalidError : public Err {
public:
	InvalidError(const str& errMsg);
};

//Helper Class

class Error_log {
public:
	Error_log() {
		//not used yet
	}

	void unexpectedError(str errMsg) {
		const UnexpectedError e(errMsg);
	}

	void unclosedError(str errMsg) {
		const UnclosedError e(errMsg);
	}

	void emptySource(str errMsg) {
		const EmptySource e(errMsg);
	}

	void invalidError(str errMsg) {
		const InvalidError e(errMsg);
	}

};

