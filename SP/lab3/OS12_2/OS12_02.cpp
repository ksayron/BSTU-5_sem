
// OS12_02 
#include "iostream"
#include <Unknwn.h>
#include "../OS12_COM/IAdder.h"
#include "../OS12_COM/IMultiplier.h"
using std::cout;

#define IERR(s) std::cout << "error: " << s << std::endl;
#define IRES(s,r) std::cout << s << r << std::endl;

IAdder* pIAdder = nullptr;
IMultiplier* pIMultiplier = nullptr;

static const CLSID CLSID_CA =
{ 0x02fe8a5e, 0x281b, 0x4e3a, { 0x8f, 0x87, 0x6c, 0x54, 0x4d, 0xed, 0xa9, 0x94 } };

void main() {
	IUnknown* pIUnknown = nullptr;

	CoInitialize(NULL);

	HRESULT hr0 = CoCreateInstance(CLSID_CA, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pIUnknown);

	if (FAILED(hr0)) {
		IERR("CoCreateInstance");
		return;
	}

	if (SUCCEEDED(pIUnknown->QueryInterface(IID_IMULTIPLIER, (void**)&pIMultiplier))) {
		double z = 0.0;

		if (FAILED(pIMultiplier->Mul(2.0, 3.0, z))) {
			IERR("IMultiplier->Mul")
		}
		else {
			IRES("2 * 3 = ", z)
		}

		if (FAILED(pIMultiplier->Div(6.0, 3.0, z))) {
			IERR("IMultiplier->Div")
		}
		else {
			IRES("6 / 3 = ", z)
		}
	}
	else {
		IERR("IMultiplier->QueryInterface")
	}

	pIMultiplier->Release();

	if (SUCCEEDED(pIUnknown->QueryInterface(IID_IADDER, (void**)&pIAdder))) {
		double z = 0.0;

		if (FAILED(pIAdder->Add(2.0, 3.0, z))) {
			IERR("IAdder->Add")
		}
		else {
			IRES("2 + 3 = ", z)
		}

		if (FAILED(pIAdder->Sub(6.0, 3.0, z))) {
			IERR("IAdder->Sub")
		}
		else {
			IRES("6 - 3 = ", z)
		}
	}
	else {
		IERR("IAdder->QueryInterface")
	}

	pIAdder->Release();
	pIUnknown->Release();

	CoUninitialize();
}


