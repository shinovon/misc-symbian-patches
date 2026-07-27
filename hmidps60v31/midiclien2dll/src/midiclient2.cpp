#include <midiclientutility.h>
#include <remconcoreapitargetobserver.h>
#include <remconinterfaceselector.h>
#include <remconcoreapitarget.h>

#define MAX_VOLUME 10
#define DEFAULT_VOLUME 7
#define VOLUME_STEP 1

class CMidiClientUtiliti : public CBase
	{
	class CBody;
public:
	IMPORT_C static CMidiClientUtiliti* NewL(MMidiClientUtilityObserver& aObserver,
											 TInt aPriority = EMdaPriorityNormal,
											 TMdaPriorityPreference aPref = EMdaPriorityPreferenceTimeAndQuality);
	
	// alternative NewL for selecting shared/separate controller heap(s)
	IMPORT_C static CMidiClientUtiliti* NewL(MMidiClientUtilityObserver& aObserver,
											 TInt aPriority,
											 TMdaPriorityPreference aPref,
											 TBool aUseSharedHeap);

	/**	Destructor */
	virtual ~CMidiClientUtiliti();
	IMPORT_C TMidiState State() const;
	IMPORT_C void OpenFile(const TDesC& aFileName);
	IMPORT_C void OpenFile(const RFile& aFile);

	IMPORT_C void OpenFile(const TMMSource& aSource);

	IMPORT_C void OpenDes(const TDesC8& aDescriptor);
	IMPORT_C void OpenUrl(const TDesC& aUrl,TInt aIapId = KUseDefaultIap,const TDesC8& aMimeType=KNullDesC8);
	IMPORT_C void Close();
	IMPORT_C void Play();
	IMPORT_C void Stop(const TTimeIntervalMicroSeconds& aFadeOutDuration);
	IMPORT_C void PlayNoteL(TInt aChannel,TInt aNote,const TTimeIntervalMicroSeconds& aDuration,TInt aNoteOnVelocity,TInt aNoteOffVelocity);
	IMPORT_C void PlayNoteL(TInt aChannel,TInt aNote,const TTimeIntervalMicroSeconds& aStartTime,const TTimeIntervalMicroSeconds& aDuration,TInt aNoteOnVelocity,TInt aNoteOffVelocity);
	IMPORT_C void StopNotes(TInt aChannel);
	IMPORT_C void NoteOnL(TInt aChannel,TInt aNote,TInt aVelocity);
	IMPORT_C void NoteOffL(TInt aChannel,TInt aNote,TInt aVelocity);
	IMPORT_C TInt PlaybackRateL() const;
	IMPORT_C void SetPlaybackRateL(TInt aRate);
	IMPORT_C TInt MaxPlaybackRateL() const;
	IMPORT_C TInt MinPlaybackRateL() const;
	IMPORT_C TInt TempoMicroBeatsPerMinuteL() const;
	IMPORT_C void SetTempoL(TInt aMicroBeatsPerMinute);
	IMPORT_C TInt PitchTranspositionCentsL() const;
	IMPORT_C TInt SetPitchTranspositionL(TInt aCents);
	IMPORT_C TTimeIntervalMicroSeconds DurationMicroSecondsL() const;
	IMPORT_C TInt64 DurationMicroBeatsL() const;
	IMPORT_C TInt NumTracksL() const;
	IMPORT_C void SetTrackMuteL(TInt aTrack,TBool aMuted) const;
	IMPORT_C const TDesC8& MimeTypeL();
	IMPORT_C TTimeIntervalMicroSeconds PositionMicroSecondsL() const;
	IMPORT_C void SetPositionMicroSecondsL(const TTimeIntervalMicroSeconds& aPosition);
	IMPORT_C TInt64 PositionMicroBeatsL() const;
	IMPORT_C void SetPositionMicroBeatsL(TInt64 aMicroBeats);
	IMPORT_C void SetSyncUpdateCallbackIntervalL(const TTimeIntervalMicroSeconds& aMicroSeconds, TInt64 aMicroBeats=0);		
	IMPORT_C TInt SendMessageL(const TDesC8& aMidiMessage);
	IMPORT_C TInt SendMessageL(const TDesC8& aMidiMessage,const TTimeIntervalMicroSeconds& aTime);
	IMPORT_C void SendMipMessageL(const RArray<TMipMessageEntry>& aEntry);
	IMPORT_C TInt NumberOfBanksL(TBool aCustom) const;
	IMPORT_C TInt GetBankIdL(TBool aCustom,TInt aBankIndex) const;
	IMPORT_C void LoadCustomBankL(const TDesC& aFileName,TInt& aBankCollectionIndex);
	IMPORT_C void LoadCustomBankDataL(const TDesC8& aBankData,TInt& aBankCollectionIndex);
	IMPORT_C void UnloadCustomBankL(TInt aBankCollectionIndex);
	IMPORT_C TBool CustomBankLoadedL(TInt aBankCollectionIndex) const;
	IMPORT_C void UnloadAllCustomBanksL();
	IMPORT_C TInt NumberOfInstrumentsL(TInt aBankId,TBool aCustom) const;
	IMPORT_C TInt GetInstrumentIdL(TInt aBankId,TBool aCustom,TInt aInstrumentIndex) const;
	IMPORT_C HBufC* InstrumentNameL(TInt aBankId, TBool aCustom, TInt aInstrumentId) const;
	IMPORT_C void GetInstrumentL(TInt aChannel, TInt& aInstrumentId, TInt& aBankId);
	IMPORT_C void SetInstrumentL(TInt aChannel,TInt aBankId,TInt aInstrumentId);
	IMPORT_C void LoadCustomInstrumentL(const TDesC& aFileName,TInt aFileBankId,TInt aFileInstrumentId,TInt aMemoryBankId,TInt aMemoryInstrumentId);
	IMPORT_C void LoadCustomInstrumentDataL(const TDesC8& aInstrumentData, TInt aBankDataId, TInt aInstrumentDataId, TInt aMemoryBankId, TInt aMemoryInstrumentId);
	IMPORT_C void UnloadCustomInstrumentL(TInt aCustomBankId,TInt aInstrumentId);
	IMPORT_C HBufC* PercussionKeyNameL(TInt aNote, TInt aBankId, TBool aCustom, TInt aInstrumentId) const;
	IMPORT_C void StopTimeL(TTimeIntervalMicroSeconds& aStopTime) const;
	IMPORT_C void SetStopTimeL(const TTimeIntervalMicroSeconds& aStopTime);
	IMPORT_C void SetRepeatsL(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence);
	IMPORT_C TInt GetRepeats() const;
	IMPORT_C TInt PolyphonyL() const;
	IMPORT_C void SetMaxPolyphonyL(TInt aMaxNotes);
	IMPORT_C TInt ChannelsSupportedL() const;
	IMPORT_C TReal32 ChannelVolumeL(TInt aChannel) const;
	IMPORT_C TReal32 MaxChannelVolumeL() const;
	IMPORT_C void SetChannelVolumeL(TInt aChannel,TReal32 aVolumeInDecibels);
	IMPORT_C void SetChannelMuteL(TInt aChannel,TBool aMuted);
	IMPORT_C TInt VolumeL() const;
	IMPORT_C TInt MaxVolumeL() const;
	IMPORT_C void SetVolumeL(TInt aVolume);
	IMPORT_C void SetVolumeRampL(const TTimeIntervalMicroSeconds& aRampDuration);
	IMPORT_C TInt GetBalanceL() const;
	IMPORT_C void SetBalanceL(TInt aBalance = KMMFBalanceCenter);
	IMPORT_C void SetPriorityL(TInt aPriority, TMdaPriorityPreference aPref);
	IMPORT_C TInt NumberOfMetaDataEntriesL() const;
	IMPORT_C CMMFMetaDataEntry* GetMetaDataEntryL(TInt aMetaDataIndex) const;
/**
 * @deprecated SetBankL is deprecated due to Mobile XMF specification requirement that user instruments (custom instruments) override standard instruments.
 */
	IMPORT_C void SetBankL(TBool aCustom);
	IMPORT_C TBool IsTrackMuteL(TInt aTrack) const;
	IMPORT_C TBool IsChannelMuteL(TInt aChannel) const;
	IMPORT_C void CustomCommandSyncL(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom);
	IMPORT_C void CustomCommandSyncL(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2);
	IMPORT_C void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus);
	IMPORT_C void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus);
	IMPORT_C TInt MaxPolyphonyL() const;

	IMPORT_C MMMFDRMCustomCommand* GetDRMCustomCommand();
	
private:
	/**
	 * a pointer to the implementation.
	 */
	CBody* iBody;	
	friend class CBody;
	};


static TInt volume = DEFAULT_VOLUME;
static RPointerArray<CMidiClientUtility> players(4);
static CArrayFixFlat<TInt> playersVolume(4);
static RMutex* lock = NULL;

static void SetPlayerVolumeL(CMidiClientUtility* player, TInt origVolume) {
	player->SetVolumeL(volume == 0 ? 0 : (origVolume * volume / MAX_VOLUME));
}

static void UpdateVolume() {
	if (lock != NULL) {
		lock->Wait();
		TInt len = players.Count();
		for (TInt i = 0; i < len; ++i) {
			CMidiClientUtility* player = players[i];
			TRAP_IGNORE(SetPlayerVolumeL(player, playersVolume[i]));
		}
		lock->Signal();
	}
}

class CRemConObserver : public CBase, public MRemConCoreApiTargetObserver
{
public:
	static CRemConObserver* NewL() {
		CRemConObserver* self = new (ELeave) CRemConObserver();
		CleanupStack::PushL(self);
		self->ConstructL();
		CleanupStack::Pop(self);
		return self;
	}
	
	~CRemConObserver() {
//		if (iRemConTarget) {
//			delete iRemConTarget;
//		}
		if (iRemConSelector) {
			delete iRemConSelector;
		}
	}

private:
	CRemConObserver() {
		
	}
	void ConstructL() {
		iRemConSelector = CRemConInterfaceSelector::NewL();
		iRemConTarget = CRemConCoreApiTarget::NewL(*iRemConSelector, *this);
		iRemConSelector->OpenTargetL();
	}

	void MrccatoCommand(TRemConCoreApiOperationId aOperationId, TRemConCoreApiButtonAction aButtonAct) {
		switch (aOperationId) {
		case ERemConCoreApiVolumeUp: {
			volume += VOLUME_STEP;
			if (volume > MAX_VOLUME) volume = MAX_VOLUME;
			
			UpdateVolume();
			break;
		}
		case ERemConCoreApiVolumeDown: {
			volume -= VOLUME_STEP;
			if (volume < 0) volume = 0;
			
			UpdateVolume();
			break;
		}
		default:
			break;
		}
	}

private:
	CRemConInterfaceSelector* iRemConSelector;
	CRemConCoreApiTarget* iRemConTarget;
};

static CRemConObserver* observer = NULL;

EXPORT_C void CMidiClientUtiliti::SetVolumeL(TInt aVolume)
{
	if (lock != NULL) {
		lock->Wait();
		TInt idx = players.Find(reinterpret_cast<CMidiClientUtility*>(this));
		if (idx != -1) {
			playersVolume[idx] = aVolume;
		}
		lock->Signal();
	}
	SetPlayerVolumeL(reinterpret_cast<CMidiClientUtility*>(this), aVolume);
}

EXPORT_C void CMidiClientUtiliti::SetBalanceL(TInt aBalance)
{
	reinterpret_cast<CMidiClientUtility*>(this)->SetBalanceL(aBalance);
}

EXPORT_C TInt CMidiClientUtiliti::SendMessageL(const TDesC8& aMidiMessage)
{
	return reinterpret_cast<CMidiClientUtility*>(this)->SendMessageL(aMidiMessage);
}

EXPORT_C void CMidiClientUtiliti::SetPriorityL(TInt aPriority, TMdaPriorityPreference aPref)
{
	reinterpret_cast<CMidiClientUtility*>(this)->SetPriorityL(aPriority, aPref);
}

EXPORT_C void CMidiClientUtiliti::SetInstrumentL(TInt aChannel, TInt aBankId, TInt aInstrumentId)
{
	reinterpret_cast<CMidiClientUtility*>(this)->SetInstrumentL(aChannel, aBankId, aInstrumentId);
}

EXPORT_C void CMidiClientUtiliti::SetPlaybackRateL(TInt aRate)
{
	reinterpret_cast<CMidiClientUtility*>(this)->SetPlaybackRateL(aRate);
}

EXPORT_C TInt CMidiClientUtiliti::SetPitchTranspositionL(TInt aCents)
{
	return reinterpret_cast<CMidiClientUtility*>(this)->SetPitchTranspositionL(aCents);
}

EXPORT_C void CMidiClientUtiliti::SetPositionMicroSecondsL(const TTimeIntervalMicroSeconds& aPosition)
{
	reinterpret_cast<CMidiClientUtility*>(this)->SetPositionMicroSecondsL(aPosition);
}

EXPORT_C void CMidiClientUtiliti::Play()
{
//	if (lock != NULL) {
//		lock->Wait();
//		TInt idx = players.Find(reinterpret_cast<CMidiClientUtility*>(this));
//		if (idx != -1) {
//			TRAP_IGNORE(SetPlayerVolumeL(reinterpret_cast<CMidiClientUtility*>(this), playersVolume[idx]));
//		}
//		lock->Signal();
//	}
	reinterpret_cast<CMidiClientUtility*>(this)->Play();
}

EXPORT_C void CMidiClientUtiliti::Stop(const TTimeIntervalMicroSeconds& aFadeOutDuration)
{
	reinterpret_cast<CMidiClientUtility*>(this)->Stop(aFadeOutDuration);
}

EXPORT_C void CMidiClientUtiliti::Close()
{
	if (lock != NULL) {
		lock->Wait();
		TInt idx = players.Find(reinterpret_cast<CMidiClientUtility*>(this));
		if (idx != -1) {
			players.Remove(idx);
			playersVolume.Delete(idx);
		}
		if (observer != NULL && players.Count() == 0) {
			delete observer;
			observer = NULL;
		}
		lock->Signal();
	}
	reinterpret_cast<CMidiClientUtility*>(this)->Close();
}

EXPORT_C void CMidiClientUtiliti::OpenDes(const TDesC8& aDescriptor)
{
	reinterpret_cast<CMidiClientUtility*>(this)->OpenDes(aDescriptor);
}

EXPORT_C void CMidiClientUtiliti::OpenFile(const TDesC& aFileName)
{
	reinterpret_cast<CMidiClientUtility*>(this)->OpenFile(aFileName);
}

EXPORT_C void CMidiClientUtiliti::SetTempoL(TInt aMicroBeatsPerMinute)
{
	reinterpret_cast<CMidiClientUtility*>(this)->SetTempoL(aMicroBeatsPerMinute);
}

EXPORT_C TInt CMidiClientUtiliti::MaxVolumeL() const
{
	return reinterpret_cast<const CMidiClientUtility*>(this)->MaxVolumeL();
}

EXPORT_C TInt CMidiClientUtiliti::GetBalanceL() const
{
	return reinterpret_cast<const CMidiClientUtility*>(this)->GetBalanceL();
}

EXPORT_C TInt CMidiClientUtiliti::PlaybackRateL() const
{
	return reinterpret_cast<const CMidiClientUtility*>(this)->PlaybackRateL();
}

EXPORT_C TReal32 CMidiClientUtiliti::ChannelVolumeL(TInt aChannel) const
{
	return reinterpret_cast<const CMidiClientUtility*>(this)->ChannelVolumeL(aChannel);
}

EXPORT_C TInt CMidiClientUtiliti::MaxPlaybackRateL() const
{
	return reinterpret_cast<const CMidiClientUtility*>(this)->MaxPlaybackRateL();
}

EXPORT_C TInt CMidiClientUtiliti::MinPlaybackRateL() const
{
	return reinterpret_cast<const CMidiClientUtility*>(this)->MinPlaybackRateL();
}

EXPORT_C CMMFMetaDataEntry* CMidiClientUtiliti::GetMetaDataEntryL(TInt aMetaDataIndex) const
{
	return reinterpret_cast<const CMidiClientUtility*>(this)->GetMetaDataEntryL(aMetaDataIndex);
}

EXPORT_C TTimeIntervalMicroSeconds CMidiClientUtiliti::DurationMicroSecondsL() const
{
	return reinterpret_cast<const CMidiClientUtility*>(this)->DurationMicroSecondsL();
}

EXPORT_C TTimeIntervalMicroSeconds CMidiClientUtiliti::PositionMicroSecondsL() const
{
	return reinterpret_cast<const CMidiClientUtility*>(this)->PositionMicroSecondsL();
}

EXPORT_C TInt CMidiClientUtiliti::NumberOfMetaDataEntriesL() const
{
	return reinterpret_cast<const CMidiClientUtility*>(this)->NumberOfMetaDataEntriesL();
}

EXPORT_C TInt CMidiClientUtiliti::PitchTranspositionCentsL() const
{
	return reinterpret_cast<const CMidiClientUtility*>(this)->PitchTranspositionCentsL();
}

EXPORT_C TInt CMidiClientUtiliti::TempoMicroBeatsPerMinuteL() const
{
	return reinterpret_cast<const CMidiClientUtility*>(this)->TempoMicroBeatsPerMinuteL();
}

EXPORT_C TMidiState CMidiClientUtiliti::State() const
{
	return reinterpret_cast<const CMidiClientUtility*>(this)->State();
}

static void AddPlayerL(CMidiClientUtility* player) {
	if (observer == NULL) {
		observer = CRemConObserver::NewL();
	}
	players.AppendL(player);
	TRAPD(err, playersVolume.AppendL(0));
	if (err) {
		players.Remove(players.Count());
	}
}

EXPORT_C CMidiClientUtiliti* CMidiClientUtiliti::NewL(MMidiClientUtilityObserver& aObserver,
	TInt aPriority, TMdaPriorityPreference aPref, TBool aUseSharedHeap)
{
	CMidiClientUtility* r = CMidiClientUtility::NewL(aObserver, aPriority, aPref, aUseSharedHeap);
	if (lock == NULL) {
		// TODO leak
		lock = new RMutex;
		lock->CreateLocal();
	}
	lock->Wait();
	TRAP_IGNORE(AddPlayerL(r));
	lock->Signal();
	return reinterpret_cast<CMidiClientUtiliti*>(r);
}
