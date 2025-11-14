#pragma once
#include <objbase.h>
#include <Unknwn.h>

static const CLSID IID_IADDER =
{ 0xe04d1823, 0x1f2e, 0x114d, {0x22, 0xae, 0x4e, 0xab, 0x4c, 0xed, 0x64, 0xd8} };

__interface IAdder : IUnknown {
	virtual HRESULT __stdcall Add(const double, const double, double&) = 0;
	virtual HRESULT __stdcall Sub(const double, const double, double&) = 0;
};