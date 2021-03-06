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

#ifndef ARCHIVER_COMMON_H_INCLUDED
#define ARCHIVER_COMMON_H_INCLUDED

#include <list>
#include <vector>
#include <memory>
#include <mapidefs.h>
#include <kopano/zcdefs.h>
#include <kopano/ArchiveControl.h>
#include <kopano/stringutil.h>

namespace KC {

#if 1
#define fMapiDeferredErrors	MAPI_DEFERRED_ERRORS
#else
#define fMapiDeferredErrors	0
#endif


#define dispidAttachedUser			"attached-user-entryid"
#define dispidStoreEntryIds			"store-entryids"
#define dispidItemEntryIds			"item-entryids"
#define dispidOrigSourceKey			"original-sourcekey"
#define dispidSearchFolderEntryIds	"search-folder-entryids"
#define dispidStubbed				"stubbed"
#define dispidDirty					"dirty"
#define dispidType					"archive-type"
#define dispidAttachType			"attach-type"
#define dispidRefStoreEntryId		"ref-store-entryid"
#define dispidRefItemEntryId		"ref-item-entryid"
#define dispidRefPrevEntryId		"ref-prev-entryid"
#define dispidFlags					"flags"
#define dispidVersion				"version"
#define dispidSpecialFolderEntryIds	"special-folder-entryids"

#define ARCH_NEVER_ARCHIVE			0x00000001
#define ARCH_NEVER_STUB				0x00000002
#define ARCH_NEVER_DELETE			0x00000004

#define _SECOND ((__int64) 10000000)
#define _MINUTE (60 * _SECOND)
#define _HOUR   (60 * _MINUTE)
#define _DAY    (24 * _HOUR)

/**
 * Utility class for easy handling of non-addressbook entryids.
 */
class _kc_export entryid_t _kc_final {
public:
	/**
	 * Constructs an empty entryid.
	 */
	_kc_hidden entryid_t(void) = default;
	
	/**
	 * Construct an entryid based on a length and pointer argument.
	 *
	 * @param[in]	cbEntryId
	 *					The length in bytes of the entryid.
	 * @param[in]	lpEntryId
	 *					Pointer to the entryid.
	 */
	_kc_hidden entryid_t(size_t z, const ENTRYID *eid) :
		m_eid(reinterpret_cast<const char *>(eid), z)
	{ }
	
	/**
	 * Construct an entryid based on a SBinary structure.
	 *
	 * @param[in]	sBin
	 *					The SBinary structure from which the data will be extracted.
	 */
	_kc_hidden entryid_t(const SBinary &sBin) :
		m_eid(reinterpret_cast<const char *>(sBin.lpb), sBin.cb)
	{ }
	
	_kc_hidden entryid_t(const entryid_t &) = default;
	_kc_hidden entryid_t(entryid_t &&) = default;
	
	/**
	 * Assign a new entryid based on a length and pointer argument.
	 *
	 * @param[in]	cbEntryId
	 *					The length in bytes of the entryid.
	 * @param[in]	lpEntryId
	 *					Pointer to the entryid.
	 */
	_kc_hidden void assign(size_t z, const ENTRYID *eid)
	{
		m_eid.assign(reinterpret_cast<const char *>(eid), z);
	}
	
	/**
	 * Assign a new entryid based on a SBinary structure.
	 *
	 * @param[in]	sBin
	 *					The SBinary structure from which the data will be extracted.
	 */
	_kc_hidden entryid_t &operator=(const SBinary &sBin)
	{
		m_eid.assign(reinterpret_cast<const char *>(sBin.lpb), sBin.cb);
		return *this;
	}
	
	/**
	 * Returns the size in bytes of the entryid.
	 * @return The size in bytes of the entryid.
	 */
	_kc_hidden ULONG size(void) const { return m_eid.size(); }
	
	/**
	 * Returns true if the entryid is empty.
	 * @return true or false
	 */
	_kc_hidden bool empty(void) const { return m_eid.empty(); }
	
	/**
	 * Return a pointer to the data as a BYTE pointer.
	 * @return The entryid data.
	 */
	_kc_hidden operator LPBYTE(void) const
	{
		return reinterpret_cast<BYTE *>(const_cast<char *>(m_eid.data()));
	}
	
	/**
	 * Return a pointer to the data as an ENTRYID pointer.
	 * @return The entryid data.
	 */
	_kc_hidden operator LPENTRYID(void) const
	{
		return reinterpret_cast<ENTRYID *>(const_cast<char *>(m_eid.data()));
	}
	
	/**
	 * Return a pointer to the data as a VOID pointer.
	 * @return The entryid data.
	 */
	_kc_hidden operator LPVOID(void) const
	{
		return const_cast<char *>(m_eid.data());
	}

	entryid_t &operator=(const entryid_t &) = default;
	entryid_t &operator=(entryid_t &&) = default;

	/**
	 * Compare the content of the current entryid with the content of another entryid.
	 * @param[in]	other
	 *					The other entryid to compare content with.
	 * @return true if the entryids are equal.
	 */
	bool operator==(const entryid_t &other) const;
	
	/**
	 * Compare the content of the current entryid with the content of another entryid.
	 * @param[in]	other
	 *					The other entryid to compare content with.
	 * @return true if the entryids are not equal.
	 */
	_kc_hidden bool operator!=(const entryid_t &other) const
	{
		return !(*this == other);
	}
	
	/**
	 * Compare the content of the current entryid with the content of another entryid.
	 * @param[in]	other
	 *					The other entryid to compare content with.
	 * @return true if a binary compare of the entryids results in the current entryid being smaller.
	 */
	bool operator<(const entryid_t &other) const;
	
	/**
	 * Compare the content of the current entryid with the content of another entryid.
	 * @param[in]	other
	 *					The other entryid to compare content with.
	 * @return true if a binary compare of the entryids results in the current entryid being smaller.
	 */
	bool operator>(const entryid_t &other) const;

	/**
	 * Convert the entryid to a human readable hexadecimal format.
	 * @return The entryid in hexadecimal format.
	 */
	_kc_hidden std::string tostring(void) const
	{
		return bin2hex(m_eid.size(), m_eid.data());
	}
	
	/**
	 * Get entryid as a std::string
	 * @return The binary data as std::string
	 */
	_kc_hidden std::string data(void) const
	{
		return m_eid;
	}
	
	/**
	 * Wrap the entryid with a server path.
	 * 
	 * The path should start with "file://", "http://" or "https://" for this
	 * call to succeed.
	 * 
	 * @param[in]	strPath		The path to wrap the entryid with.
	 * 
	 * @return true on success
	 */
	_kc_hidden bool wrap(const std::string &path);
	
	/**
	 * Unwrap the path from the entryid. 
	 * 
	 * Extracts the path from the entryid and remove it from the data.
	 * 
	 * @param[out]	lpstrPath	The path that wrapped the entryid.
	 * 
	 * @retval	true	The path was successfully extracted.
	 * @retval	false	THe entryid wasn't wrapped.
	 */
	_kc_hidden bool unwrap(std::string *path);

	/**
	 * Check if an entryid is wrapped with a server path.
	 *
	 * @retval	true	The entryid is wrapped.
	 * @retval	false	The entryis is not wrapped.
	 */
	bool isWrapped() const;

	/**
	 * Get the unwrapped entryid.
	 *
	 * @returns		An entryid object.
	 */
	entryid_t getUnwrapped() const;
	
private:
	std::string m_eid;
};

/**
 * Utility class for easy handling of addressbook entryids.
 */
class abentryid_t _kc_final {
public:
	/**
	 * Constructs an empty entryid.
	 */
	abentryid_t(void) = default;
	
	/**
	 * Construct an entryid based on a length and pointer argument.
	 *
	 * @param[in]	cbEntryId
	 *					The length in bytes of the entryid.
	 * @param[in]	lpEntryId
	 *					Pointer to the entryid.
	 */
	abentryid_t(size_t z, const ENTRYID *eid) :
		m_eid(reinterpret_cast<const char *>(eid), z)
	{ }
	
	/**
	 * Construct an entryid based on a SBinary structure.
	 *
	 * @param[in]	sBin
	 *					The SBinary structure from which the data will be extracted.
	 */
	abentryid_t(const SBinary &sBin) :
		m_eid(reinterpret_cast<const char *>(sBin.lpb), sBin.cb)
	{ }
	
	abentryid_t(const abentryid_t &) = default;
	abentryid_t(abentryid_t &&) = default;
	
	/**
	 * Assign a new entryid based on a length and pointer argument.
	 *
	 * @param[in]	cbEntryId
	 *					The length in bytes of the entryid.
	 * @param[in]	lpEntryId
	 *					Pointer to the entryid.
	 */
	void assign(size_t z, const ENTRYID *eid)
	{
		m_eid.assign(reinterpret_cast<const char *>(eid), z);
	}
	
	/**
	 * Assign a new entryid based on a SBinary structure.
	 *
	 * @param[in]	sBin
	 *					The SBinary structure from which the data will be extracted.
	 */
	abentryid_t &operator=(const SBinary &sBin)
	{
		m_eid.assign(reinterpret_cast<const char *>(sBin.lpb), sBin.cb);
		return *this;
	}
	
	/**
	 * Returns the size in bytes of the entryid.
	 * @return The size in bytes of the entryid.
	 */
	ULONG size() const { return m_eid.size(); }
	
	/**
	 * Returns true if the entryid is empty.
	 * @return true or false
	 */
	bool empty() const { return m_eid.empty(); }
	
	/**
	 * Return a pointer to the data as a BYTE pointer.
	 * @return The entryid data.
	 */
	operator LPBYTE(void) const
	{
		return reinterpret_cast<BYTE *>(const_cast<char *>(m_eid.data()));
	}
	
	/**
	 * Return a pointer to the data as an ENTRYID pointer.
	 * @return The entryid data.
	 */
	operator LPENTRYID(void) const
	{
		return reinterpret_cast<ENTRYID *>(const_cast<char *>(m_eid.data()));
	}
	
	/**
	 * Return a pointer to the data as a VOID pointer.
	 * @return The entryid data.
	 */
	operator LPVOID(void) const
	{
		return const_cast<char *>(m_eid.data());
	}

	abentryid_t &operator=(const abentryid_t &) = default;
	abentryid_t &operator=(abentryid_t &&) = default;

	/**
	 * Compare the content of the current entryid with the content of another entryid.
	 * @param[in]	other
	 *					The other entryid to compare content with.
	 * @return true if the entryids are equal.
	 */
	bool operator==(const abentryid_t &other) const {
		return compare(other) == 0;
	}
	
	/**
	 * Compare the content of the current entryid with the content of another entryid.
	 * @param[in]	other
	 *					The other entryid to compare content with.
	 * @return true if the entryids are not equal.
	 */
	bool operator!=(const abentryid_t &other) const {
		return !(*this == other);
	}
	
	/**
	 * Compare the content of the current entryid with the content of another entryid.
	 * @param[in]	other
	 *					The other entryid to compare content with.
	 * @return true if a binary compare of the entryids results in the current entryid being smaller.
	 */
	bool operator<(const abentryid_t &other) const {
		return compare(other) < 0;
	}
	
	/**
	 * Compare the content of the current entryid with the content of another entryid.
	 * @param[in]	other
	 *					The other entryid to compare content with.
	 * @return true if a binary compare of the entryids results in the current entryid being smaller.
	 */
	bool operator>(const abentryid_t &other) const {
		return compare(other) > 0;
	}

	/**
	 * Convert the entryid to a human readable hexadecimal format.
	 * @return The entryid in hexadecimal format.
	 */
	std::string tostring() const {
		return bin2hex(m_eid.size(), m_eid.data());
	}

private:
	/**
	 * Compare the entryid with another entryid.
	 * A shorter entryid will always compare less to a longer entryid.
	 * Entryids shorter or equal to 32 bytes will simply be memcmp'd.
	 * Longer entryids will be compared in two steps, leaving out the
	 * legacy user id (which differs, depending on the server the entryid
	 * was obtained from).
	 * No attempt is made to match v0 and v1 entryids.
	 * 
	 * @param[in]	other	The other entryid.
	 * 
	 * @returns < 0	This entryid compares less to the other entryid.
	 * @returns 0	This entryid equals the other entryid.
	 * @returns >0	This entryid compared greater to the other entryid.
	 * 
	 * @note: SortCompareABEID from provider/common/pcutil.cpp would be nicer. However, that's
	 * not intended to be used in client applications. Client applications shouldn't be aware
	 * of entryid formats at all...
	 */ 
	int compare(const abentryid_t &other) const;

	std::string m_eid;
};

/**
 * An SObjectEntry is a reference to an object in a particular store. The sItemEntryId can point to any 
 * MAPI object, but is currently used for folders and messages.
 */
struct SObjectEntry {
	entryid_t sStoreEntryId;
	entryid_t sItemEntryId;
};

/**
 * List of SObjectEntry objects.
 */
typedef std::list<SObjectEntry> ObjectEntryList;

static inline bool operator==(const SObjectEntry &lhs, const SObjectEntry &rhs) {
	return lhs.sStoreEntryId == rhs.sStoreEntryId && lhs.sItemEntryId == rhs.sItemEntryId;
}

static inline bool operator!=(const SObjectEntry &lhs, const SObjectEntry &rhs) {
	return !(lhs == rhs);
}

static inline bool operator<(const SObjectEntry &lhs, const SObjectEntry &rhs) {
	return	lhs.sStoreEntryId < rhs.sStoreEntryId || 
			(lhs.sStoreEntryId == rhs.sStoreEntryId && lhs.sItemEntryId < rhs.sItemEntryId);
}


/**
 * Compares two entryids that are assumed to be store entryids.
 *
 * This class is used as the predicate argument in find_if. If one of the
 * entryids is wrapped, it will be unwrapped before the comparison.
 */
class StoreCompare _kc_final {
public:
	/**
	 * This constructor takes the store entryid from an SObjectEntry.
	 *
	 * @param[in]	sEntry	The SObjectEntry from which the store entryid will be used
	 * 						to compare all other entryids with.
	 */
	StoreCompare(const SObjectEntry &sEntry): m_sEntryId(sEntry.sStoreEntryId.getUnwrapped()) {}

	/**
	 * This constructor takes an explicit entryid.
	 *
	 * @param[in]	sEntryId	The entryid the will be used
	 * 				to compare all other entryids with.
	 */
	StoreCompare(const entryid_t &sEntryId): m_sEntryId(sEntryId.getUnwrapped()) {}

	/**
	 * This method is called for each entryid that is to be compared with the stored
	 * entryid.
	 *
	 * @param[in]	sEntryId	The entryid that is to be compared with the
	 * 							stored entryid.
	 *
	 * @retval	true	The passed entryid equals the stored entryid.
	 * @retval	false	The passed entryid differs from the stored entryid.
	 */
	bool operator()(const SObjectEntry &sEntry) const { return m_sEntryId == sEntry.sStoreEntryId; }

private:
	entryid_t m_sEntryId;
};

extern _kc_export eResult MAPIErrorToArchiveError(HRESULT);
extern _kc_export const char *ArchiveResultString(eResult);

} /* namespace */

#endif // !defined ARCHIVER_COMMON_H_INCLUDED
