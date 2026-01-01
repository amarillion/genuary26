#pragma once

/**
Take any number, return 0, 1 or -1
*/
// after: https://stackoverflow.com/questions/1903954/is-there-a-standard-sign-function-signum-sgn-in-c-c
template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}
