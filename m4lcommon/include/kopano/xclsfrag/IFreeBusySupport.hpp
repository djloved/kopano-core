virtual HRESULT __stdcall Open(IMAPISession *lpMAPISession, IMsgStore *lpMsgStore, BOOL bStore) _kc_override;
virtual HRESULT __stdcall Close(void) _kc_override;
virtual HRESULT __stdcall LoadFreeBusyData(ULONG cMax, FBUser *rgfbuser, IFreeBusyData **prgfbdata, HRESULT *phrStatus, ULONG *pcRead) _kc_override;
virtual HRESULT __stdcall LoadFreeBusyUpdate(ULONG cUsers, FBUser *lpUsers, IFreeBusyUpdate **lppFBUpdate, ULONG *lpcFBUpdate, void *lpData4) _kc_override;
virtual HRESULT __stdcall CommitChanges(void) _kc_override;
virtual HRESULT __stdcall GetDelegateInfo(FBUser fbUser, void *lpData) _kc_override;
virtual HRESULT __stdcall SetDelegateInfo(void *lpData) _kc_override;
virtual HRESULT __stdcall AdviseFreeBusy(void *lpData) _kc_override;
virtual HRESULT __stdcall Reload(void *lpData) _kc_override;
virtual HRESULT __stdcall GetFBDetailSupport(void **lppData, BOOL bData) _kc_override;
virtual HRESULT __stdcall HrHandleServerSched(void *lpData) _kc_override;
virtual HRESULT __stdcall HrHandleServerSchedAccess(void) _kc_override;
virtual BOOL __stdcall FShowServerSched(BOOL bData) _kc_override;
virtual HRESULT __stdcall HrDeleteServerSched(void) _kc_override;
virtual HRESULT __stdcall GetFReadOnly(void *lpData) _kc_override;
virtual HRESULT __stdcall SetLocalFB(void *lpData) _kc_override;
virtual HRESULT __stdcall PrepareForSync(void) _kc_override;
virtual HRESULT __stdcall GetFBPublishMonthRange(void *lpData) _kc_override;
virtual HRESULT __stdcall PublishRangeChanged(void) _kc_override;
//virtual HRESULT __stdcall CleanTombstone(void) _kc_override;
virtual HRESULT __stdcall GetDelegateInfoEx(FBUser fbUser, unsigned int *lpData1, unsigned int *lpData2, unsigned int *lpData3) _kc_override;
virtual HRESULT __stdcall PushDelegateInfoToWorkspace(void) _kc_override;
virtual HRESULT __stdcall Placeholder21(void *lpData, HWND hwnd, BOOL bData) _kc_override;
virtual HRESULT __stdcall Placeholder22(void) _kc_override;
