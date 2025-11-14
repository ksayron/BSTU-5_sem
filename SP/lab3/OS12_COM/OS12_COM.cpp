#include "pch.h"

SP02::SP02() : counter(1) {}

SP02::~SP02() {}

HRESULT __stdcall SP02::QueryInterface(const IID& iid, void** ppv) {
	if (iid == IID_IADDER) {
		*ppv = (IAdder*)this;
	}
	else if (iid == IID_IMULTIPLIER) {
		*ppv = (IMultiplier*)this;
	}
	else {
		*ppv = this;
	}

	this->AddRef();
	return S_OK;
}

ULONG __stdcall SP02::AddRef() {
	InterlockedIncrement(&counter);
	return counter;
}

ULONG __stdcall SP02::Release() {
	InterlockedDecrement(&counter);

	if (counter == 0) {
		delete this;
		return 0;
	}

	return counter;
}

HRESULT __stdcall SP02::Add(const double x, const double y, double& c) {
	c = x + y;
	return S_OK;
}

HRESULT __stdcall SP02::Sub(const double x, const double y, double& c) {
	c = x - y;
	return S_OK;
}

HRESULT __stdcall SP02::Mul(const double x, const double y, double& c) {
	c = x * y;
	return S_OK;
}

HRESULT __stdcall SP02::Div(const double x, const double y, double& c) {
	c = x / y;
	return S_OK;
}