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

#ifndef ECIMPORTHIERARCHYCHANGESPROXY_H
#define ECIMPORTHIERARCHYCHANGESPROXY_H

#include <kopano/zcdefs.h>
#include <edkmdb.h>

class ECImportHierarchyChangesProxy _kc_final :
    public IExchangeImportHierarchyChanges {
private:
    ULONG m_cRef;
	zval m_lpObj;
public:
	ECImportHierarchyChangesProxy(const zval *v TSRMLS_DC);
    ~ECImportHierarchyChangesProxy();
	virtual ULONG AddRef(void) _kc_override;
	virtual ULONG Release(void) _kc_override;
	virtual HRESULT QueryInterface(REFIID iid, void **lpvoid) _kc_override;
    
	virtual HRESULT GetLastError(HRESULT hResult, ULONG ulFlags, LPMAPIERROR *lppMAPIError) _kc_override;
	virtual HRESULT Config(LPSTREAM lpStream, ULONG ulFlags) _kc_override;
	virtual HRESULT UpdateState(LPSTREAM lpStream) _kc_override;
	virtual HRESULT ImportFolderChange(ULONG cValue, LPSPropValue lpPropArray) _kc_override;
	virtual HRESULT ImportFolderDeletion(ULONG ulFlags, LPENTRYLIST lpSourceEntryList) _kc_override;
};

#endif
