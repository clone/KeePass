/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2014 Dominik Reichl <dominik.reichl@t-online.de>

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
#define ___KEY_TRANSFORM_H___

#pragma once

#include <boost/utility.hpp>

class CKeyTransform : boost::noncopyable
{
public:
	CKeyTransform(UINT64 qwRounds, UINT8* pBuf, const UINT8* pKey);

	void Run();
	bool Succeeded() const { return m_bSucceeded; }

	static bool Transform256(UINT64 qwRounds, UINT8* pBuffer256, const UINT8* pKeySeed256);
	static UINT64 Benchmark(DWORD dwTimeMs);

private:
	UINT64 m_qwRounds;
	UINT8* m_pBuf;
	const UINT8* m_pKey;
	bool m_bSucceeded;
};

class CKeyTransformBenchmark : boost::noncopyable
{
public:
	CKeyTransformBenchmark(DWORD dwTimeMs);

	void Run();

	UINT64 GetComputedRounds() const { return m_qwComputedRounds; }

private:
	DWORD m_dwTimeMs;
	UINT64 m_qwComputedRounds;
};

// Private:
DWORD WINAPI CKeyTrf_ThreadProc(LPVOID lpParameter);
DWORD WINAPI CKeyTrfBench_ThreadProc(LPVOID lpParameter);

#endif // ___KEY_TRANSFORM_H___
