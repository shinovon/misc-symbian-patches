_LIT(KDbFile2, "c:MIDP2SystemAMSStaticV2");

class MMIDletSuiteTableIterator2 {
public:
	virtual ~MMIDletSuiteTableIterator2();
	virtual bool HasNext();
	virtual void NextL(TUint32&, TUint32&, TUint32&, TPtrC&, TPtrC&, TPtrC&, TPtrC&, TPtrC&, TPtrC&, TPtrC&);
};

class MMIDletSuiteTable2 {
public:
	virtual void CreateL();
	virtual void OpenL(RDbNamedDatabase&);
	virtual void Close();
	virtual void AddL();
	virtual void RemoveL();
	virtual void UpdateL();
	virtual MMIDletSuiteTableIterator2* IteratorLC();
};

class MDatabase2 {
public:
	virtual ~MDatabase2();
	virtual void* AlarmTableL();
	virtual void* CHAccessTableL();
	virtual void* CHActionsTableL();
	virtual void* CHLocalActionNameTableL();
	virtual void* CHSuffixesTableL();
	virtual void* CHTypesTableL();
	virtual void* ComponentPropertyTableL();
	virtual void* DynamicConnectionTableL();
	virtual void* StaticConnectionTableL();
	virtual void* ContentHandlerTableL();
	virtual void* MIDletPropertyTableL();
	virtual void* MIDletSuitePropertyTableL();
	virtual MMIDletSuiteTable2* MIDletSuiteTableL();
	virtual void* MIDletTableL();
	virtual void* SecurityAttributeTableL();
	virtual void* UserSecurityPreferencesTableL();
	virtual void* CustomAttribTableL();
};
