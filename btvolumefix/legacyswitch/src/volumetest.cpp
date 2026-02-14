#include <e32base.h>
#include <centralrepository.h>
#include <aknglobalnote.h>

LOCAL_C void MainL() {
	CAknGlobalNote* dialog = CAknGlobalNote::NewL();
	CleanupStack::PushL(dialog);
	CRepository* cenrep = NULL;
	TUid KCRUidBluetoothEngine = { 0x10204DAB };
	TRAP_IGNORE(cenrep = CRepository::NewL(KCRUidBluetoothEngine));
	if (cenrep) {
		CleanupStack::PushL(cenrep);
		TInt value = 0;
		TInt err = cenrep->Get(8, value);
		if (err != KErrNone || cenrep->Set(8, value == 1 ? 0 : 1) != KErrNone) {
			dialog->ShowNoteL(EAknGlobalErrorNote, _L("CRepository4All patch not enabled!"));
		} else if (value == 1) {
			dialog->ShowNoteL(EAknGlobalInformationNote, _L("Bluetooth legacy volume control disabled"));
		} else {
			dialog->ShowNoteL(EAknGlobalInformationNote, _L("Bluetooth legacy volume control enabled"));
		}
		CleanupStack::PopAndDestroy(cenrep);
	} else {
		dialog->ShowNoteL(EAknGlobalErrorNote, _L("Failed to open repository!"));
	}
	CleanupStack::PopAndDestroy(dialog);
}

GLDEF_C TInt E32Main() {
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TRAPD(error, MainL());
	delete cleanup;
	return error;
}
