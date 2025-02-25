/*
 * TRAKTOR
 * Copyright (c) 2022-2023 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Render/PrimitiveRenderer.h"
#include "Scene/Editor/CameraMesh.h"
#include "Scene/Editor/DefaultComponentEditor.h"
#include "Scene/Editor/EntityAdapter.h"
#include "Scene/Editor/SceneEditorContext.h"
#include "World/Entity/CameraComponentData.h"
#include "World/Entity/LightComponentData.h"
#include "World/Entity/VolumeComponentData.h"

namespace traktor::scene
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.scene.DefaultComponentEditor", DefaultComponentEditor, IComponentEditor)

DefaultComponentEditor::DefaultComponentEditor(SceneEditorContext* context, EntityAdapter* entityAdapter, world::IEntityComponentData* componentData)
:	m_context(context)
,	m_entityAdapter(entityAdapter)
,	m_componentData(componentData)
{
}

void DefaultComponentEditor::transformModified(EntityAdapter* entityAdapter, EntityAdapter* modifiedEntityAdapter)
{
}

void DefaultComponentEditor::drawGuide(render::PrimitiveRenderer* primitiveRenderer) const
{
	const Transform transform = m_entityAdapter->getTransform();

	if (is_a< world::CameraComponentData >(m_componentData))
	{
		const Scalar distance = (primitiveRenderer->getView() * transform.toMatrix44()).translation().xyz0().length();
		const Scalar s = clamp(distance / 8.0_simd, 0.01_simd, 1.0_simd);

		primitiveRenderer->pushWorld(transform.toMatrix44() * scale(s, s, s));
		primitiveRenderer->pushDepthState(false, false, false);

		primitiveRenderer->drawWireAabb(
			Vector4::origo(),
			Vector4(0.1f, 0.1f, 0.1f, 0.0f),
			1.0f,
			Color4ub(255, 255, 255, 255)
		);

		for (int j = 0; j < sizeof_array(c_cameraMeshIndices); j += 2)
		{
			const int32_t i1 = c_cameraMeshIndices[j + 0] - 1;
			const int32_t i2 = c_cameraMeshIndices[j + 1] - 1;

			const float* v1 = &c_cameraMeshVertices[i1 * 3];
			const float* v2 = &c_cameraMeshVertices[i2 * 3];

			primitiveRenderer->drawLine(
				Vector4(v1[0], v1[1], v1[2], 1.0f),
				Vector4(v2[0], v2[1], v2[2], 1.0f),
				Color4ub(255, 255, 255, 255)
			);
		}

		primitiveRenderer->popDepthState();
		primitiveRenderer->popWorld();
	}

	if (const world::LightComponentData* lightComponent = dynamic_type_cast< const world::LightComponentData* >(m_componentData))
	{
		if (m_context->shouldDrawGuide(L"Entity.Light"))
		{
			const Vector4 lightPosition = transform.translation();
			const Vector4 lightDirection = -transform.axisY();
			const Vector4 lightX = transform.axisX();
			const Vector4 lightZ = transform.axisZ();

			if (lightComponent->getLightType() == world::LightType::Directional)
			{
				primitiveRenderer->pushDepthState(true, true, false);

				primitiveRenderer->drawLine(
					lightPosition - lightDirection * Scalar(0.5f),
					lightPosition + lightDirection * Scalar(0.5f),
					5.0f,
					Color4ub(255, 255, 0)
				);
				primitiveRenderer->drawArrowHead(
					lightPosition + lightDirection * Scalar(0.5f),
					lightPosition + lightDirection * Scalar(0.7f),
					0.5f,
					Color4ub(255, 255, 0)
				);

				primitiveRenderer->popDepthState();
			}
			else if (lightComponent->getLightType() == world::LightType::Point)
			{
				primitiveRenderer->pushDepthState(true, true, false);

				primitiveRenderer->drawSolidPoint(lightPosition, 8.0f, Color4ub(255, 255, 0));
				primitiveRenderer->drawWireSphere(translate(lightPosition), lightComponent->getFarRange(), Color4ub(255, 255, 0, 128));

				primitiveRenderer->popDepthState();
			}
			else if (lightComponent->getLightType() == world::LightType::Spot)
			{
				primitiveRenderer->pushDepthState(true, true, false);

				primitiveRenderer->drawSolidPoint(lightPosition, 8.0f, Color4ub(255, 255, 0));

				Frustum spotFrustum;
				spotFrustum.buildPerspective(lightComponent->getRadius(), 1.0f, lightComponent->getNearRange(), lightComponent->getFarRange());

				primitiveRenderer->pushWorld(transform.toMatrix44() * rotateX(deg2rad(90.0f)));
				primitiveRenderer->drawWireQuad(
					spotFrustum.corners[0],
					spotFrustum.corners[1],
					spotFrustum.corners[2],
					spotFrustum.corners[3],
					Color4ub(255, 255, 255)
				);
				primitiveRenderer->drawWireQuad(
					spotFrustum.corners[4],
					spotFrustum.corners[5],
					spotFrustum.corners[6],
					spotFrustum.corners[7],
					Color4ub(255, 255, 255)
				);
				primitiveRenderer->drawLine(
					spotFrustum.corners[0],
					spotFrustum.corners[4],
					Color4ub(255, 255, 255)
				);
				primitiveRenderer->drawLine(
					spotFrustum.corners[1],
					spotFrustum.corners[5],
					Color4ub(255, 255, 255)
				);
				primitiveRenderer->drawLine(
					spotFrustum.corners[2],
					spotFrustum.corners[6],
					Color4ub(255, 255, 255)
				);
				primitiveRenderer->drawLine(
					spotFrustum.corners[3],
					spotFrustum.corners[7],
					Color4ub(255, 255, 255)
				);
				primitiveRenderer->popWorld();

				primitiveRenderer->popDepthState();
			}
		}
	}

	if (const world::VolumeComponentData* volumeComponent = dynamic_type_cast< const world::VolumeComponentData* >(m_componentData))
	{
		if (m_context->shouldDrawGuide(L"Entity.Volumes"))
		{
			primitiveRenderer->pushWorld(transform.toMatrix44());

			const Aabb3 boundingBox = volumeComponent->getBoundingBox();

			primitiveRenderer->drawSolidAabb(boundingBox, Color4ub(120, 255, 120, 80));
			primitiveRenderer->drawWireAabb(boundingBox, 1.0f, Color4ub(120, 255, 120, 255));

			primitiveRenderer->popWorld();
		}
	}
}

}
