/*
 * Copyright 2005 - 2016 Zarafa and its licensors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License, version 3,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <kopano/platform.h>
#include <kopano/ECConfig.h>
#include "ECArchiverLogger.h"
#include "stubber.h"
#include <kopano/archiver-common.h>
#include "helpers/MAPIPropHelper.h"
#include <kopano/mapiext.h>
using namespace KC::helpers;

namespace KC {

/**
 * Check if the store entryid from a SObjectEntry is wrapped with a server path.
 *
 * This class is used as the predicate argument in find_if. 
 */
class IsNotWrapped _kc_final {
	public:
	/**
	 * This method is called for each SObjectEntry for which the store entryid needs to be
	 * checked if it is wrapped or not.
	 *
	 * @param[in]	sEntry	The SObjectEntry under inspection.
	 *
	 * @retval	true	The store entryid from the passed SObjectEntry is wrapped.
	 * @retval	false	The store entryid from the passed SObjectEntry is not wrapped.
	 */
	inline bool operator()(const SObjectEntry &sEntry) const
	{
		return !sEntry.sStoreEntryId.isWrapped();
	}
};

namespace operations {

/**
 * @param[in]	lpLogger
 *					Pointer to the logger.
 * @param[in]	ulptStubbed
 *					The proptag of the stubbed property {72e98ebc-57d2-4ab5-b0aad50a7b531cb9}/stubbed
 */
Stubber::Stubber(ECArchiverLogger *lpLogger, ULONG ulptStubbed, int ulAge, bool bProcessUnread)
: ArchiveOperationBase(lpLogger, ulAge, bProcessUnread, ARCH_NEVER_STUB)
, m_ulptStubbed(ulptStubbed)
{ }

HRESULT Stubber::ProcessEntry(IMAPIFolder * lpFolder, const SRow &proprow)
{
	HRESULT hr;
	MessagePtr ptrMessage;
	ULONG ulType = 0;

	assert(lpFolder != NULL);
	if (lpFolder == NULL)
		return MAPI_E_INVALID_PARAMETER;
	auto lpEntryId = proprow.cfind(PR_ENTRYID);
	if (lpEntryId == NULL) {
		Logger()->Log(EC_LOGLEVEL_FATAL, "PR_ENTRYID missing");
		return MAPI_E_NOT_FOUND;
	}
	Logger()->Log(EC_LOGLEVEL_DEBUG, "Opening message (%s)", bin2hex(lpEntryId->Value.bin).c_str());
	hr = lpFolder->OpenEntry(lpEntryId->Value.bin.cb, reinterpret_cast<ENTRYID *>(lpEntryId->Value.bin.lpb), &IID_IECMessageRaw, MAPI_BEST_ACCESS, &ulType, &~ptrMessage);
	if (hr == MAPI_E_NOT_FOUND) {
		Logger()->Log(EC_LOGLEVEL_WARNING, "Failed to open message. This can happen if the search folder is lagging.");
		return hrSuccess;
	} else if (hr != hrSuccess) {
		return Logger()->perr("Failed to open message", hr);
	}
	return ProcessEntry(ptrMessage);
}

HRESULT Stubber::ProcessEntry(LPMESSAGE lpMessage)
{
	HRESULT hr;
	SPropValue sProps[3];
	SPropValue sProp = {0};
	MAPITablePtr ptrAttTable;
	SRowSetPtr ptrRowSet;
	AttachPtr ptrAttach;
	ULONG ulAttachNum = 0;
	MAPIPropHelperPtr ptrMsgHelper;
	ObjectEntryList lstMsgArchives;
	static constexpr const SizedSPropTagArray(1, sptaTableProps) = {1, {PR_ATTACH_NUM}};

	assert(lpMessage != NULL);
	if (lpMessage == NULL)
		return MAPI_E_INVALID_PARAMETER;

	hr = VerifyRestriction(lpMessage);
	if (hr == MAPI_E_NOT_FOUND) {
		// This is not an error
		Logger()->Log(EC_LOGLEVEL_WARNING, "Ignoring message because it doesn't match the criteria for begin stubbed.");
		Logger()->Log(EC_LOGLEVEL_WARNING, "This can happen when huge amounts of message are being processed.");
		return hrSuccess;
	} else if (hr != hrSuccess) {
		return Logger()->pwarn("Failed to verify message criteria", hr);
	}

	// Verify if we have at least one archive that's in the current multi-server cluster.
	hr = MAPIPropHelper::Create(MAPIPropPtr(lpMessage, true), &ptrMsgHelper);
	if (hr != hrSuccess)
		return Logger()->perr("Failed to create prop helper", hr);
	hr = ptrMsgHelper->GetArchiveList(&lstMsgArchives);
	if (hr != hrSuccess) {
		if (hr == MAPI_E_CORRUPT_DATA) {
			Logger()->Log(EC_LOGLEVEL_ERROR, "Existing list of archives is corrupt, skipping message.");
			hr = hrSuccess;
		} else
			Logger()->perr("Failed to get list of archives", hr);
		return hr;
	}

	if (find_if(lstMsgArchives.begin(), lstMsgArchives.end(), IsNotWrapped()) == lstMsgArchives.end()) {
		Logger()->Log(EC_LOGLEVEL_WARNING, "Message has no archives that are directly accessible, message will not be stubbed.");
		return hr;
	}

	sProps[0].ulPropTag = m_ulptStubbed;
	sProps[0].Value.b = 1;
	
	sProps[1].ulPropTag = PR_BODY;
	sProps[1].Value.LPSZ = const_cast<TCHAR *>(KC_T("This message is archived..."));
	sProps[2].ulPropTag = PR_ICON_INDEX;
	sProps[2].Value.l = 2;

	hr = lpMessage->SetProps(3, sProps, NULL);
	if (hr != hrSuccess)
		return Logger()->perr("Failed to set properties", hr);
	hr = lpMessage->GetAttachmentTable(fMapiDeferredErrors, &~ptrAttTable);
	if (hr != hrSuccess)
		return Logger()->perr("Failed to get attachment table", hr);
	hr = HrQueryAllRows(ptrAttTable, sptaTableProps, nullptr, nullptr, 0, &~ptrRowSet);
	if (hr != hrSuccess)
		return Logger()->perr("Failed to get attachment numbers", hr);

	if (!ptrRowSet.empty()) {
		Logger()->Log(EC_LOGLEVEL_INFO, "Removing %u attachments", ptrRowSet.size());
		for (ULONG i = 0; i < ptrRowSet.size(); ++i) {
			hr = lpMessage->DeleteAttach(ptrRowSet[i].lpProps[0].Value.ul, 0, NULL, 0);
			if (hr != hrSuccess) {
				Logger()->Log(EC_LOGLEVEL_FATAL, "Failed to delete attachment %u. (hr=%s)", i, stringify(hr, true).c_str());
				return hr;
			}
		}
		
		Logger()->Log(EC_LOGLEVEL_INFO, "Adding placeholder attachment");		
		hr = lpMessage->CreateAttach(&iid_of(ptrAttach), 0, &ulAttachNum, &~ptrAttach);
		if (hr != hrSuccess)
			return Logger()->perr("Failed to create attachment", hr);
		sProp.ulPropTag = PR_ATTACH_FILENAME;
		sProp.Value.LPSZ = const_cast<TCHAR *>(KC_T("dummy"));
		hr = ptrAttach->SetProps(1, &sProp, NULL);
		if (hr != hrSuccess)
			return Logger()->perr("Failed to set attachment properties", hr);
		hr = ptrAttach->SaveChanges(0);
		if (hr != hrSuccess)
			return Logger()->perr("Failed to save attachment", hr);
	}
	
	hr = lpMessage->SaveChanges(0);
	if (hr != hrSuccess)
		return Logger()->perr("Failed to save stubbed message", hr);
	return hrSuccess;
}

}} /* namespace */
