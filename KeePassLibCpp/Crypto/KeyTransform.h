/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2011 Dominik Reichl <dominik.reichl@t-online.de>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef ___KEY_TRANSFORM_H___

#include <boost/utility.hpp>

#if !defined(_M_X64) && !defined(_WIN32_WCE)
#include <boost/thread/thread.hpp>
#endif

class CKeyTransform
{
public:
	CKeyTransform();
	CKeyTransform(const CKeyTransform& cc);
	CKeyTransform(UINT64 qwRounds, UINT8* pBuf, const UINT8* pKey, bool* pbSucceeded);
	virtual ~CKeyTransform();

	void operator()();

	static bool Transform256(UINT64 qwRounds, UINT8* pBuffer256, const UINT8* pKeySeed256);

	static UINT64 Benchmark(DWORD dwTimeMs);

private:
	UINT64 m_qwRounds;
	UINT8* m_pBuf;
	const UINT8* m_pKey;
	bool* m_pbSucceeded;
};

class CKeyTransformBenchmark
{
public:
	CKeyTransformBenchmark();
	CKeyTransformBenchmark(const CKeyTransformBenchmark& cc);
	CKeyTransformBenchmark(DWORD dwTimeMs);
	virtual ~CKeyTransformBenchmark();

	void operator()();

	UINT64 GetComputedRounds();

private:
	DWORD m_dwTimeMs;
	UINT64 m_qwComputedRounds;
};

#endif // ___KEY_TRANSFORM_H___
