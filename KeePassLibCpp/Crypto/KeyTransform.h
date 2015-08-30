/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2008 Dominik Reichl <dominik.reichl@t-online.de>

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
#include <boost/thread/thread.hpp>

class CKeyTransform
{
public:
	CKeyTransform();
	CKeyTransform(const CKeyTransform& cc);
	CKeyTransform(DWORD dwRounds, UINT8 *pBuf, const UINT8 *pKey, bool *pbSucceeded);
	virtual ~CKeyTransform();

	void operator()();

	static bool Transform256(DWORD dwRounds, UINT8 *pBuffer256, const UINT8 *pKeySeed256);

	static DWORD Benchmark(DWORD dwTimeMs);

private:
	DWORD m_dwRounds;
	UINT8 *m_pBuf;
	const UINT8 *m_pKey;
	bool *m_pbSucceeded;
};

class CKeyTransformBenchmark
{
public:
	CKeyTransformBenchmark();
	CKeyTransformBenchmark(const CKeyTransformBenchmark& cc);
	CKeyTransformBenchmark(DWORD dwTimeMs);
	virtual ~CKeyTransformBenchmark();

	void operator()();

	DWORD GetComputedRounds();

private:
	DWORD m_dwTimeMs;
	DWORD m_dwComputedRounds;
};

#endif // ___KEY_TRANSFORM_H___
