/*********************************************************\
 *  File: SPTessellationShaders.cpp                      *
 *
 *  Copyright (C) 2002-2012 The PixelLight Team (http://www.pixellight.org/)
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
#include <PLCore/Tools/Timing.h>
#include <PLMath/Matrix3x3.h>
#include <PLMath/Matrix4x4.h>
#include <PLMath/Rectangle.h>
#include <PLRenderer/Renderer/Renderer.h>
#include <PLRenderer/Renderer/IndexBuffer.h>
#include <PLRenderer/Renderer/VertexShader.h>
#include <PLRenderer/Renderer/ShaderLanguage.h>
#include <PLRenderer/Renderer/FragmentShader.h>
#include <PLRenderer/Renderer/ProgramWrapper.h>
#include <PLRenderer/Renderer/FragmentShader.h>
#include <PLRenderer/Renderer/TessellationControlShader.h>
#include <PLRenderer/Renderer/TessellationEvaluationShader.h>
#include "SPTessellationShaders.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
using namespace PLCore;
using namespace PLMath;
using namespace PLGraphics;
using namespace PLRenderer;


//[-------------------------------------------------------]
//[ RTTI interface                                        ]
//[-------------------------------------------------------]
pl_implement_class(SPTessellationShaders)


//[-------------------------------------------------------]
//[ Public functions                                      ]
//[-------------------------------------------------------]
/**
*  @brief
*    Constructor
*/
SPTessellationShaders::SPTessellationShaders(Renderer &cRenderer) : SPTessellation(cRenderer),
	m_pVertexShader(nullptr),
	m_pTessellationControlShader(nullptr),
	m_pTessellationEvaluationShader(nullptr),
	m_pGeometryShader(nullptr),
	m_pFragmentShader(nullptr),
	m_pProgram(nullptr)
{
	// Decide which shader language should be used (for example "GLSL" or "Cg")
	ShaderLanguage *pShaderLanguage = cRenderer.GetShaderLanguage(cRenderer.GetDefaultShaderLanguage());
	if (pShaderLanguage) {
		// Shader source code
		String sVertexShaderSourceCode;
		String sTessellationControlShaderSourceCode;
		String sTessellationEvaluationShaderSourceCode;
		String sGeometryShaderSourceCode;
		String sFragmentShaderSourceCode;
		if (pShaderLanguage->GetShaderLanguage() == "GLSL") {
			#include "SPTessellationShaders_GLSL.h"
			sVertexShaderSourceCode					= "#version 400\n" + sVertexShaderSourceCodeGLSL;
			sTessellationControlShaderSourceCode	= "#version 400\n" + sTessellationControlShaderSourceCodeGLSL;
			sTessellationEvaluationShaderSourceCode	= "#version 400\n" + sTessellationEvaluationShaderSourceCodeGLSL;
			sGeometryShaderSourceCode				= "#version 400\n" + sGeometryShaderSourceCodeGLSL;
			sFragmentShaderSourceCode				= "#version 400\n" + sFragmentShaderSourceCodeGLSL;
		} else if (pShaderLanguage->GetShaderLanguage() == "Cg") {
			// [TODO] Under construction
			#include "SPTessellationShaders_Cg.h"
		//	sVertexShaderSourceCode					= sVertexShaderSourceCodeCg;
		//	sTessellationControlShaderSourceCode	= sTessellationControlShaderSourceCodeCg;
		//	sTessellationEvaluationShaderSourceCode	= sTessellationEvaluationShaderSourceCodeCg;
		//	sGeometryShaderSourceCode				= sGeometryShaderSourceCodeCg;
		//	sFragmentShaderSourceCode				= sFragmentShaderSourceCodeCg;
		}

		// Create the shader instances
		m_pVertexShader					= pShaderLanguage->CreateVertexShader(sVertexShaderSourceCode);
		m_pTessellationControlShader	= pShaderLanguage->CreateTessellationControlShader(sTessellationControlShaderSourceCode);
		m_pTessellationEvaluationShader	= pShaderLanguage->CreateTessellationEvaluationShader(sTessellationEvaluationShaderSourceCode);
		m_pGeometryShader				= pShaderLanguage->CreateGeometryShader();
		if (m_pGeometryShader)
			m_pGeometryShader->SetSourceCode(sGeometryShaderSourceCode, GeometryShader::InputTriangles, GeometryShader::OutputTriangles, 3);
		m_pFragmentShader				= pShaderLanguage->CreateFragmentShader(sFragmentShaderSourceCode);

		// Create a program instance and assign the created shaders to it
		m_pProgram = static_cast<ProgramWrapper*>(pShaderLanguage->CreateProgram(m_pVertexShader, m_pTessellationControlShader, m_pTessellationEvaluationShader, m_pGeometryShader, m_pFragmentShader));
	}
}

/**
*  @brief
*    Destructor
*/
SPTessellationShaders::~SPTessellationShaders()
{
	// Cleanup
	if (m_pProgram)
		delete m_pProgram;
	if (m_pFragmentShader)
		delete m_pFragmentShader;
	if (m_pGeometryShader)
		delete m_pGeometryShader;
	if (m_pTessellationEvaluationShader)
		delete m_pTessellationEvaluationShader;
	if (m_pTessellationControlShader)
		delete m_pTessellationControlShader;
	if (m_pVertexShader)
		delete m_pVertexShader;
}


//[-------------------------------------------------------]
//[ Private virtual PLRenderer::SurfacePainter functions  ]
//[-------------------------------------------------------]
void SPTessellationShaders::OnPaint(Surface &cSurface)
{
	// Get the used renderer
	Renderer &cRenderer = GetRenderer();

	// Clear the content of the current used render target by using gray (this way, in case on an graphics error we might still see at least something)
	cRenderer.Clear(Clear::Color | Clear::ZBuffer, Color4::Gray);

	// Make our program to the current one
	if (cRenderer.SetProgram(m_pProgram)) {
		// Set "TessellationLevelInner" program uniform
		m_pProgram->Set("TessellationLevelInner", m_fTessellationLevelInner);

		// Set "TessellationLevelOuter" program uniform
		m_pProgram->Set("TessellationLevelOuter", m_fTessellationLevelOuter);

		// Set "ObjectSpaceToClipSpaceMatrix" program uniform
		ProgramUniform *pProgramUniform = m_pProgram->GetUniform("ObjectSpaceToClipSpaceMatrix");
		if (pProgramUniform) {
			// Calculate the world matrix
			Matrix4x4 mWorld;
			{
				// Build a rotation matrix by using a given Euler angle around the y-axis
				mWorld.FromEulerAngleY(static_cast<float>(m_fRotation*Math::DegToRad));
			}

			// Calculate the view matrix
			Matrix4x4 mView;
			{
				mView.SetTranslation(0.0f, 0.0f, -3.0f);
			}

			// Calculate the projection matrix
			Matrix4x4 mProjection;
			{
				const float fAspect      = 1.0f;
				const float fAspectRadio = cRenderer.GetViewport().GetWidth()/(cRenderer.GetViewport().GetHeight()*fAspect);
				mProjection.PerspectiveFov(static_cast<float>(45.0f*Math::DegToRad), fAspectRadio, 0.001f, 1000.0f);
			}

			// Calculate the final composed world view projection matrix
			const Matrix4x4 mWorldViewProjection = mProjection*mView*mWorld;

			// Set object space to clip space matrix uniform
			pProgramUniform->Set(mWorldViewProjection);

			// Set normal matrix uniform
			m_pProgram->Set("NormalMatrix", Matrix3x3(mWorldViewProjection));
		}

		// Set "LightPosition" program uniform
		m_pProgram->Set("LightPosition", 0.25f, 0.25f, 1.0f);

		// Set "DiffuseMaterial" program uniform
		m_pProgram->Set("DiffuseMaterial", 0.0f, 0.75f, 0.75f);

		// Set "AmbientMaterial" program uniform
		m_pProgram->Set("AmbientMaterial", 0.04f, 0.04f, 0.04f);

		// Set program vertex attributes, this creates a connection between "Vertex Buffer Attribute" and "Vertex Shader Attribute"
		m_pProgram->Set("VertexPosition", m_pVertexBuffer, VertexBuffer::Position);

		// No back face culling, please. Else we can only see one 'side' of the triangle
		cRenderer.SetRenderState(RenderState::CullMode, Cull::None);

		// Set index buffer
		cRenderer.SetIndexBuffer(m_pIndexBuffer);

		// Draw the geometry
		cRenderer.DrawIndexedPatches(
			3,
			0,
			m_pVertexBuffer->GetNumOfElements()-1,
			0,
			m_pIndexBuffer->GetNumOfElements()
		);
	}

	{ // Animation
	  // -> Normally, such work should NOT be performed within the rendering step, but we want
	  //    to keep the implementation simple in here...
		const float fTimeDifference = Timing::GetInstance()->GetTimeDifference();

		// Increase the rotation by the current time difference (time past since the last frame)
		m_fRotation += fTimeDifference*10;

		// Update inner tessellation level
		m_fTessellationLevelInner += fTimeDifference/3;
		if (m_fTessellationLevelInner > 10)
			m_fTessellationLevelInner = 1.0f;

		// Update outer tessellation level
		m_fTessellationLevelOuter += fTimeDifference;
		if (m_fTessellationLevelOuter > 10)
			m_fTessellationLevelOuter = 1.0f;
	}
}