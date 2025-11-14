#include "pch.h"
#include "OS12_LIB.h"
#include <Unknwn.h>
#include <stdexcept>
#include <iostream>
#include "../OS12_COM/IAdder.h"
#include "../OS12_COM/IMultiplier.h"

static ULONG cObjects = 0;

#define IERR(s) std::cout << "error: " << s << std::endl;
#define IRES(s,r) std::cout << s << r << std::endl;

static const CLSID CLSID_CA =
{ 0x02fe8a5e, 0x281b, 0x4e3a, { 0x8f, 0x87, 0x6c, 0x54, 0x4d, 0xed, 0xa9, 0x94 } };


OS12LIB OS12::Init() {
	IUnknown* pIUnknown = nullptr;

	try {
		if (cObjects == 0) {
			if (FAILED(CoInitialize(NULL))) {
				throw std::runtime_error("CoInitialize");
			}
		}
		if (FAILED(CoCreateInstance(CLSID_CA, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pIUnknown))) {
			throw std::runtime_error("CreateInstance");
		}

		InterlockedIncrement(&cObjects);

		return pIUnknown;
	}
	catch (std::runtime_error error) {
		IERR(error.what());
	}
}

double OS12::Adder::Add(OS12LIB h, double x, double y) {
	double z = 0.0;
	IAdder* pIAdder = nullptr;

	try {
		if (FAILED(((IUnknown*)h)->QueryInterface(IID_IADDER, (void**)&pIAdder))) {
			throw std::runtime_error("QueryInterface");
		}

		if (FAILED(pIAdder->Add(x, y, z)))
			throw std::runtime_error("Add");
	}
	catch (std::runtime_error error) {
		IERR(error.what());
	}

	if (pIAdder != nullptr) {
		pIAdder->Release();
	}
	return z;
}

double OS12::Adder::Sub(OS12LIB h, double x, double y) {
	double z = 0.0;
	IAdder* pIAdder = nullptr;

	try {
		if (FAILED(((IUnknown*)h)->QueryInterface(IID_IADDER, (void**)&pIAdder))) {
			throw std::runtime_error("QueryInterface");
		}

		if (FAILED(pIAdder->Sub(x, y, z)))
			throw std::runtime_error("Sub");

	}
	catch (std::runtime_error error) {
		IERR(error.what());
	}

	if (pIAdder != nullptr) {
		pIAdder->Release();
	}
	return z;
}

double OS12::Multiplier::Mul(OS12LIB h, double x, double y) {
	IMultiplier* pIMultiplier = nullptr;
	double z = 0.0;

	try {
		if (FAILED(((IUnknown*)h)->QueryInterface(IID_IMULTIPLIER, (void**)&pIMultiplier))) {
			throw std::runtime_error("QueryInterface");
		}

		if (FAILED(pIMultiplier->Mul(x, y, z)))
			throw std::runtime_error("Mul");

		pIMultiplier->Release();
	}
	catch (std::runtime_error error) {
		IERR(error.what());
	}

	return z;
}

double OS12::Multiplier::Div(OS12LIB h, double x, double y) {
	try {
		IMultiplier* pIMultiplier = nullptr;
		double z = 0.0;

		if (FAILED(((IUnknown*)h)->QueryInterface(IID_IMULTIPLIER, (void**)&pIMultiplier))) {
			throw std::runtime_error("QueryInterface");
		}

		if (FAILED(pIMultiplier->Div(x, y, z)))
			throw std::runtime_error("Div");

		pIMultiplier->Release();
		return z;
	}
	catch (std::runtime_error error) {
		IERR(error.what());
		return 0;
	}
}

void OS12::Dispose(OS12LIB h) {
	((IUnknown*)h)->Release();
	InterlockedDecrement(&cObjects);

	if (cObjects == 0)
		CoFreeUnusedLibraries();
}