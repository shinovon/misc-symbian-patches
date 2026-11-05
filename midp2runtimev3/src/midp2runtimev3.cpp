#include <e32base.h>
#include <hal.h>
#include <hal_data.h>
#include <etel3rdparty.h>
#include <bautils.h>
#include <apgcli.h>

// TODO: uiq3

#ifdef EKA2
#define sizeof_Runtime 48
#define sizeof_Runtime_vtable 64
#else
#define sizeof_Runtime 30
#define sizeof_Runtime_vtable 64
#endif

_LIT(KLocale, "microedition.locale");
_LIT(KMemoryRamFree, "com.nokia.memoryramfree");
_LIT(KBatteryLevel, "com.nokia.mid.batterylevel");
_LIT(KVersion, "cc.nnproject.symbianext.version");
_LIT(KCheckTls, "cc.nnproject.symbianext.checkTLS12");
_LIT(KIsAppInstalledPrefix, "cc.nnproject.symbianext.isAppInstalled:");
_LIT(KTrue, "true");
_LIT(KFalse, "false");

static HBufC* GetFreeRamL() {
	HBufC* res = HBufC::NewLC(16);
	TPtr16 des = res->Des();
	int tmp;
	HAL::Get(HALData::EMemoryRAMFree, tmp);
	des.AppendNum(tmp);
	CleanupStack::Pop();
	return res;
}

static HBufC* GetBatteryLevelL() {
	CTelephony* telephony = CTelephony::NewLC();
	CTelephony::TBatteryInfoV1 info;
	CTelephony::TBatteryInfoV1Pckg infoPkg(info);
	TRequestStatus status;
	telephony->GetBatteryInfo(status, infoPkg);
	User::WaitForRequest(status);

	HBufC* res = NULL;
	if (status.Int() != KErrNone) {
		res = HBufC16::NewLC(16);
		TPtr16 des = res->Des();
		des.AppendNum(info.iChargeLevel);
		CleanupStack::Pop();
	}
	CleanupStack::PopAndDestroy(telephony);
	return res;
}

static HBufC* CheckTlsL() {
	RFs fs;
	if (fs.Connect() != KErrNone) return NULL;
	HBufC* res = NULL;
#ifdef EKA2
	if (BaflUtils::FileExists(fs, _L("Z:\\system\\install\\mbedtls_stub.sis"))) {
		res = _L("mbedtls_cfw").AllocL();
	} else {
		RLibrary lib;
		if (lib.Load(_L("mbedtls.dll")) == KErrNone) {
			res = _L("mbedtls").AllocL();
			
			lib.Close();
		} else {
			res = KFalse().AllocL();
		}
	}
#else
	if (BaflUtils::FileExists(fs, _L("C:\\system\\libs\\ssl.dll"))
			|| BaflUtils::FileExists(fs, _L("C:\\system\\libs\\ssladapto2.dll"))
			|| BaflUtils::FileExists(fs, _L("C:\\system\\libs\\ssladaptor.dll"))) {
		res = _L("bearssl").AllocL();
	} else {
		res = KFalse().AllocL();
	}
#endif
	fs.Close();
	return res;
}

static HBufC* IsAppInstalledL(TDesC* property) {
	if ((TUint32)property->Length() < KIsAppInstalledPrefix.iTypeLength) return NULL;
	TLex lexer(property->Mid(KIsAppInstalledPrefix.iTypeLength));
	TUint32 uid;
	if (lexer.Val(uid, EHex) != KErrNone) return NULL;
	
	RApaLsSession ls;
	if (ls.Connect() != KErrNone) return NULL;
	CleanupClosePushL(ls);
	
	TApaAppInfo appInfo;
	HBufC* res = NULL;
	if (ls.GetAppInfo(appInfo, TUid::Uid(uid)) == KErrNone) {
		res = KTrue().AllocL();
	} else {
		res = KFalse().AllocL();
	}

	CleanupStack::PopAndDestroy(); // ls
	return res;
}

static HBufC* GetVersionL() {
	HBufC* res = HBufC16::NewLC(4);
	TPtr16 des = res->Des();
	des.Append(_L("0.1"));
	CleanupStack::Pop();
	return res;
}

HBufC* GetDynamicPropertyL(void* self, TDesC* property) {
	HBufC* res = NULL;
#ifndef EKA2
	if (*property == KLocale) {
		return (((HBufC* (*)(void*)) ((unsigned int*)self)[4]))(self);
	} else
#endif
	if (*property == KMemoryRamFree) {
		res = GetFreeRamL();
	} else if (*property == KBatteryLevel) {
		res = GetBatteryLevelL();
	} else if (*property == KVersion) {
		res = GetVersionL();
	} else if (*property == KCheckTls) {
		res = CheckTlsL();
	} else if (property->Find(KIsAppInstalledPrefix) == 0) {
		res = IsAppInstalledL(property);
	}
	return res;
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
