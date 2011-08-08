/*********************************************************\
 *  File: FrontendImpl.cpp                               *
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


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "PLCore/Log/Log.h"
#include "PLCore/Base/Class.h"
#include "PLCore/System/System.h"
#include "PLCore/Frontend/Frontend.h"
#include "PLCore/Frontend/FrontendImpl.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace PLCore {


//[-------------------------------------------------------]
//[ RTTI interface                                        ]
//[-------------------------------------------------------]
pl_implement_class(FrontendImpl)


//[-------------------------------------------------------]
//[ Public functions                                      ]
//[-------------------------------------------------------]
/**
*  @brief
*    Constructor
*/
FrontendImpl::FrontendImpl() :
	m_pFrontend(nullptr)
{
}

/**
*  @brief
*    Destructor
*/
FrontendImpl::~FrontendImpl()
{
}

/**
*  @brief
*    Returns the frontend instance
*/
Frontend *FrontendImpl::GetFrontend() const
{
	return m_pFrontend;
}


//[-------------------------------------------------------]
//[ Protected virtual AbstractLifecycle functions         ]
//[-------------------------------------------------------]
void FrontendImpl::OnCreate()
{
	// No implementation required
}

void FrontendImpl::OnRestart()
{
	// Call virtual function from frontend
	if (m_pFrontend)
		m_pFrontend->OnRestart();
}

bool FrontendImpl::OnStart()
{
	// Call virtual function from frontend
	return m_pFrontend ? m_pFrontend->OnStart() : true;
}

void FrontendImpl::OnResume()
{
	// Call virtual function from frontend
	if (m_pFrontend)
		m_pFrontend->OnResume();
}

void FrontendImpl::OnPause()
{
	// Call virtual function from frontend
	if (m_pFrontend)
		m_pFrontend->OnPause();
}

void FrontendImpl::OnStop()
{
	// Call virtual function from frontend
	if (m_pFrontend)
		m_pFrontend->OnStop();
}

void FrontendImpl::OnDestroy()
{
	// No implementation required
}


//[-------------------------------------------------------]
//[ Protected virtual AbstractFrontend functions          ]
//[-------------------------------------------------------]
void FrontendImpl::OnSize()
{
	// Call virtual function from frontend
	if (m_pFrontend)
		m_pFrontend->OnSize();
}

void FrontendImpl::OnDisplayMode()
{
	// Call virtual function from frontend
	if (m_pFrontend)
		m_pFrontend->OnDisplayMode();
}

void FrontendImpl::OnFullscreenMode()
{
	// Call virtual function from frontend
	if (m_pFrontend)
		m_pFrontend->OnFullscreenMode();
}

void FrontendImpl::OnDraw()
{
	// Call virtual function from frontend
	if (m_pFrontend)
		m_pFrontend->OnDraw();
}

void FrontendImpl::OnUpdate()
{
	// Call virtual function from frontend
	if (m_pFrontend)
		m_pFrontend->OnUpdate();
}


//[-------------------------------------------------------]
//[ Protected virtual FrontendImpl functions              ]
//[-------------------------------------------------------]
/**
*  @brief
*    Called when the frontend should run
*/
int FrontendImpl::Run(const String &sExecutableFilename, const Array<String> &lstArguments)
{
	// Nothing to do in here
	return 0;
}

/**
*  @brief
*    Run the frontend using traditional C-arguments
*/
int FrontendImpl::Run(int argc, char **argv)
{
	// By default, call the version of the "Run()"-method using PixelLight strings by using the same approach used in "PLMain()"
	Array<String> lstArguments;
	for (int i=1; i<argc; i++)
		lstArguments.Add(argv[i]);
	return Run(System::GetInstance()->GetExecutableFilename(), lstArguments);
}

int FrontendImpl::Run(int argc, wchar_t **argv)
{
	// By default, call the version of the "Run()"-method using PixelLight strings by using the same approach used in "PLMain()"
	Array<String> lstArguments;
	for (int i=1; i<argc; i++)
		lstArguments.Add(argv[i]);
	return Run(System::GetInstance()->GetExecutableFilename(), lstArguments);
}


//[-------------------------------------------------------]
//[ Protected static functions                            ]
//[-------------------------------------------------------]
/**
*  @brief
*    Creates a frontend instance
*/
Frontend *FrontendImpl::CreateFrontend(FrontendImpl &cFrontendImpl,
									   const String &sFrontend,
									   const String &sFrontendConstructor,
									   const String &sFrontendConstructorParameters,
									   const String &sFrontendParameters)
{
	// Get the frontend RTTI class
	const Class *pClass = ClassManager::GetInstance()->GetClass(sFrontend);
	if (pClass && pClass->IsDerivedFrom("PLCore::Frontend")) {
		// Create the frontend RTTI class instance
		Object *pObject = sFrontendConstructor.GetLength() ? pClass->Create(sFrontendConstructor, "FrontendImpl=\"" + Type<FrontendImpl&>::ConvertToString(cFrontendImpl) + "\" " + sFrontendConstructorParameters) : pClass->Create(Params<Object*, FrontendImpl&>(cFrontendImpl));
		if (pObject) {
			// Cast the pointer to a frontend pointer
			Frontend *pFrontend = static_cast<Frontend*>(pObject);

			// Write down a log message
			PL_LOG(Info, "Using frontend '" + pClass->GetClassName() + "': " + pClass->GetDescription())

			// Set parameters for the instanced frontend RTTI class
			if (sFrontendParameters.GetLength())
				pObject->SetValues(sFrontendParameters);

			// Done
			return pFrontend;
		} else {
			// Error!
			PL_LOG(Error, "Failed instancing frontend '" + pClass->GetClassName() + "': " + pClass->GetDescription())
		}
	} else {
		// Error!
		PL_LOG(Error, "Frontend '" + sFrontend + "' is no valid frontend RTTI class")
	}

	// Error!
	return nullptr;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // PLCore