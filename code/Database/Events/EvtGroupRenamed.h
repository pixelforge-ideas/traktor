/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Database/IEvent.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_DATABASE_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::db
{

/*! Group renamed event.
 * \ingroup Database
 */
class T_DLLCLASS EvtGroupRenamed : public IEvent
{
	T_RTTI_CLASS;

public:
	EvtGroupRenamed() = default;

	explicit EvtGroupRenamed(const std::wstring& name, const std::wstring& previousPath);

	/*! New group name.
	 *
	 * \return New group name.
	 */
	const std::wstring& getName() const;

	/*! Path to group.
	 *
	 * This path also contain the current name
	 * of the group.
	 *
	 * \return Path to group.
	 */
	const std::wstring& getPreviousPath() const;

	virtual void serialize(ISerializer& s) override final;

private:
	std::wstring m_name;
	std::wstring m_previousPath;
};

}
