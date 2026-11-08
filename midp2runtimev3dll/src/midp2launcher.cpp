#include <e32base.h>

TInt Run(TPtrC cmd) {
	TInt res = 0;
	
	TUint32 uid;
	TInt midletId;
	{
		TInt idx = cmd.Locate(' ');
		if (idx == -1) return -1;
		{
			TPtrC uidDes = cmd.Left(idx);
			cmd.Set(cmd.Mid(idx + 1));
				
			TLex lex(uidDes);
			if (lex.Val(uid) != KErrNone) return -1;
		}

		idx = cmd.Locate(' ');
		if (idx == -1) return -1;
		{
			TPtrC numDes = cmd.Left(idx);
			cmd.Set(cmd.Mid(idx + 1));
				
			TLex lex(numDes);
			if (lex.Val(midletId) != KErrNone) return -1;
		}
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
			res = ((int (*)(void*, int, int, HBufC*, int))launchFunc)(&midpClient, uid, midletId, cmdBuf, 0);
			
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
	TRAP(res, res = Run(des));
	
	delete cmd;
	delete cleanup;
	
	return res;
}
