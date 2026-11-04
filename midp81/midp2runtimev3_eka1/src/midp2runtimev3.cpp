#include <e32base.h>
#include <hal.h>
#include <hal_data.h>

TDesC* GetDynamicPropertyL(void* self, TDesC16* property) {
	HBufC16* ret = NULL;
	if (!property->Compare(_L("microedition.locale"))) {
		return (((TDesC* (*)(void*)) ((unsigned int*)self)[4]))(self);
	}
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
		ret = HBufC16::NewLC(16);
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
	unsigned int* r = ((unsigned int* (*)()) lib.Lookup(1))();
	User::LeaveIfNull(r);
	r -= 1;
	
	unsigned int* vtable = (unsigned int*) User::AllocL(64);
	Mem::Copy(vtable, (void*)r[1], 64);

	vtable[14] = vtable[13];
	vtable[15] = (unsigned int)GetDynamicPropertyL;
	r[1] = (unsigned int)vtable;
	
	return r + 1;
}

TInt E32Dll(TDllReason) {
	return 0;
}
