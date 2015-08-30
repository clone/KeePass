/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2013 Dominik Reichl <dominik.reichl@t-online.de>

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

#include "StdAfx.h"
#include "PwQualityEst.h"
#include <cmath>
#include <map>
#include <set>
#include "../PasswordGenerator/PwCharSet.h"
#include "PopularPasswords.h"

#define QE_PAT_LOWERALPHA L'L'
#define QE_PAT_UPPERALPHA L'U'
#define QE_PAT_DIGIT      L'D'
#define QE_PAT_SPECIAL    L'S'
#define QE_PAT_HIGH       L'H'
#define QE_PAT_OTHER      L'X'
#define QE_PAT_DICTIONARY L'W'
#define QE_PAT_REPETITION L'R'
#define QE_PAT_NUMBER     L'N'
#define QE_PAT_DIFFSEQ    L'C'
#define QE_PAT_ALL        L"LUDSHXWRNC"

#define QE_LOG2(__x) (log(__x)/log(2.0))

class CQeCharType : boost::noncopyable
{
public:
	CQeCharType(WCHAR chTypeID, LPCWSTR lpwAlph, bool bIsConsecutive) :
		m_chTypeID(chTypeID)
	{
		if(lpwAlph == NULL) { ASSERT(FALSE); return; }
		m_strAlph = lpwAlph;
		if(m_strAlph.size() == 0) { ASSERT(FALSE); return; }

		m_uChars = m_strAlph.size();
		m_chFirst = (bIsConsecutive ? m_strAlph[0] : L'\0');
		m_chLast = (bIsConsecutive ? m_strAlph[m_uChars - 1] : L'\0');

		m_dblCharSize = QE_LOG2(static_cast<double>(m_uChars));

		ASSERT((static_cast<size_t>(m_chLast - m_chFirst) == (m_uChars - 1)) ||
			!bIsConsecutive);
	}

	CQeCharType(WCHAR chTypeID, size_t uChars) : // Catch-none set
		m_chTypeID(chTypeID), m_uChars(uChars)
	{
		if(uChars == 0) { ASSERT(FALSE); return; }

		m_chFirst = L'\0';
		m_chLast = L'\0';

		m_dblCharSize = QE_LOG2(static_cast<double>(uChars));
	}

	WCHAR GetTypeID() const { return m_chTypeID; }
	const std::basic_string<WCHAR>& GetAlphabet() const { return m_strAlph; }
	size_t GetCharCount() const { return m_uChars; }
	double GetCharSize() const { return m_dblCharSize; }

	bool Contains(WCHAR ch) const
	{
		if(m_chLast != L'\0')
			return ((ch >= m_chFirst) && (ch <= m_chLast));

		ASSERT(m_strAlph.size() > 0);
		return (m_strAlph.find(ch) != m_strAlph.npos);
	}

private:
	WCHAR m_chTypeID;
	std::basic_string<WCHAR> m_strAlph;
	size_t m_uChars;

	WCHAR m_chFirst;
	WCHAR m_chLast;

	double m_dblCharSize;
};

static std::vector<boost::shared_ptr<CQeCharType> > m_vCharTypes;

class CEntropyEncoder : boost::noncopyable
{
public:
	CEntropyEncoder(LPCWSTR lpwAlph, UINT64 uBaseWeight, UINT64 uCharWeight,
		UINT64 uOccExclThreshold) :
		m_uBaseWeight(uBaseWeight), m_uCharWeight(uCharWeight),
		m_uOccExclThreshold(uOccExclThreshold)
	{
		if((lpwAlph == NULL) || (lpwAlph[0] == L'\0')) { ASSERT(FALSE); return; }

		m_strAlph = lpwAlph;

#ifdef DEBUG
		std::set<WCHAR> s;
		for(size_t i = 0; i < m_strAlph.size(); ++i)
		{
			ASSERT(s.find(m_strAlph[i]) == s.end()); // No duplicates
			s.insert(m_strAlph[i]);
		}
		ASSERT(s.size() == m_strAlph.size());
#endif
	}

	void Reset() { m_dHisto.clear(); }

	void Write(WCHAR ch)
	{
		ASSERT(m_strAlph.find(ch) != m_strAlph.npos);

		const UINT64 uOcc = m_dHisto[ch];
		m_dHisto[ch] = uOcc + 1;
	}

	double GetOutputSize() const
	{
		UINT64 uTotalWeight = m_uBaseWeight * m_strAlph.size();
		for(std::map<WCHAR, UINT64>::const_iterator it = m_dHisto.begin();
			it != m_dHisto.end(); ++it)
		{
			ASSERT(it->second >= 1);
			if(it->second > m_uOccExclThreshold)
				uTotalWeight += (it->second - m_uOccExclThreshold) * m_uCharWeight;
		}

		const double dTotalWeight = static_cast<double>(uTotalWeight);
		double dSize = 0.0;
		for(std::map<WCHAR, UINT64>::const_iterator it = m_dHisto.begin();
			it != m_dHisto.end(); ++it)
		{
			UINT64 uWeight = m_uBaseWeight;
			if(it->second > m_uOccExclThreshold)
				uWeight += (it->second - m_uOccExclThreshold) * m_uCharWeight;

			dSize -= static_cast<double>(it->second) * QE_LOG2(
				static_cast<double>(uWeight) / dTotalWeight);
		}

		return dSize;
	}

private:
	std::basic_string<WCHAR> m_strAlph;
	std::map<WCHAR, UINT64> m_dHisto;
	UINT64 m_uBaseWeight;
	UINT64 m_uCharWeight;
	UINT64 m_uOccExclThreshold;
};

typedef std::map<WCHAR, boost::shared_ptr<CEntropyEncoder> > TmeeMap;

class CMultiEntropyEncoder : boost::noncopyable
{
public:
	CMultiEntropyEncoder() {}

	void AddEncoder(WCHAR chTypeID, boost::shared_ptr<CEntropyEncoder> pEnc)
	{
		if(pEnc.get() == NULL) { ASSERT(FALSE); return; }

		ASSERT(m_dEncs.find(chTypeID) == m_dEncs.end());
		m_dEncs[chTypeID] = pEnc;
	}

	void Reset()
	{
		for(TmeeMap::iterator it = m_dEncs.begin(); it != m_dEncs.end(); ++it)
			it->second->Reset();
	}

	bool Write(WCHAR chTypeID, WCHAR chData)
	{
		TmeeMap::iterator it = m_dEncs.find(chTypeID);
		if(it == m_dEncs.end()) return false;

		it->second->Write(chData);
		return true;
	}

	double GetOutputSize() const
	{
		double d = 0.0;

		for(TmeeMap::const_iterator it = m_dEncs.begin();
			it != m_dEncs.end(); ++it)
			d += it->second->GetOutputSize();

		return d;
	}

private:
	TmeeMap m_dEncs;
};

class CQePatternInstance : boost::noncopyable
{
public:
	CQePatternInstance(size_t uPos, size_t uLen, WCHAR chPatternID,
		double dblCost) :
		m_uPos(uPos), m_uLen(uLen), m_chPatternID(chPatternID),
		m_dblCost(dblCost) {}

	CQePatternInstance(size_t uPos, size_t uLen,
		boost::shared_ptr<CQeCharType> pctSingle) :
		m_uPos(uPos), m_uLen(uLen)
	{
		if(pctSingle.get() == NULL) { ASSERT(FALSE); return; }

		m_chPatternID = pctSingle->GetTypeID();
		m_dblCost = pctSingle->GetCharSize();
		m_pctSingle = pctSingle;
	}

	size_t GetPosition() const { return m_uPos; }
	size_t GetLength() const { return m_uLen; }
	WCHAR GetPatternID() const { return m_chPatternID; }
	double GetCost() const { return m_dblCost; }
	boost::shared_ptr<CQeCharType> GetSingleCharType() const { return m_pctSingle; }

private:
	size_t m_uPos;
	size_t m_uLen;
	WCHAR m_chPatternID;
	double m_dblCost;
	boost::shared_ptr<CQeCharType> m_pctSingle;
};

typedef std::vector<boost::shared_ptr<CQePatternInstance> > TqePatInsts;
typedef boost::shared_ptr<TqePatInsts> TqePatInstsPtr;

class CQePathState : boost::noncopyable
{
public:
	CQePathState(size_t uPos, TqePatInstsPtr p) :
		m_uPos(uPos)
	{
		if(p.get() == NULL) { ASSERT(FALSE); return; }
		m_pPath = p;
	}

	size_t GetPosition() const { return m_uPos; }
	TqePatInstsPtr GetPath() const { return m_pPath; }

private:
	size_t m_uPos;
	TqePatInstsPtr m_pPath;
};

CPwQualityEst::CPwQualityEst()
{
}

void CPwQualityEst::_EnsureInitialized()
{
	if(m_vCharTypes.size() == 0)
	{
		std::basic_string<WCHAR> strSpecial(PDCS_PRINTASCIISPEC);
		if(strSpecial.find(L' ') != strSpecial.npos) { ASSERT(FALSE); }
		else strSpecial += L' ';

		const std::basic_string<WCHAR> strHigh =
			PwCharSet::GetHighAnsiChars().ToString();

		m_vCharTypes.push_back(boost::shared_ptr<CQeCharType>(
			new CQeCharType(QE_PAT_LOWERALPHA, PDCS_LOWER_CASE, true)));
		m_vCharTypes.push_back(boost::shared_ptr<CQeCharType>(
			new CQeCharType(QE_PAT_UPPERALPHA, PDCS_UPPER_CASE, true)));
		m_vCharTypes.push_back(boost::shared_ptr<CQeCharType>(
			new CQeCharType(QE_PAT_DIGIT, PDCS_NUMERIC, true)));
		m_vCharTypes.push_back(boost::shared_ptr<CQeCharType>(
			new CQeCharType(QE_PAT_SPECIAL, strSpecial.c_str(), false)));
		m_vCharTypes.push_back(boost::shared_ptr<CQeCharType>(
			new CQeCharType(QE_PAT_HIGH, strHigh.c_str(), false)));
		m_vCharTypes.push_back(boost::shared_ptr<CQeCharType>(
			new CQeCharType(QE_PAT_OTHER, 0x10000U - (2U * 26U) - 10U -
			strSpecial.size() - strHigh.size())));
	}
}

DWORD CPwQualityEst::EstimatePasswordBits(LPCTSTR lpPassword)
{
#ifdef _UNICODE
	return CPwQualityEst::_EstimateQuality(lpPassword);
#else
	LPWSTR lpw = _StringToUnicode(lpPassword);
	if(lpw == NULL) { ASSERT(FALSE); return 0; }

	const DWORD dwRes = CPwQualityEst::_EstimateQuality(lpw);

	mem_erase((unsigned char *)lpw, wcslen(lpw) * sizeof(WCHAR));
	SAFE_DELETE_ARRAY(lpw);
	return dwRes;
#endif
}

boost::shared_ptr<CQeCharType> QeGetCharType(WCHAR ch)
{
	const size_t nTypes = m_vCharTypes.size();
	ASSERT((nTypes > 0) && (m_vCharTypes[nTypes - 1]->GetCharCount() > 256));

	for(size_t i = 0; i < (nTypes - 1); ++i)
	{
		if(m_vCharTypes[i]->Contains(ch))
			return m_vCharTypes[i];
	}

	return m_vCharTypes[nTypes - 1];
}

double QeComputePathCost(const TqePatInsts& v, LPCWSTR lpw,
	CEntropyEncoder& ecPattern, CMultiEntropyEncoder& mcData)
{
	ecPattern.Reset();
	for(size_t i = 0; i < v.size(); ++i)
		ecPattern.Write(v[i]->GetPatternID());
	const double dblPatternCost = ecPattern.GetOutputSize();

	mcData.Reset();
	double dblDataCost = 0.0;
	for(size_t i = 0; i < v.size(); ++i)
	{
		boost::shared_ptr<CQeCharType> ptChar = v[i]->GetSingleCharType();
		if(ptChar.get() != NULL)
		{
			const WCHAR ch = lpw[v[i]->GetPosition()];
			if(!mcData.Write(ptChar->GetTypeID(), ch))
				dblDataCost += v[i]->GetCost();
		}
		else dblDataCost += v[i]->GetCost();
	}
	dblDataCost += mcData.GetOutputSize();

	return (dblPatternCost + dblDataCost);
}

WCHAR QeDecodeLeetChar(WCHAR chLeet)
{
	if((chLeet >= L'\x00C0') && (chLeet <= L'\x00C6')) return L'a';
	if((chLeet >= L'\x00C8') && (chLeet <= L'\x00CB')) return L'e';
	if((chLeet >= L'\x00CC') && (chLeet <= L'\x00CF')) return L'i';
	if((chLeet >= L'\x00D2') && (chLeet <= L'\x00D6')) return L'o';
	if((chLeet >= L'\x00D9') && (chLeet <= L'\x00DC')) return L'u';
	if((chLeet >= L'\x00E0') && (chLeet <= L'\x00E6')) return L'a';
	if((chLeet >= L'\x00E8') && (chLeet <= L'\x00EB')) return L'e';
	if((chLeet >= L'\x00EC') && (chLeet <= L'\x00EF')) return L'i';
	if((chLeet >= L'\x00F2') && (chLeet <= L'\x00F6')) return L'o';
	if((chLeet >= L'\x00F9') && (chLeet <= L'\x00FC')) return L'u';

	WCHAR ch;
	switch(chLeet)
	{
		case L'4':
		case L'@':
		case L'?':
		case L'^':
		case L'\x00AA': ch = L'a'; break;
		case L'8':
		case L'\x00DF': ch = L'b'; break;
		case L'(':
		case L'{':
		case L'[':
		case L'<':
		case L'\x00A2':
		case L'\x00A9':
		case L'\x00C7':
		case L'\x00E7': ch = L'c'; break;
		case L'\x00D0':
		case L'\x00F0': ch = L'd'; break;
		case L'3':
		case L'\x20AC':
		case L'&':
		case L'\x00A3': ch = L'e'; break;
		case L'6':
		case L'9': ch = L'g'; break;
		case L'#': ch = L'h'; break;
		case L'1':
		case L'!':
		case L'|':
		case L'\x00A1':
		case L'\x00A6': ch = L'i'; break;
		case L'\x00D1':
		case L'\x00F1': ch = L'n'; break;
		case L'0':
		case L'°':
		case L'*':
		case L'\x00A4':
		case L'\x00D8':
		case L'\x00F8': ch = L'o'; break;
		case L'\x00AE': ch = L'r'; break;
		case L'$':
		case L'5':
		case L'\x00A7': ch = L's'; break;
		case L'+':
		case L'7': ch = L't'; break;
		case L'\x00B5': ch = L'u'; break;
		case L'%':
		case L'\x00D7': ch = L'x'; break;
		case L'\x00A5':
		case L'\x00DD':
		case L'\x00FD':
		case L'\x00FF': ch = L'y'; break;
		case L'2': ch = L'z'; break;
		default: ch = chLeet; break;
	}

	return ch;
}

bool QeVectorContains(const std::vector<WCHAR>& v, WCHAR ch,
	size_t uOffset, size_t cchLen)
{
	for(size_t i = 0; i < cchLen; ++i)
	{
		if(v[uOffset + i] == ch) return true;
	}

	return false;
}

size_t QeHammingDist(LPCWSTR lp1, size_t uOffset1, LPCWSTR lp2,
	size_t uOffset2, size_t uLen)
{
	size_t uDist = 0;
	for(size_t i = 0; i < uLen; ++i)
	{
		if(lp1[uOffset1 + i] != lp2[uOffset2 + i]) ++uDist;
	}

	return uDist;
}

bool QeEvalAddPopularPasswordPattern(std::vector<TqePatInsts>& vPatterns,
	LPCWSTR lpw, size_t i, std::vector<WCHAR>& vSub, double dblCostPerMod)
{
	vSub.push_back(0); // Terminate string
	size_t uDictSize;
	const bool b = CPopularPasswords::IsPopular(&vSub[0], &uDictSize);
	vSub.pop_back();
	if(!b) return false;

	const size_t n = vSub.size();
	const size_t d = QeHammingDist(&vSub[0], 0, lpw, i, n);

	double dblCost = QE_LOG2(static_cast<double>(uDictSize));

	// dblCost += log2(n binom d)
	size_t k = min(d, n - d);
	for(size_t j = n; j > (n - k); --j)
		dblCost += QE_LOG2(static_cast<double>(j));
	for(size_t j = k; j >= 2; --j)
		dblCost -= QE_LOG2(static_cast<double>(j));

	dblCost += dblCostPerMod * static_cast<double>(d);

	vPatterns[i].push_back(boost::shared_ptr<CQePatternInstance>(
		new CQePatternInstance(i, n, QE_PAT_DICTIONARY, dblCost)));
	return true;
}

void QeFindPopularPasswords(LPCWSTR lpw, size_t n, std::vector<TqePatInsts>& vPatterns)
{
	std::vector<WCHAR> vLower(n);
	std::vector<WCHAR> vLeet(n);
	for(size_t i = 0; i < n; ++i)
	{
		const WCHAR ch = lpw[i];

		vLower[i] = towlower(ch);
		vLeet[i] = towlower(QeDecodeLeetChar(ch));
	}

	const size_t nMaxLen = min(n, CPopularPasswords::GetMaxLength());
	for(size_t nSubLen = nMaxLen; nSubLen >= 3; --nSubLen)
	{
		if(!CPopularPasswords::ContainsLength(nSubLen)) continue;

		std::vector<WCHAR> vSub(nSubLen);

		for(size_t i = 0; i <= (n - nSubLen); ++i)
		{
			if(QeVectorContains(vLower, L'\0', i, nSubLen)) continue;

			memcpy(&vSub[0], &vLower[i], nSubLen * sizeof(WCHAR));
			if(!QeEvalAddPopularPasswordPattern(vPatterns, lpw, i, vSub, 0.0))
			{
				memcpy(&vSub[0], &vLeet[i], nSubLen * sizeof(WCHAR));
				if(QeEvalAddPopularPasswordPattern(vPatterns, lpw, i, vSub, 1.5))
					memset(&vLower[i], 0, nSubLen * sizeof(WCHAR));
			}
			else memset(&vLower[i], 0, nSubLen * sizeof(WCHAR));
		}
	}
}

bool QePartsEqual(const std::vector<WCHAR>& v, size_t x1, size_t x2,
	size_t nLength)
{
	for(size_t i = 0; i < nLength; ++i)
	{
		if(v[x1 + i] != v[x2 + i]) return false;
	}

	return true;
}

void QeErasePart(std::vector<WCHAR>& v, size_t i, size_t n, WCHAR& chErased)
{
	for(size_t j = 0; j < n; ++j)
	{
		v[i + j] = chErased;
		--chErased;
	}
}

void QeFindRepetitions(LPCWSTR lpw, size_t n, std::vector<TqePatInsts>& vPatterns)
{
	std::vector<WCHAR> v(n);
	memcpy(&v[0], lpw, n * sizeof(WCHAR));

	WCHAR chErased = WCHAR_MAX;
	ASSERT(chErased == L'\xFFFF');
	for(size_t m = (n / 2); m >= 3; --m)
	{
		for(size_t x1 = 0; x1 <= (n - (2 * m)); ++x1)
		{
			bool bFoundRep = false;

			for(size_t x2 = (x1 + m); x2 <= (n - m); ++x2)
			{
				if(QePartsEqual(v, x1, x2, m))
				{
					const double dblCost = QE_LOG2(static_cast<double>(
						x1 + 1)) + QE_LOG2(static_cast<double>(m));
					vPatterns[x2].push_back(boost::shared_ptr<CQePatternInstance>(
						new CQePatternInstance(x2, m, QE_PAT_REPETITION, dblCost)));

					QeErasePart(v, x2, m, chErased);
					bFoundRep = true;
				}
			}

			if(bFoundRep) QeErasePart(v, x1, m, chErased);
		}
	}
}

void QeAddNumberPattern(std::vector<TqePatInsts>& vPatterns,
	std::vector<WCHAR>& vNumber, size_t i)
{
	if(vNumber.size() <= 2) return;

	size_t nZeros = 0;
	for(size_t j = 0; j < vNumber.size(); ++j)
	{
		if(vNumber[j] != L'0') break;
		++nZeros;
	}

	double dblCost = QE_LOG2(static_cast<double>(nZeros + 1));
	if(nZeros < vNumber.size())
	{
		vNumber.push_back(0); // Terminate string
		const double d = _wtof(&vNumber[nZeros]);
		vNumber.pop_back();

		if(d > 0.999) dblCost += QE_LOG2(d);
		else { ASSERT(FALSE); return; }
	}

	vPatterns[i].push_back(boost::shared_ptr<CQePatternInstance>(
		new CQePatternInstance(i, vNumber.size(), QE_PAT_NUMBER, dblCost)));
}

void QeFindNumbers(LPCWSTR lpw, size_t n, std::vector<TqePatInsts>& vPatterns)
{
	std::vector<WCHAR> v;

	for(size_t i = 0; i < n; ++i)
	{
		const WCHAR ch = lpw[i];
		if((ch >= L'0') && (ch <= L'9')) v.push_back(ch);
		else
		{
			QeAddNumberPattern(vPatterns, v, i - v.size());
			v.clear();
		}
	}
	QeAddNumberPattern(vPatterns, v, n - v.size());
}

void QeFindDiffSeqs(LPCWSTR lpw, size_t n, std::vector<TqePatInsts>& vPatterns)
{
	int d = INT_MIN;
	size_t p = 0;

	std::vector<WCHAR> v(n + 1);
	memcpy(&v[0], lpw, n * sizeof(WCHAR));
	v[n] = WCHAR_MAX;

	for(size_t i = 1; i < v.size(); ++i)
	{
		const int dCur = static_cast<int>(v[i]) - static_cast<int>(v[i - 1]);
		if(dCur != d)
		{
			if((i - p) >= 3) // At least 3 chars involved
			{
				boost::shared_ptr<CQeCharType> pct = QeGetCharType(v[p]);
				double dblCost = pct->GetCharSize() + QE_LOG2(
					static_cast<double>(i - p - 1));

				vPatterns[p].push_back(boost::shared_ptr<CQePatternInstance>(
					new CQePatternInstance(p, i - p, QE_PAT_DIFFSEQ, dblCost)));
			}

			d = dCur;
			p = i - 1;
		}
	}
}

DWORD CPwQualityEst::_EstimateQuality(LPCWSTR lpw)
{
	if(lpw == NULL) { ASSERT(FALSE); return 0; }
	if(lpw[0] == L'\0') return 0;

	_EnsureInitialized();

	const size_t n = wcslen(lpw);
	std::vector<TqePatInsts> vPatterns(n);
	for(size_t i = 0; i < n; ++i)
	{
		boost::shared_ptr<CQePatternInstance> piChar(new CQePatternInstance(
			i, 1, QeGetCharType(lpw[i])));
		vPatterns[i].push_back(piChar);
	}

	QeFindRepetitions(lpw, n, vPatterns);
	QeFindNumbers(lpw, n, vPatterns);
	QeFindDiffSeqs(lpw, n, vPatterns);
	QeFindPopularPasswords(lpw, n, vPatterns);

	// Encoders must not be static, because the entropy estimation
	// may run concurrently in multiple threads and the encoders are
	// not read-only
	CEntropyEncoder ecPattern(QE_PAT_ALL, 0, 1, 0);
	CMultiEntropyEncoder mcData;
	for(size_t i = 0; i < (m_vCharTypes.size() - 1); ++i)
	{
		// Let m be the alphabet size. In order to ensure that two same
		// characters cost at least as much as a single character, for
		// the probability p and weight w of the character it must hold:
		//     -log(1/m) >= -2*log(p)
		// <=> log(1/m) <= log(p^2) <=> 1/m <= p^2 <=> p >= sqrt(1/m);
		//     sqrt(1/m) = (1+w)/(m+w)
		// <=> m+w = (1+w)*sqrt(m) <=> m+w = sqrt(m) + w*sqrt(m)
		// <=> w*(1-sqrt(m)) = sqrt(m) - m <=> w = (sqrt(m)-m)/(1-sqrt(m))
		// <=> w = (sqrt(m)-m)*(1+sqrt(m))/(1-m)
		// <=> w = (sqrt(m)-m+m-m*sqrt(m))/(1-m) <=> w = sqrt(m)
		const UINT64 uw = static_cast<UINT64>(sqrt(static_cast<double>(
			m_vCharTypes[i]->GetCharCount())));

		mcData.AddEncoder(m_vCharTypes[i]->GetTypeID(),
			boost::shared_ptr<CEntropyEncoder>(new CEntropyEncoder(
			m_vCharTypes[i]->GetAlphabet().c_str(), 1, uw, 1)));
	}

	double dblMinCost = static_cast<double>(INT_MAX);
	const DWORD tStart = GetTickCount();

	std::vector<boost::shared_ptr<CQePathState> > sRec;
	sRec.push_back(boost::shared_ptr<CQePathState>(new CQePathState(0,
		TqePatInstsPtr(new TqePatInsts()))));

	while(sRec.size() > 0)
	{
		const DWORD tDiff = GetTickCount() - tStart;
		if(tDiff > 500) break;

		boost::shared_ptr<CQePathState> s = sRec.back();
		sRec.pop_back();

		if(s->GetPosition() >= n)
		{
			ASSERT(s->GetPosition() == n);

			double dblCost = QeComputePathCost(*(s->GetPath().get()),
				lpw, ecPattern, mcData);
			if(dblCost < dblMinCost) dblMinCost = dblCost;
		}
		else
		{
			const TqePatInsts& vSubs = vPatterns[s->GetPosition()];
			for(int i = static_cast<int>(vSubs.size()) - 1; i >= 0; --i)
			{
				boost::shared_ptr<CQePatternInstance> pi = vSubs[i];
				ASSERT(pi->GetPosition() == s->GetPosition());
				ASSERT(pi->GetLength() >= 1);

				TqePatInstsPtr pvNewPath(new TqePatInsts(*(s->GetPath().get())));
				pvNewPath->push_back(pi);
				ASSERT(pvNewPath->size() == (s->GetPath()->size() + 1));

				boost::shared_ptr<CQePathState> sNew(new CQePathState(
					s->GetPosition() + pi->GetLength(), pvNewPath));
				sRec.push_back(sNew);
			}
		}
	}

	return static_cast<DWORD>(ceil(dblMinCost));
}
