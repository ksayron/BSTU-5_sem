#pragma once
#include <objbase.h>
#include <Unknwn.h>
#include "IAdder.h"
#include "IMultiplier.h"



class SP02 : public IAdder, public IMultiplier {
private:
	ULONG counter;
public:
	SP02();
	~SP02();
	HRESULT __stdcall QueryInterface(const IID& iid, void** ppv);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall Add(const double, const double, double&);
	HRESULT __stdcall Sub(const double, const double, double&);
	HRESULT __stdcall Mul(const double, const double, double&);
	HRESULT __stdcall Div(const double, const double, double&);
};