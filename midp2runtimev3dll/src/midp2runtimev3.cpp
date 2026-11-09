#include <e32base.h>
#include <hal.h>
#include <hal_data.h>
#include <etel3rdparty.h>
#include <bautils.h>
#include <apgcli.h>
#include <apacmdln.h>
#include <apgicnfl.h>
#include <apgtask.h>
#include <barsc.h>
#include <w32std.h>
#include "mjavaregistry.h"
#include "appversion.h"
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
_LIT(KGetMidletUidPrefix, "cc.nnproject.symbianext.getMidletUid:");
_LIT(KLaunchMidletPrefix, "cc.nnproject.symbianext.launchMidlet:");
_LIT(KGetMidletSuiteVersionPrefix, "cc.nnproject.symbianext.getMidletSuiteVersion:");
_LIT(KIsAppRunningPrefix, "cc.nnproject.symbianext.isAppRunning:");
_LIT(KBringAppToForegroundPrefix, "cc.nnproject.symbianext.bringAppToForeground:");
_LIT(KTrue, "true");
_LIT(KFalse, "false");
_LIT(KApiVersion, "1.0");

typedef HBufC* (*TPropertyFunction0)(void*);
typedef HBufC* (*TPropertyFunction1)(void*, TDesC*);

static TBool ParseInt(TPtrC aDes, TUint32& aVal) {
	TLex lexer(aDes);
	TBool hex = aDes.Find(_L("0x")) == 0;
	if (hex) lexer.Inc(2);
	return lexer.Val(aVal, hex ? EHex : EDecimal) == KErrNone;
}

static HBufC* GetFreeRamL() {
	HBufC* res = HBufC::NewLC(16);
	int tmp;
	HAL::Get(HALData::EMemoryRAMFree, tmp);
	res->Des().AppendNum((TInt)tmp);
	CleanupStack::Pop();
	return res;
}

static TInt RunTelephonyThread(TAny* aParam) {
	HBufC* res = (HBufC*) aParam;
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
			res->Des().AppendNum((TInt)info.iChargeLevel);
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
	
	// may be installed manually
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

static HBufC* IsAppInstalledL(TDesC* aProperty) {
	if ((TUint32)aProperty->Length() <= KIsAppInstalledPrefix.iTypeLength) return NULL;
	
	TUint32 uid;
	if (!ParseInt(aProperty->Mid(KIsAppInstalledPrefix.iTypeLength), uid)) {
		return NULL;
	}
	
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

static HBufC* IsAppRunningL(TDesC* aProperty) {
	if ((TUint32)aProperty->Length() <= KIsAppRunningPrefix.iTypeLength) return NULL;
	
	TUint32 uid;
	if (!ParseInt(aProperty->Mid(KIsAppRunningPrefix.iTypeLength), uid)) {
		return NULL;
	}
	
	RWsSession ws;
	User::LeaveIfError(ws.Connect());
	CleanupClosePushL(ws);
		
	TApaTaskList tasklist(ws);
	TApaTask task = tasklist.FindApp(TUid::Uid(uid));

	HBufC* res = NULL;
	if (task.Exists()) {
		res = HBufC16::NewLC(4);
		res->Des().Append(KTrue);
		CleanupStack::Pop();
	} else {
		res = HBufC16::NewLC(5);
		res->Des().Append(KFalse);
		CleanupStack::Pop();
	}

	CleanupStack::PopAndDestroy(); // ws
	return res;
}

static HBufC* BringAppToForegroundL(TDesC* aProperty) {
	if ((TUint32)aProperty->Length() <= KBringAppToForegroundPrefix.iTypeLength) return NULL;
	
	TUint32 uid;
	if (!ParseInt(aProperty->Mid(KBringAppToForegroundPrefix.iTypeLength), uid)) {
		return NULL;
	}
	
	RWsSession ws;
	User::LeaveIfError(ws.Connect());
	CleanupClosePushL(ws);
		
	TApaTaskList tasklist(ws);
	TApaTask task = tasklist.FindApp(TUid::Uid(uid));

	HBufC* res = NULL;
	if (task.Exists()) {
		task.BringToForeground();
		// TODO
//		task.SendMessage(TUid::Uid(0), doc);
		
		res = HBufC16::NewLC(4);
		res->Des().Append(_L("true"));
		CleanupStack::Pop();
	} else {
		res = HBufC16::NewLC(5);
		res->Des().Append(_L("false")); // not found
		CleanupStack::Pop();
	}

	CleanupStack::PopAndDestroy(); // ws
	return res;
}

#ifdef EKA2
static HBufC* GetInstalledSisVersionL(TDesC* aProperty) {
	if ((TUint32)aProperty->Length() <= KGetInstalledSisVersionPrefix.iTypeLength) return NULL;

	TUint32 uid;
	if (!ParseInt(aProperty->Mid(KGetInstalledSisVersionPrefix.iTypeLength), uid)) {
		return NULL;
	}
	
	Swi::RSisRegistrySession session;
	CleanupClosePushL(session);
	User::LeaveIfError(session.Connect());
	
	Swi::RSisRegistryEntry entry;
	CleanupClosePushL(entry);
	User::LeaveIfError(entry.Open(session, TUid::Uid(uid)));
	
	TVersion version = entry.VersionL();
	
	HBufC* res = res = HBufC::NewLC(16);
	TPtr16 des = res->Des();
	des.AppendNum((TInt)version.iMajor);
	des.Append('.');
	des.AppendNum((TInt)version.iMinor);
	des.Append('.');
	des.AppendNum((TInt)version.iBuild);
	CleanupStack::Pop();

	CleanupStack::PopAndDestroy(); // entry
	CleanupStack::PopAndDestroy(); // session
	return res;
}
#endif

static void DoLaunchAppUidL(TUid aUid, const TDesC& aDoc = KNullDesC) {
	RApaLsSession ls;
	User::LeaveIfError(ls.Connect());
	CleanupClosePushL(ls);
	
	TApaAppInfo appInfo;
	User::LeaveIfError(ls.GetAppInfo(appInfo, aUid));
	
	if (aDoc.Length() != 0) {
		TThreadId tid;
		User::LeaveIfError(ls.StartDocument(aDoc, aUid, tid));
	} else {
		CApaCommandLine* cli = CApaCommandLine::NewLC();
		cli->SetExecutableNameL(appInfo.iFullName);
//		if (doc.Length() != 0) {
//			cli->SetDocumentNameL(doc);
//		}
		User::LeaveIfError(ls.StartApp(*cli));
		CleanupStack::PopAndDestroy(); // cli
	}

	CleanupStack::PopAndDestroy(); // ls
////		RWsSession 		ws;
////		User::LeaveIfError(ws.Connect());
////		CleanupClosePushL( ws );
////			
////		TApaTaskList tasklist(ws);
////		TApaTask task = tasklist.FindApp(uid);
////
////		if (task.Exists()) {
////			task.BringToForeground();
////			task.SendMessage(TUid::Uid(0), doc);
////		} else {
//		RApaLsSession ls;
//		User::LeaveIfError(ls.Connect());
//		CleanupClosePushL(ls);
//
//		TThreadId tid;
//		ls.StartDocument(doc, uid, tid);
//		
//		CleanupStack::PopAndDestroy(); // ls
////		}
////		CleanupStack::PopAndDestroy(); // ws
}

static HBufC* LaunchAppL(TDesC* aProperty) {
	if ((TUint32)aProperty->Length() <= KLaunchAppPrefix.iTypeLength) return NULL;
	
	TPtrC param = aProperty->Mid(KLaunchAppPrefix.iTypeLength);
	
	TUint32 uid;
	TPtrC doc(KNullDesC);
	{
		TInt commaIdx = param.Locate(',');
		if (!ParseInt(commaIdx != -1 ? param.Left(commaIdx) : param, uid)) {
			return NULL;
		}
		
		if (commaIdx != -1) {
			doc.Set(param.Mid(commaIdx + 1));
		}
	}

	TRAPD(err, DoLaunchAppUidL(TUid::Uid(uid), doc));
	
	HBufC* res = res = HBufC16::NewLC(16);
	res->Des().AppendNum((TInt)err);
	CleanupStack::Pop();
	
	return res;
}

static HBufC* GetVersionL() {
	HBufC* res = HBufC16::NewLC(4);
	res->Des().Append(KApiVersion);
	CleanupStack::Pop();
	return res;
}

static HBufC* GetMidletSuiteVersionL(TDesC* aProperty) {
	if ((TUint32)aProperty->Length() <= KGetMidletSuiteVersionPrefix.iTypeLength) return NULL;
	
	TPtrC param = aProperty->Mid(KGetMidletSuiteVersionPrefix.iTypeLength);
	TInt commaIdx = param.Locate(',');
	
	if (commaIdx == -1) return NULL;
	TPtrC name = param.Left(commaIdx);
	TPtrC vendor = param.Mid(commaIdx + 1);
	
	HBufC* res = NULL;
	RLibrary lib;
	if (lib.Load(_L("javaregistry.dll")) != KErrNone) {
		return NULL;
	}
	CleanupClosePushL(lib);
	
#ifdef EKA2
	void* func = (void*) lib.Lookup(5);
#else
	void* func = (void*) lib.Lookup(3);
#endif
	if (!func) User::Leave(KErrNotSupported);
	
	MJavaRegistry* registry = ((MJavaRegistry* (*)(void))func)();
	User::LeaveIfNull(registry);
	CleanupReleasePushL(*registry);
	
	MJavaRegistrySuiteEntry* entry = registry->SuiteEntryL(name, vendor);
	
	TAppVersion version = entry->VersionL();
	res = HBufC16::NewLC(16);
	TPtr16 des = res->Des();
	des.AppendNum((TInt)version.iMajor);
	des.Append('.');
	des.AppendNum((TInt)version.iMinor);
	des.Append('.');
	des.AppendNum((TInt)version.iBuild);
	CleanupStack::Pop();
	
	CleanupStack::PopAndDestroy(); // registry
	CleanupStack::PopAndDestroy(); // lib
	
	return res;
}

static HBufC* GetMidletUidL(TDesC* aProperty) {
	if ((TUint32)aProperty->Length() <= KGetMidletUidPrefix.iTypeLength) return NULL;
	
	TPtrC param = aProperty->Mid(KGetMidletUidPrefix.iTypeLength);
	TInt commaIdx = param.Locate(',');
	
	if (commaIdx == -1) return NULL;
	TPtrC name = param.Left(commaIdx);
	TPtrC vendor = param.Mid(commaIdx + 1);
	TUint32 midletId = 1;
	
	commaIdx = vendor.Locate(',');
	if (commaIdx != -1) {
		TPtrC midletIdDes = vendor.Mid(commaIdx + 1);
		vendor.Set(vendor.Left(commaIdx));
		
		TLex lexer(midletIdDes);
		if (lexer.Val(midletId, EDecimal) != KErrNone) return NULL;
	}
	
	HBufC* res = NULL;
	RLibrary lib;
	if (lib.Load(_L("javaregistry.dll")) != KErrNone) {
		return NULL;
	}
	CleanupClosePushL(lib);
	
#ifdef EKA2
	void* func = (void*) lib.Lookup(5);
#else
	void* func = (void*) lib.Lookup(3);
#endif
	if (!func) User::Leave(KErrNotSupported);
	
	MJavaRegistry* registry = ((MJavaRegistry* (*)(void))func)();
	User::LeaveIfNull(registry);
	CleanupReleasePushL(*registry);

	MJavaRegistrySuiteEntry* suite = registry->SuiteEntryL(name, vendor);
	MJavaRegistryMIDletEntry* midlet = suite->MIDletByNumberL(midletId);
	TUid uid = midlet->UidL();

	res = HBufC16::NewLC(16);
	res->Des().AppendFormat(_L("0x%x"), midlet->UidL().iUid);
	CleanupStack::Pop();
	
	CleanupStack::PopAndDestroy(); // registry
	CleanupStack::PopAndDestroy(); // lib
	
	return res;
}

static void DoLaunchMidletL(TPtrC aParam, TPtrC aName, TPtrC aVendor, TUint32 aMidletId) {
	RLibrary lib;
	User::LeaveIfError(lib.Load(_L("javaregistry.dll")));
	CleanupClosePushL(lib);
		
#ifdef EKA2
	void* func = (void*) lib.Lookup(5);
#else
	void* func = (void*) lib.Lookup(3);
#endif
	if (!func) User::Leave(KErrNotSupported);
	
	MJavaRegistry* registry = ((MJavaRegistry* (*)(void))func)();
	User::LeaveIfNull(registry);
	CleanupReleasePushL(*registry);
	
	MJavaRegistrySuiteEntry* suite = registry->SuiteEntryL(aName, aVendor);
	MJavaRegistryMIDletEntry* midlet = suite->MIDletByNumberL(aMidletId);
	TUid uid = midlet->UidL();
	
	RProcess process;
	CleanupClosePushL(process);
	User::LeaveIfError(process.Create(_L("midp2launcher.exe"), aParam));
	
	TRequestStatus status;
	process.Logon(status);
	process.Resume();
	User::WaitForRequest(status);
	
	TInt reason = process.ExitReason();
	if (reason != 0) User::Leave(reason);

	CleanupStack::PopAndDestroy(); // process
	CleanupStack::PopAndDestroy(); // registry
	CleanupStack::PopAndDestroy(); // lib
}

static HBufC* LaunchMidletL(TDesC* aProperty) {
	if ((TUint32)aProperty->Length() <= KLaunchMidletPrefix.iTypeLength) return NULL;
	
	TPtrC param = aProperty->Mid(KLaunchMidletPrefix.iTypeLength);
	TInt commaIdx = param.Locate(',');
	
	if (commaIdx == -1) return NULL;
	TPtrC name = param.Left(commaIdx);
	TPtrC vendor = param.Mid(commaIdx + 1);
	TUint32 midletId = 1;
	TPtrC cmd(KNullDesC);
	
	commaIdx = vendor.Locate(',');
	if (commaIdx != -1) {
		TPtrC midletIdDes = vendor.Mid(commaIdx + 1);
		vendor.Set(vendor.Left(commaIdx));
		
		commaIdx = midletIdDes.Locate(',');
		if (commaIdx != -1) {
			cmd.Set(midletIdDes.Mid(commaIdx + 1));
			midletIdDes.Set(midletIdDes.Left(commaIdx));
		}
		
		TLex lexer(midletIdDes);
		if (lexer.Val(midletId, EDecimal) != KErrNone) return NULL;
	}
	
	TRAPD(err, DoLaunchMidletL(param, name, vendor, midletId))
	HBufC* res = HBufC16::NewLC(16);
	res->Des().AppendNum((TInt)err);
	CleanupStack::Pop();
	return res;
}

HBufC* GetDynamicProperty(void* aThis, TDesC* aProperty) {
	HBufC* res = NULL;
#ifndef EKA2
	if (!aProperty->Compare(_L("microedition.locale"))) {
			TRAP_IGNORE(res = (*(TPropertyFunction0**)aThis)[4](aThis));
			if (res != NULL) return res->Alloc();
			return NULL;
		}
		if (!aProperty->Compare(_L("microedition.timezone"))) {
			TRAP_IGNORE(res = (*(TPropertyFunction0**)aThis)[5](aThis));
			if (res != NULL) return res->Alloc();
			return NULL;
		}
#endif
	if (!aProperty->Compare(_L("com.nokia.memoryramfree"))) {
		TRAP_IGNORE(res = GetFreeRamL());
		return res;
	}
	if (!aProperty->Compare(_L("com.nokia.mid.batterylevel"))) {
		TRAP_IGNORE(res = GetBatteryLevelL());
		return res;
	}
	if (!aProperty->Compare(_L("cc.nnproject.symbianext.version"))) {
		TRAP_IGNORE(res = GetVersionL());
		return res;
	}
	if (!aProperty->Compare(_L("cc.nnproject.symbianext.checkTLS12"))) {
		TRAP_IGNORE(res = CheckTlsL());
		return res;
	}
	if (aProperty->Find(KIsAppInstalledPrefix) == 0) {
		TRAP_IGNORE(res = IsAppInstalledL(aProperty));
		return res;
	}
	if (aProperty->Find(KLaunchAppPrefix) == 0) {
		TRAP_IGNORE(res = LaunchAppL(aProperty));
		return res;
	}
	if (aProperty->Find(KGetMidletUidPrefix) == 0) {
		TRAP_IGNORE(res = GetMidletUidL(aProperty));
		return res;
	}
	if (aProperty->Find(KGetMidletSuiteVersionPrefix) == 0) {
		TRAP_IGNORE(res = GetMidletSuiteVersionL(aProperty));
		return res;
	}
	if (aProperty->Find(KLaunchMidletPrefix) == 0) {
		TRAP_IGNORE(res = LaunchMidletL(aProperty));
		return res;
	}
	if (aProperty->Find(KIsAppRunningPrefix) == 0) {
		TRAP_IGNORE(res = IsAppRunningL(aProperty));
		return res;
	}
	if (aProperty->Find(KBringAppToForegroundPrefix) == 0) {
		TRAP_IGNORE(res = BringAppToForegroundL(aProperty));
		return res;
	}
#ifdef EKA2
	if (aProperty->Find(KGetInstalledSisVersionPrefix) == 0) {
		TRAP_IGNORE(res = GetInstalledSisVersionL(aProperty));
		return res;
	}

	return (*(TPropertyFunction1**)aThis)[sizeof_Runtime_vtable >> 2](aThis, aProperty);
#else
	return NULL;
#endif
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
	vtable[sizeof_Runtime_vtable >> 2] = vtable[11]; // store pointer to original function
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
