#include <e32base.h>
#include <hal.h>
#include <hal_data.h>
#include <etel3rdparty.h>
#include <bautils.h>
#include <apgcli.h>
#include <apacmdln.h>
#ifdef EKA2
#include <swi/sisregistrysession.h>
#include <swi/sisregistryentry.h>
#endif

#ifdef EKA2
#define sizeof_Runtime 48
#define sizeof_Runtime_vtable 64
#else
#define sizeof_Runtime 30
#define sizeof_Runtime_vtable 64
#endif

_LIT(KIsAppInstalledPrefix, "cc.nnproject.symbianext.isAppInstalled:");
_LIT(KGetInstalledSisVersionPrefix, "cc.nnproject.symbianext.getInstalledSisVersion:");
_LIT(KLaunchAppPrefix, "cc.nnproject.symbianext.launchApp:");
_LIT(KTrue, "true");
_LIT(KFalse, "false");
_LIT(KApiVersion, "0.1");

static HBufC* GetFreeRamL() {
	HBufC* res = HBufC::NewLC(16);
	int tmp;
	HAL::Get(HALData::EMemoryRAMFree, tmp);
	res->Des().AppendNum((TInt64)tmp);
	CleanupStack::Pop();
	return res;
}

static TInt RunTelephonyThread(TAny* param) {
	HBufC* res = (HBufC*) param;
	TBool success = EFalse;
	TInt error = KErrNone;
	
	CTrapCleanup* cleanup = CTrapCleanup::New();
	CActiveScheduler* scheduler = new CActiveScheduler();
	CActiveScheduler::Install(scheduler);
	
	CTelephony* telephony = NULL;
	TRAP(error, telephony = CTelephony::NewL());
	if (error == KErrNone && telephony != NULL) {
		CTelephony::TBatteryInfoV1 info;
		CTelephony::TBatteryInfoV1Pckg infoPkg(info);
		TRequestStatus status;
		telephony->GetBatteryInfo(status, infoPkg);
		
		while (CActiveScheduler::RunIfReady(error, CActive::EPriorityIdle) || status.Int() == KRequestPending) {
			;
		}
		
		if (status.Int() == KErrNone) {
			res->Des().AppendNum((TInt64)info.iChargeLevel);
			success = ETrue;
		}
		
		delete telephony;
	}

	CActiveScheduler::Install(NULL);
	delete scheduler;
	delete cleanup;
	
	return success ? 0 : 1;
}

static HBufC* GetBatteryLevelL() {
	HBufC* res = HBufC16::NewL(16);
	
	RThread thread;
	if (thread.Create(KNullDesC, RunTelephonyThread, 0x2000, NULL, res) != KErrNone) {
		delete res;
		return NULL;
	}
	thread.Resume();
	
	TRequestStatus status;
	thread.Logon(status);
	User::WaitForRequest(status);
	
	if (thread.ExitReason() != 0) {
		delete res;
		res = NULL;
	}
	
	thread.Close();
	
	return res;
}

static HBufC* CheckTlsL() {
	HBufC* res = NULL;
#ifdef EKA2
	Swi::RSisRegistrySession session;
	if (session.Connect() == KErrNone) {
		CleanupClosePushL(session);
		
		Swi::RSisRegistryEntry entry;
		CleanupClosePushL(entry);
		if (entry.Open(session, TUid::Uid(0x20292B6B)) == KErrNone) {
			res = HBufC16::NewLC(7);
			res->Des().Append(_L("mbedtls"));
			CleanupStack::Pop();
		}

		CleanupStack::PopAndDestroy(); // entry
		CleanupStack::PopAndDestroy(); // session
		return res;
	}
	
	// fallback for uiq3
	RLibrary lib;
	if (lib.Load(_L("mbedtls.dll")) == KErrNone) {
		res = _L("mbedtls").AllocL();
		
		lib.Close();
	} else {
		res = HBufC16::NewLC(5);
		res->Des().Append(KFalse);
		CleanupStack::Pop();
	}
#else
	RFs fs;
	if (fs.Connect() != KErrNone) return NULL;
	if (BaflUtils::FileExists(fs, _L("C:\\system\\libs\\ssl.dll"))
			|| BaflUtils::FileExists(fs, _L("C:\\system\\libs\\ssladapto2.dll"))
			|| BaflUtils::FileExists(fs, _L("C:\\system\\libs\\ssladaptor.dll"))) {
		res = HBufC16::NewLC(7);
		res->Des().Append(_L("bearssl"));
		CleanupStack::Pop();
	} else {
		res = HBufC16::NewLC(5);
		res->Des().Append(KFalse);
		CleanupStack::Pop();
	}
	fs.Close();
#endif
	return res;
}

static HBufC* IsAppInstalledL(TDesC* property) {
	if ((TUint32)property->Length() <= KIsAppInstalledPrefix.iTypeLength) return NULL;
	TLex lexer(property->Mid(KIsAppInstalledPrefix.iTypeLength));
	TUint32 uid;
	if (lexer.Val(uid, EHex) != KErrNone) return NULL;
	
	RApaLsSession ls;
	User::LeaveIfError(ls.Connect());
	CleanupClosePushL(ls);
	
	TApaAppInfo appInfo;
	HBufC* res = NULL;
	if (ls.GetAppInfo(appInfo, TUid::Uid(uid)) == KErrNone) {
		res = HBufC16::NewLC(4);
		res->Des().Append(KTrue);
		CleanupStack::Pop();
	} else {
		res = HBufC16::NewLC(5);
		res->Des().Append(KFalse);
		CleanupStack::Pop();
	}

	CleanupStack::PopAndDestroy(); // ls
	return res;
}

#ifdef EKA2
static HBufC* GetInstalledSisVersionL(TDesC* property) {
	if ((TUint32)property->Length() <= KGetInstalledSisVersionPrefix.iTypeLength) return NULL;
	TLex lexer(property->Mid(KGetInstalledSisVersionPrefix.iTypeLength));
	TUint32 uid;
	if (lexer.Val(uid, EHex) != KErrNone) return NULL;
	
	Swi::RSisRegistrySession session;
	CleanupClosePushL(session);
	User::LeaveIfError(session.Connect());
	
	Swi::RSisRegistryEntry entry;
	CleanupClosePushL(entry);
	User::LeaveIfError(entry.Open(session, TUid::Uid(uid)));
	
	HBufC* res = NULL;

	TVersion version;
	TRAPD(err, version = entry.VersionL());
	if (err == KErrNone) {
		res = HBufC::NewLC(16);
		TPtr16 des = res->Des();
		des.AppendNum((TInt64)version.iMajor);
		des.Append('.');
		des.AppendNum((TInt64)version.iMinor);
		des.Append('.');
		des.AppendNum((TInt64)version.iBuild);
		CleanupStack::Pop();
	}

	CleanupStack::PopAndDestroy(); // entry
	CleanupStack::PopAndDestroy(); // session
	return res;
}
#endif

static HBufC* LaunchAppL(TDesC* property) {
	if ((TUint32)property->Length() <= KLaunchAppPrefix.iTypeLength) return NULL;
	TLex lexer(property->Mid(KLaunchAppPrefix.iTypeLength));
	TUint32 uid;
	if (lexer.Val(uid, EHex) != KErrNone) return NULL;
	
	RApaLsSession ls;
	User::LeaveIfError(ls.Connect());
	CleanupClosePushL(ls);
	
	TApaAppInfo appInfo;
	HBufC* res = NULL;
	if (ls.GetAppInfo(appInfo, TUid::Uid(uid)) == KErrNone) {
		CApaCommandLine* cli = CApaCommandLine::NewLC();
		cli->SetExecutableNameL(appInfo.iFullName);
		if (ls.StartApp(*cli) == KErrNone) {
			res = HBufC16::NewLC(4);
			res->Des().Append(KTrue);
			CleanupStack::Pop();
		} else {
			res = HBufC16::NewLC(5);
			res->Des().Append(KFalse);
			CleanupStack::Pop();
		}
		CleanupStack::PopAndDestroy(); // cli
	} else {
		res = HBufC16::NewLC(5);
		res->Des().Append(KFalse);
		CleanupStack::Pop();
	}

	CleanupStack::PopAndDestroy(); // ls
	return res;
}

static HBufC* GetVersionL() {
	HBufC* res = HBufC16::NewLC(4);
	res->Des().Append(KApiVersion);
	CleanupStack::Pop();
	return res;
}

HBufC* GetDynamicProperty(void* self, TDesC* property) {
	HBufC* res = NULL;
#ifndef EKA2
	if (!property->Compare(_L("microedition.locale"))) {;
		TRAP_IGNORE(res = (((HBufC* (*)(void*)) ((unsigned int**)self)[0][4]))(self));
		if (res != NULL) return res->Alloc();
		return NULL;
	}
	if (!property->Compare(_L("microedition.timezone"))) {
		TRAP_IGNORE(res = (((HBufC* (*)(void*)) ((unsigned int**)self)[0][5]))(self));
		if (res != NULL) return res->Alloc();
		return NULL;
	}
#endif
	if (!property->Compare(_L("com.nokia.memoryramfree"))) {
		TRAP_IGNORE(res = GetFreeRamL());
		return res;
	}
	if (!property->Compare(_L("com.nokia.mid.batterylevel"))) {
		TRAP_IGNORE(res = GetBatteryLevelL());
		return res;
	}
	if (!property->Compare(_L("cc.nnproject.symbianext.version"))) {
		TRAP_IGNORE(res = GetVersionL());
		return res;
	}
	if (!property->Compare(_L("cc.nnproject.symbianext.checkTLS12"))) {
		TRAP_IGNORE(res = CheckTlsL());
		return res;
	}
	if (property->Find(KIsAppInstalledPrefix) == 0) {
		TRAP_IGNORE(res = IsAppInstalledL(property));
		return res;
	}
	if (property->Find(KLaunchAppPrefix) == 0) {
		TRAP_IGNORE(res = LaunchAppL(property));
		return res;
	}
#ifdef EKA2
	if (property->Find(KGetInstalledSisVersionPrefix) == 0) {
		TRAP_IGNORE(res = GetInstalledSisVersionL(property));
		return res;
	}
#endif
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
	vtable[11] = (unsigned int)GetDynamicProperty;
#else
	vtable[16] = (unsigned int)GetDynamicProperty;
#endif
	runtime[1] = (unsigned int)vtable;
	
	return runtime + 1;
}

#ifndef EKA2
TInt E32Dll(TDllReason) {
	return 0;
}
#endif
