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
//_LIT(KGetMidletSuiteUidPrefix, "cc.nnproject.symbianext.getMidletSuiteUid:");
//_LIT(KGetJavaAppUidPrefix, "cc.nnproject.symbianext.getMidletUid:");
_LIT(KLaunchMidletPrefix, "cc.nnproject.symbianext.launchMidlet:");
_LIT(KGetMidletSuiteVersionPrefix, "cc.nnproject.symbianext.getMidletSuiteVersion:");
_LIT(KTrue, "true");
_LIT(KFalse, "false");
_LIT(KApiVersion, "1.0");

typedef HBufC* (*TPropertyFunction0)(void*);
typedef HBufC* (*TPropertyFunction1)(void*, TDesC*);

static HBufC* GetFreeRamL() {
	HBufC* res = HBufC::NewLC(16);
	int tmp;
	HAL::Get(HALData::EMemoryRAMFree, tmp);
	res->Des().AppendNum((TInt)tmp);
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

static HBufC* IsAppInstalledL(TDesC* property) {
	if ((TUint32)property->Length() <= KIsAppInstalledPrefix.iTypeLength) return NULL;
	
	TUint32 uid;
	{
		TPtrC param = property->Mid(KIsAppInstalledPrefix.iTypeLength);
		TLex lexer(param);
		TBool hex = param.Find(_L("0x")) == 0;
		if (hex) lexer.Inc(2);
		if (lexer.Val(uid, hex ? EHex : EDecimal) != KErrNone) return NULL;
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

#ifdef EKA2
static HBufC* GetInstalledSisVersionL(TDesC* property) {
	if ((TUint32)property->Length() <= KGetInstalledSisVersionPrefix.iTypeLength) return NULL;

	TUint32 uid;
	{
		TPtrC param = property->Mid(KGetInstalledSisVersionPrefix.iTypeLength);
		TLex lexer(param);
		TBool hex = param.Find(_L("0x")) == 0;
		if (hex) lexer.Inc(2);
		if (lexer.Val(uid, hex ? EDecimal : EHex) != KErrNone) return NULL;
	}
	
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
		des.AppendNum((TInt)version.iMajor);
		des.Append('.');
		des.AppendNum((TInt)version.iMinor);
		des.Append('.');
		des.AppendNum((TInt)version.iBuild);
		CleanupStack::Pop();
	}

	CleanupStack::PopAndDestroy(); // entry
	CleanupStack::PopAndDestroy(); // session
	return res;
}
#endif

static void LaunchAppUidL(TUid uid, const TDesC& doc = KNullDesC) {
	RApaLsSession ls;
	User::LeaveIfError(ls.Connect());
	CleanupClosePushL(ls);
	
	TApaAppInfo appInfo;
	User::LeaveIfError(ls.GetAppInfo(appInfo, uid));
	
	if (doc.Length() != 0) {
		TThreadId tid;
		ls.StartDocument(doc, uid, tid);
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

static HBufC* LaunchAppL(TDesC* property) {
	if ((TUint32)property->Length() <= KLaunchAppPrefix.iTypeLength) return NULL;
	
	TPtrC param = property->Mid(KLaunchAppPrefix.iTypeLength);
	
	TUint32 uid;
	TPtrC doc(KNullDesC);
	{
		TInt commaIdx = param.Locate(',');
		TBool hex = param.Find(_L("0x")) == 0;
		TLex lexer(commaIdx != -1 ? param.Left(commaIdx) : param);
		if (hex) lexer.Inc(2);
		if (lexer.Val(uid, hex ? EHex : EDecimal) != KErrNone) return NULL;
		
		if (commaIdx != -1) {
			doc.Set(param.Mid(commaIdx + 1));
		}
	}

	HBufC* res = NULL;
	TRAPD(err, LaunchAppUidL(TUid::Uid(uid), doc));
	if (err == KErrNone) {
		res = HBufC16::NewLC(4);
		res->Des().Append(KTrue);
		CleanupStack::Pop();
	} else {
		res = HBufC16::NewLC(5);
		res->Des().Append(KFalse);
		CleanupStack::Pop();
	}
	
	return res;
}

static HBufC* GetVersionL() {
	HBufC* res = HBufC16::NewLC(4);
	res->Des().Append(KApiVersion);
	CleanupStack::Pop();
	return res;
}

static HBufC* GetMidletSuiteVersionL(TDesC* property) {
	if ((TUint32)property->Length() <= KGetMidletSuiteVersionPrefix.iTypeLength) return NULL;
	
	TPtrC param = property->Mid(KGetMidletSuiteVersionPrefix.iTypeLength);
	TInt commaIdx = param.Locate(',');
	
	if (commaIdx == -1) return NULL;
	TPtrC name = param.Left(commaIdx);
	TPtrC vendor = param.Mid(commaIdx + 1);
	
	HBufC* res = NULL;
	RLibrary lib;
	if (lib.Load(_L("javaregistry.dll")) != KErrNone) {
		return NULL;
	}
	
#ifdef EKA2
	void* func = (void*) lib.Lookup(5);
#else
	void* func = (void*) lib.Lookup(3);
#endif
	if (func != NULL) {
		MJavaRegistry* registry = ((MJavaRegistry* (*)(void))func)();
		if (registry != NULL) {
			MJavaRegistrySuiteEntry* entry = NULL;
			TRAPD(err, entry = registry->SuiteEntryL(name, vendor));
			if (err == KErrNone && entry != NULL) {
				TAppVersion version = entry->VersionL();
				res = HBufC16::NewLC(16);
				TPtr16 des = res->Des();
				des.AppendNum((TInt)version.iMajor);
				des.Append('.');
				des.AppendNum((TInt)version.iMinor);
				des.Append('.');
				des.AppendNum((TInt)version.iBuild);
				CleanupStack::Pop();
			}
			registry->Release();
		}
	}
	
	lib.Close();
	return res;
}

static HBufC* LaunchMidletL(TDesC* property) {
	if ((TUint32)property->Length() <= KLaunchMidletPrefix.iTypeLength) return NULL;
	
	TPtrC param = property->Mid(KLaunchMidletPrefix.iTypeLength);
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
	
	HBufC* res = NULL;
	RLibrary lib;
	if (lib.Load(_L("javaregistry.dll")) != KErrNone) {
		return NULL;
	}
		
#ifdef EKA2
	void* func = (void*) lib.Lookup(5);
#else
	void* func = (void*) lib.Lookup(3);
#endif
	if (func != NULL) {
		MJavaRegistry* registry = ((MJavaRegistry* (*)(void))func)();
		if (registry != NULL) {
			MJavaRegistrySuiteEntry* suite = NULL;
			TRAPD(err, suite = registry->SuiteEntryL(name, vendor));
			if (err != KErrNone || suite == NULL) goto ret2;
			
			MJavaRegistryMIDletEntry* midlet = NULL;
			TRAP(err, midlet = suite->MIDletByNumberL(midletId));
			if (err != KErrNone || midlet == NULL) goto ret2;
			
			TUid uid = TUid::Null();
			TRAP(err, uid = midlet->UidL());
			if (err != KErrNone) goto ret2;
			
//			TRAP(err, LaunchAppUidL(uid));
//			if (err == KErrNone) {
//				res = HBufC16::NewLC(4);
//				res->Des().Append(KTrue);
//				CleanupStack::Pop();
//			} else {
//				res = HBufC16::NewLC(5);
//				res->Des().Append(KFalse);
//				CleanupStack::Pop();
//			}
			
//			HBufC* runcmd = NULL;
//			TRAP(err, runcmd = HBufC::NewL(param.Length() + 32));
//			if (err != KErrNone || runcmd == NULL) goto ret2;			
//			TPtr des = runcmd->Des();
//			des.AppendFormat(_L("%x %d "), suite->UidL().iUid, midletId);
//			des.Append(cmd);
			
			RProcess process;
//			if (process.Create(_L("midp2launcher.exe"), des) != KErrNone) goto ret2;
			if (process.Create(_L("midp2launcher.exe"), param) != KErrNone) goto ret2;
			
			TRequestStatus status;
			process.Logon(status);
			process.Resume();
			User::WaitForRequest(status);
			
			
//			if ( == 0) {
				res = HBufC16::NewLC(10);
				res->Des().AppendNum((TInt)process.ExitReason());
				CleanupStack::Pop();
//			}
				
			process.Close();
//			delete runcmd;
		}
		
		ret2:
		if (registry != NULL) registry->Release();
	}
	if (res == NULL) {
		res = HBufC16::NewLC(5);
		res->Des().Append(KFalse);
		CleanupStack::Pop();
	}

	lib.Close();
	return res;
}

HBufC* GetDynamicProperty(void* self, TDesC* property) {
	HBufC* res = NULL;
#ifndef EKA2
	if (!property->Compare(_L("microedition.locale"))) {
			TRAP_IGNORE(res = (*(TPropertyFunction0**)self)[4](self));
			if (res != NULL) return res->Alloc();
			return NULL;
		}
		if (!property->Compare(_L("microedition.timezone"))) {
			TRAP_IGNORE(res = (*(TPropertyFunction0**)self)[5](self));
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
//	if (property->Find(KGetMidletSuiteUidPrefix) == 0) {
//		TRAP_IGNORE(res = GetMidletSuiteUidL(property));
//		return res;
//	}
	if (property->Find(KGetMidletSuiteVersionPrefix) == 0) {
		TRAP_IGNORE(res = GetMidletSuiteVersionL(property));
		return res;
	}
	if (property->Find(KLaunchMidletPrefix) == 0) {
		TRAP_IGNORE(res = LaunchMidletL(property));
		return res;
	}
#ifdef EKA2
	if (property->Find(KGetInstalledSisVersionPrefix) == 0) {
		TRAP_IGNORE(res = GetInstalledSisVersionL(property));
		return res;
	}

	return (*(TPropertyFunction1**)self)[sizeof_Runtime_vtable >> 2](self, property);
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
