/*********************************************************\
 *  File: IndexBuffer.h                                  *
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


#ifndef __PLRENDEREROPENGLES_INDEXBUFFER_H__
#define __PLRENDEREROPENGLES_INDEXBUFFER_H__
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <PLRenderer/Renderer/IndexBuffer.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace PLRendererOpenGLES {


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
/**
*  @brief
*    OpenGL ES index buffer
*/
class IndexBuffer : public PLRenderer::IndexBuffer {


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
	friend class Renderer;


	//[-------------------------------------------------------]
	//[ Public functions                                      ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Destructor
		*/
		virtual ~IndexBuffer();

		/**
		*  @brief
		*    Makes this index buffer to the renderers current index buffer
		*
		*  @return
		*   'true' if all went fine, else 'false'
		*
		*  @note
		*    - This function is used by the renderer when SetIndexBuffer() was called
		*/
		bool MakeCurrent();

		/**
		*  @brief
		*    Returns the dynamic data (none IBO)
		*
		*  @return
		*    Dynamic data, can be a null pointer (if no IBO if used!)
		*/
		void *GetDynamicData() const;


	//[-------------------------------------------------------]
	//[ Private functions                                     ]
	//[-------------------------------------------------------]
	private:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] cRenderer
		*    Owner renderer
		*/
		IndexBuffer(PLRenderer::Renderer &cRenderer);


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		PLGeneral::uint32  m_nIndexBuffer;	/**< OpenGL ES index buffer (IBO) */
		void			  *m_pData;			/**< Dynamic buffer, can be a null pointer (none IBO) */
		void			  *m_pLockedData;	/**< Locked data, can be a null pointer */
		bool			   m_bLockReadOnly;	/**< Read only lock? */
		bool			   m_bUpdateIBO;	/**< Do we need to update the IBO? */
		PLGeneral::uint32  m_nUsageAPI;		/**< API usage setting */


	//[-------------------------------------------------------]
	//[ Public virtual PLRenderer::Buffer functions           ]
	//[-------------------------------------------------------]
	public:
		virtual bool IsAllocated() const;
		virtual bool Allocate(PLGeneral::uint32 nElements, PLRenderer::Usage::Enum nUsage = PLRenderer::Usage::Dynamic, bool bManaged = true, bool bKeepData = false);
		virtual bool Clear();
		virtual void *Lock(PLGeneral::uint32 nFlag = PLRenderer::Lock::ReadWrite);
		virtual void *GetData();
		virtual bool Unlock();


	//[-------------------------------------------------------]
	//[ Private virtual PLRenderer::Resource functions        ]
	//[-------------------------------------------------------]
	private:
		virtual void BackupDeviceData(PLGeneral::uint8 **ppBackup);
		virtual void RestoreDeviceData(PLGeneral::uint8 **ppBackup);


};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // PLRendererOpenGLES


#endif // __PLRENDEREROPENGLES_INDEXBUFFER_H__