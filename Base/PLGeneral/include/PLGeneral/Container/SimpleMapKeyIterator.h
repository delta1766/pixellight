/*********************************************************\
 *  File: SimpleMapKeyIterator.h                         *
 *
 *  Copyright (C) 2002-2011 The PixelLight Team (http://www.pixellight.org/)
 *
 *  This file is part of PixelLight.
 *
 *  PixelLight is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  PixelLight is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with PixelLight. If not, see <http://www.gnu.org/licenses/>.
\*********************************************************/


#ifndef __PLGENERAL_CONTAINER_SIMPLEMAPKEYITERATOR_H__
#define __PLGENERAL_CONTAINER_SIMPLEMAPKEYITERATOR_H__
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "PLGeneral/PLGeneral.h"
#include "PLGeneral/Container/Iterator.h"
#include "PLGeneral/Container/IteratorImpl.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace PLGeneral {


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
template <class KeyType, class ValueType, class Comparer> class SimpleMap;


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
/**
*  @brief
*    Simple map key iterator class
*/
template <class KeyType, class ValueType, class Comparer>
class SimpleMapKeyIterator : public IteratorImpl<KeyType> {


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
	friend class SimpleMap<KeyType, ValueType, Comparer>;


	//[-------------------------------------------------------]
	//[ Private functions                                     ]
	//[-------------------------------------------------------]
	private:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] mapOwner
		*    Map to operate on
		*  @param[in] nIndex
		*    Start index, if >= GetNumOfElements() the index is set to the last valid index
		*/
		SimpleMapKeyIterator(const SimpleMap<KeyType, ValueType, Comparer> &mapOwner, uint32 nIndex);

		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] mapOwner
		*    Map to operate on
		*
		*  @note
		*    - The iterator will start at the last element
		*/
		SimpleMapKeyIterator(const SimpleMap<KeyType, ValueType, Comparer> &mapOwner);

		/**
		*  @brief
		*    Copy constructor
		*
		*  @param[in] cSource
		*    Source to copy from
		*/
		SimpleMapKeyIterator(const SimpleMapKeyIterator<KeyType, ValueType, Comparer> &cSource);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~SimpleMapKeyIterator();


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		const SimpleMap<KeyType, ValueType, Comparer>						 *m_pmapOwner;	/**< Simple map to operate on (always valid!) */
		Iterator<typename SimpleMap<KeyType, ValueType, Comparer>::Element > *m_pIterator;	/**< Internal iterator (always valid!) */


	//[-------------------------------------------------------]
	//[ Private virtual IteratorImpl functions                ]
	//[-------------------------------------------------------]
	private:
		virtual IteratorImpl<KeyType> *Clone() const;
		virtual bool HasNext() const;
		virtual KeyType &Next();
		virtual bool HasPrevious() const;
		virtual KeyType &Previous();


};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // PLGeneral


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "PLGeneral/Container/SimpleMapKeyIterator.inl"


#endif // __PLGENERAL_CONTAINER_SIMPLEMAPKEYITERATOR_H__