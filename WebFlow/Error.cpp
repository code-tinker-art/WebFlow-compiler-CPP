#include "Error.h"
#include <iostream>

Err::Err(const str& errMsg) {
	this->error = errMsg;
}

EmptySource::EmptySource(const str& errMsg) : Err(errMsg) {
	cerr << this->error;
	abort();
};

UnclosedError::UnclosedError(const str& errMsg) : Err(errMsg) {
	cerr << this->error;
	abort();
};

UnexpectedError::UnexpectedError(const str& errMsg) : Err(errMsg) {
	cerr << this->error;
	abort();
};

InvalidError::InvalidError(const str& errMsg) : Err(errMsg) {
	cerr << this->error;
	abort();
};