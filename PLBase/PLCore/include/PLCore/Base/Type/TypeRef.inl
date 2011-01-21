/*********************************************************\
 *  File: TypeRef.inl                                    *
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


#ifndef __PLCORE_TYPE_REF_INL__
#define __PLCORE_TYPE_REF_INL__
#pragma once


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
/**
*  @brief
*    Type wrapper for references 'T&'
*/
template <typename T>
class Type<T&> {


	//[-------------------------------------------------------]
	//[ Public static type information                        ]
	//[-------------------------------------------------------]
	public:
		// Type
		typedef T& _Type;

		// Type ID
		static const int TypeID = -1;

		// Get type name
		static PLGeneral::String GetTypeName()
		{
			return "void&";
		}

		// Convert var to pointer
		static T &ConvertFromVar(const DynVar *pValue)
		{
			// [TODO] Should be as big as possible (uint32/uint64)
			return *static_cast<T*>(pValue->GetInt());
		}

		// Convert pointer to bool
		static bool ConvertToBool(T &cValue)
		{
			return static_cast<bool>((&cValue) != nullptr);
		}

		// Convert bool to pointer
		static T &ConvertFromBool(bool bValue)
		{
			return *static_cast<T*>(nullptr);
		}

		// Convert pointer to int
		static int ConvertToInt(T &cValue)
		{
			return static_cast<int>(static_cast<PLGeneral::uint_ptr>(&cValue));
		}

		// Convert int to pointer
		static T &ConvertFromInt(int nValue)
		{
			return *static_cast<T*>(nValue);
		}

		// Convert pointer to int8
		static PLGeneral::int8 ConvertToInt8(T &cValue)
		{
			return static_cast<PLGeneral::int8>(static_cast<PLGeneral::uint_ptr>(&cValue));
		}

		// Convert int8 to pointer
		static T &ConvertFromInt8(PLGeneral::int8 nValue)
		{
			return *static_cast<T*>(nValue);
		}

		// Convert pointer to int16
		static PLGeneral::int16 ConvertToInt16(T &cValue)
		{
			return static_cast<PLGeneral::int16>(static_cast<PLGeneral::uint_ptr>(&cValue));
		}

		// Convert int16 to pointer
		static T &ConvertFromInt16(PLGeneral::int16 nValue)
		{
			return *static_cast<T*>(nValue);
		}

		// Convert pointer to int32
		static PLGeneral::int32 ConvertToInt32(T &cValue)
		{
			return static_cast<PLGeneral::int32>(static_cast<PLGeneral::uint_ptr>(&cValue));
		}

		// Convert int32 to pointer
		static T &ConvertFromInt32(PLGeneral::int32 nValue)
		{
			return *static_cast<T*>(nValue);
		}

		// Convert pointer to int64
		static PLGeneral::int64 ConvertToInt64(T &cValue)
		{
			return static_cast<PLGeneral::int64>(static_cast<PLGeneral::uint_ptr>(&cValue));
		}

		// Convert int64 to pointer
		static T &ConvertFromInt64(PLGeneral::int64 nValue)
		{
			return *static_cast<T*>(nValue);
		}

		// Convert pointer to uint8
		static PLGeneral::uint8 ConvertToUInt8(T &cValue)
		{
			return static_cast<PLGeneral::uint8>(static_cast<PLGeneral::uint_ptr>(&cValue));
		}

		// Convert uint8 to pointer
		static T &ConvertFromUInt8(PLGeneral::uint8 nValue)
		{
			return *static_cast<T*>(nValue);
		}

		// Convert pointer to uint16
		static PLGeneral::uint16 ConvertToUInt16(T &cValue)
		{
			return static_cast<PLGeneral::uint16>(static_cast<PLGeneral::uint_ptr>(&cValue));
		}

		// Convert uint16 to pointer
		static T &ConvertFromUInt16(PLGeneral::uint16 nValue)
		{
			return *static_cast<T*>(nValue);
		}

		// Convert pointer to uint32
		static PLGeneral::uint32 ConvertToUInt32(T &cValue)
		{
			return static_cast<PLGeneral::uint32>(static_cast<PLGeneral::uint_ptr>(&cValue));
		}

		// Convert uint32 to pointer
		static T &ConvertFromUInt32(PLGeneral::uint32 nValue)
		{
			return *static_cast<T*>(nValue);
		}

		// Convert pointer to uint64
		static PLGeneral::uint64 ConvertToUInt64(T &cValue)
		{
			return static_cast<PLGeneral::uint64>(static_cast<PLGeneral::uint_ptr>(&cValue));
		}

		// Convert uint64 to pointer
		static T &ConvertFromUInt64(PLGeneral::uint64 nValue)
		{
			return *static_cast<T*>(nValue);
		}

		// Convert pointer to uint_ptr
		static PLGeneral::uint_ptr ConvertToUIntPtr(T &cValue)
		{
			return static_cast<PLGeneral::uint_ptr>(&cValue);
		}

		// Convert uint_ptr to pointer
		static T &ConvertFromUIntPtr(PLGeneral::uint_ptr nValue)
		{
			return *static_cast<T*>(nValue);
		}

		// Convert pointer to float
		static float ConvertToFloat(T &cValue)
		{
			// No conversion from pointer types in non-integral types!
			return 0.0f;
		}

		// Convert float to pointer
		static T &ConvertFromFloat(float fValue)
		{
			// No conversion from pointer types in non-integral types!
			return *static_cast<T*>(nullptr);
		}

		// Convert pointer to double
		static double ConvertToDouble(T &cValue)
		{
			// No conversion from pointer types in non-integral types!
			return 0.0;
		}

		// Convert double to pointer
		static T &ConvertFromDouble(double dValue)
		{
			// No conversion from pointer types in non-integral types!
			return *static_cast<T*>(nullptr);
		}

		// Convert pointer to string
		static PLGeneral::String ConvertToString(T &cValue)
		{
			return PLGeneral::String() + reinterpret_cast<PLGeneral::uint_ptr>(reinterpret_cast<T*>(&cValue));
		}

		// Convert string to pointer
		static T &ConvertFromString(const PLGeneral::String &sString)
		{
			return *reinterpret_cast<T*>(sString.GetUIntPtr());
		}

};


#endif // __PLCORE_TYPE_REF_INL__
