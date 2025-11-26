#pragma once

#ifdef __cplusplus
extern "C" {
#endif

	// Exported functions with C linkage and cdecl calling convention
	__declspec(dllexport) void __cdecl FileHandler();
	__declspec(dllexport) void __cdecl LTT_main();
	__declspec(dllexport) void __cdecl LTTWrite();

#ifdef __cplusplus
}
#endif