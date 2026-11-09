#include <e32base.h>
#include <d32dbms.h>

_LIT(KUidSSystemAMSDbPolicy, "secure[102045FE]");

#ifdef EKA2
#include "midp2dbv2.h"

static MDatabase2* GetDatabase2L(RLibrary& lib, TUint32 a) {
	void* func = (void*) lib.Lookup(1);
	if (func == NULL) User::Leave(KErrNotSupported);
	return ((MDatabase2* (*)(TUint32)) func)(a);
}

TInt GetId2L(TPtrC aName, TPtrC aVendor) {	
	const TUint32 KDBVersion = 0x0100;

	RLibrary lib;
	User::LeaveIfError(lib.Load(_L("midp2dbv2.dll")));

	RDbs dbs;
	User::LeaveIfError(dbs.Connect());

	RDbNamedDatabase db;
	User::LeaveIfError(db.Open(dbs, KDbFile2(), KUidSSystemAMSDbPolicy));

	MDatabase2* database = GetDatabase2L(lib, KDBVersion);
	MMIDletSuiteTable2* suiteTable = database->MIDletSuiteTableL();

	suiteTable->OpenL(db);

	MMIDletSuiteTableIterator2* i = suiteTable->IteratorLC();

	TBool found = EFalse;
	TUint32 id;

	while (i->HasNext()) {
		TUint32 uid;
		TUint32 flags;
		TPtrC vendor;
		TPtrC name;
		TPtrC version;
		TPtrC protectionDomain;
		TPtrC path;
		TPtrC jadName;
		TPtrC jarName;

		i->NextL(id, uid, flags, vendor, name, version, protectionDomain, path, jadName, jarName);
		if ((aName == name) && (aVendor == vendor)) {
			found = ETrue;
			break;
		}
	}
	CleanupStack::PopAndDestroy(); // i
	suiteTable->Close();
	delete database;
	db.Close();
	dbs.Close();
	if (!found) {
		User::Leave(-1);
	}
	return id;
}
#endif

#include "midp2db.h"

static MDatabase* GetDatabaseL(RLibrary& lib, TUint32 a) {
	void* func = (void*) lib.Lookup(1);
	if (func == NULL) User::Leave(KErrNotSupported);
	return ((MDatabase* (*)(TUint32)) func)(a);
}

TInt GetIdL(TPtrC aName, TPtrC aVendor) {
	const TUint32 KDBVersion = 0x0100;

	RLibrary lib;
	User::LeaveIfError(lib.Load(_L("midp2db.dll")));

	RDbs dbs;
	User::LeaveIfError(dbs.Connect());

	RDbNamedDatabase db;
#ifdef EKA2
	User::LeaveIfError(db.Open(dbs, KDbFile(), KUidSSystemAMSDbPolicy()));
#else
	User::LeaveIfError(db.Open(dbs, KDbFile()));
#endif

	MDatabase* database = GetDatabaseL(lib, KDBVersion);
	MMIDletSuiteInfoTable* suiteInfoTable = database->MIDletSuiteInfoTableL();

	suiteInfoTable->OpenL(db);

	TUint32 id = 0;
	TPtrC version;
	HBufC* someBuf = NULL;
	suiteInfoTable->FindL(aName, aVendor, id, version, someBuf);

	suiteInfoTable->Close();
	delete database;
	db.Close();
	dbs.Close();
	return id;
}

TInt RunL(TPtrC param) {
	TInt res = 0;
	
	TInt commaIdx = param.Locate(',');
		
	if (commaIdx == -1) return KErrArgument;
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
		if (lexer.Val(midletId, EDecimal) != KErrNone) return KErrArgument;
	}
	
	TUint32 suiteId = 0;
#ifdef EKA2
	TRAP(res, suiteId = GetId2L(name, vendor));
	if (res != KErrNotFound && res != KErrNotSupported && res != 0) return res;
	if (res != 0)
#endif
	{
		TRAP(res, suiteId = GetIdL(name, vendor));
		if (res != KErrNone) return res;
	}
	
	RLibrary lib;
	res = lib.Load(_L("midp2client.dll"));
	if (res != KErrNone) {
		return res;
	}
	
#ifdef EKA2
	void* disconnectFunc = (void*) lib.Lookup(1);
	void* connectFunc = (void*) lib.Lookup(4);
#else
	void* disconnectFunc = (void*) lib.Lookup(2);
	void* connectFunc = (void*) lib.Lookup(1);
#endif
	void* launchFunc = (void*) lib.Lookup(6);
	
	if (disconnectFunc != NULL && connectFunc != NULL && launchFunc != NULL) {
		int midpClient[4]; // exact size is unknown
		
		if (((int (*)(void*))connectFunc)(&midpClient) == KErrNone) {
			HBufC* cmdBuf = cmd.Alloc();
			res = ((int (*)(void*, int, int, HBufC*, int))launchFunc)(&midpClient, suiteId, midletId, cmdBuf, 0);
			
			delete cmdBuf;
			
			((int (*)(void*))disconnectFunc)(&midpClient);
		}
	}

	lib.Close();
	
	return res;
}

TInt E32Main() {
	CTrapCleanup* cleanup = CTrapCleanup::New();
	
#ifdef EKA2
	HBufC* cmd = HBufC::New(User::CommandLineLength());
	TPtr des = cmd->Des();
	User::CommandLine(des);
#else
	RProcess process;
	HBufC* cmd = HBufC::New(process.CommandLineLength());
	TPtr des = cmd->Des();
	process.CommandLine(des);
#endif
	
	TInt res;
	TRAP(res, res = RunL(des));
	
	delete cmd;
	delete cleanup;
	
	return res;
}
