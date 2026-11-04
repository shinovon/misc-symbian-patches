#include <e32base.h>
#include <hal.h>
#include <hal_data.h>

// TODO: uiq3

#ifdef EKA2
#define sizeof_Runtime 48
#define sizeof_Runtime_vtable 64
#else
#define sizeof_Runtime 30
#define sizeof_Runtime_vtable 64
#endif

TDesC* GetDynamicPropertyL(void* self, TDesC16* property) {
	HBufC16* ret = NULL;
#ifndef EKA2
	if (!property->Compare(_L("microedition.locale"))) {
		return (((TDesC* (*)(void*)) ((unsigned int*)self)[4]))(self);
	}
#endif
	if (!property->Compare(_L("com.nokia.memoryramfree"))) {
		ret = HBufC16::NewLC(16);
		TPtr16 des = ret->Des();
		int tmp;
		HAL::Get(HALData::EMemoryRAMFree, tmp);
		des.AppendNum(tmp);
		CleanupStack::Pop();
		return ret;
	}
	if (!property->Compare(_L("asd"))) {
		ret = HBufC16::NewLC(4);
		TPtr16 des = ret->Des();
		des.Append(_L("dsa"));
		CleanupStack::Pop();
		return ret;
	}
	// TODO: custom dynamic properties
	return NULL;
}

EXPORT_C void* GetRuntimeL() {
	RLibrary lib;
	User::LeaveIfError(lib.Load(_L("midp2runtimev2.dll")));
	unsigned int* runtime = ((unsigned int* (*)()) lib.Lookup(1))();
	User::LeaveIfNull(runtime);
	runtime -= 1;

	unsigned int* vtable = (unsigned int*) User::AllocL(sizeof_Runtime_vtable + 4);
	Mem::Copy(vtable, (void*)runtime[1], sizeof_Runtime_vtable);

#ifdef EKA2
	vtable[11] = (unsigned int)GetDynamicPropertyL;
#else
	vtable[16] = (unsigned int)GetDynamicPropertyL;
#endif
	runtime[1] = (unsigned int)vtable;
	
	return runtime + 1;
}

#ifndef EKA2
TInt E32Dll(TDllReason) {
	return 0;
}
#endif
