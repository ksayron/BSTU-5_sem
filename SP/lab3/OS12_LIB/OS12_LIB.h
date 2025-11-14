#pragma once

typedef void* OS12LIB;

namespace OS12 {
	OS12LIB Init();

	namespace Adder {
		double Add(OS12LIB h, double x, double y);
		double Sub(OS12LIB h, double x, double y);
	};

	namespace Multiplier {
		double Mul(OS12LIB h, double x, double y);
		double Div(OS12LIB h, double x, double y);
	}

	void Dispose(OS12LIB h);
}