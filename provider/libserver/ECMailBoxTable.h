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

#pragma once

#include <kopano/zcdefs.h>
#include <kopano/Util.h>
#include "ECStoreObjectTable.h"

namespace KC {

class ECSession;

class ECMailBoxTable _kc_final : public ECStoreObjectTable {
protected:
	ECMailBoxTable(ECSession *lpSession, unsigned int ulFlags, const ECLocale &locale);

public:
	static ECRESULT Create(ECSession *lpSession, unsigned int ulFlags, const ECLocale &locale, ECMailBoxTable **lppTable);
	virtual ECRESULT Load();

private:
	unsigned int m_ulStoreTypes;
	ALLOC_WRAP_FRIEND;
};

} /* namespace */
