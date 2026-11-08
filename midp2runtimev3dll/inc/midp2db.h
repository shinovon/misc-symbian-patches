#ifdef EKA2
_LIT(KDbFile, "c:MIDP2SystemAMSStatic");
#else
_LIT(KDbFile, "c:\\system\\data\\midp2\\systemams\\MIDP2SystemAMSStatic.db2");
#endif

class MMIDletSuiteInfoTable {
public:
	virtual ~MMIDletSuiteInfoTable();
	virtual void CreateL();
	virtual void OpenL(RDbNamedDatabase&);
	virtual void Close();
	virtual void AddL();
	virtual void RemoveL();
	virtual void FindL(const TDesC& name, const TDesC& vendor, TUint32& id, TPtrC& version, HBufC*&);
	virtual void GetL(TUint32& id, TPtrC& name, TPtrC& vendor, TPtrC& version);
};

class MDatabase {
public:
	virtual ~MDatabase();
	virtual void* AlarmTableL();
	virtual void* CertificateTableL();
	virtual void* CertificateChainIdTableL();
	virtual void* DynamicConnectionTableL();
	virtual void* StaticConnectionTableL();
	virtual void* JarEncodingTableL();
	virtual MMIDletSuiteInfoTable* MIDletSuiteInfoTableL();
	virtual void* MIDletSuiteTableL();
	virtual void* MIDletTableL();
	virtual void* ProtectionDomainTableL();
	virtual void* SecurityAttributeTableL();
	virtual void* UninstallLaterTableL();
	virtual void* UserSecurityPreferencesTableL();
	virtual void* APNTableL();
	virtual void* SilentInstallTableL();
	virtual void* CustomAttribTableL();
};
