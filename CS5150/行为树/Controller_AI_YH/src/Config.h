#pragma once

enum class Status {
	BH_INVALID = 0,
	BH_SUCCESS,
	BH_FAILURE,
	BH_RUNNING // in the middle of doing sth
};