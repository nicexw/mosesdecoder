// $Id: AlignmentPhrase.h 87 2007-09-10 23:17:43Z hieu $
/***********************************************************************
Moses - factored phrase-based language decoder
Copyright (C) 2006 University of Edinburgh

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
***********************************************************************/

#pragma once

#include <iostream>
#include <vector>
#include "AlignmentElement.h"
#include "Util.h"

class WordsRange;
class WordsBitmap;

//! alignments of each word in a phrase
class AlignmentPhrase
{
	friend std::ostream& operator<<(std::ostream& out, const AlignmentPhrase &alignmentPhrase);
public:
	typedef std::vector<AlignmentElement*> CollectionType;
protected:
	CollectionType  m_collection;
public:
	AlignmentPhrase()
	{}
	AlignmentPhrase(const AlignmentPhrase &copy);
	AlignmentPhrase(size_t size);
	~AlignmentPhrase();

	/** compare with another alignment phrase, return true if the other alignment phrase is a 
		*	subset of this. Used to see whether a trans opt can be used to expand a hypo
		*/
	bool IsCompatible(const AlignmentPhrase &compare
										, size_t mergePosStart
										, size_t shiftPos
										, bool allowSourceNullAlign = false) const;

	//! add newAlignment to end of this alignment phrase, offsetting by newAlignmentRange.GetStartPos()
	void Add(const AlignmentPhrase &newAlignment, size_t shift, size_t startPos);

	/*< merge newAlignment to this alignment phrase, offsetting by newAlignmentRange.GetStartPos().
			Use intersection of each alignment element
	*/
	void Merge(const AlignmentPhrase &newAlignment, size_t shift, size_t startPos);

	size_t GetSize() const
	{
		return m_collection.size();
	}

	CollectionType &GetVector()
	{
		return m_collection;
	}

	void Add(const AlignmentElement &element)
	{
		m_collection.push_back(new AlignmentElement(element));
	}

	bool IsCompletable(size_t decodeStepId
										, const WordsBitmap &thisCompleted
										, const WordsBitmap &otherCompleted) const;
				
	// add elements which didn't have alignments, so are set to uniform on the other side
	void AddUniformAlignmentElement(std::list<size_t> &uniformAlignmentTarget);

	AlignmentElement &GetElement(size_t pos)
	{ return *m_collection[pos];	}

	const AlignmentElement &GetElement(size_t pos) const
	{ return *m_collection[pos];	}

	bool Exists(size_t pos) const
	{
		return m_collection[pos] != NULL;
	}

	TO_STRING();									
};


