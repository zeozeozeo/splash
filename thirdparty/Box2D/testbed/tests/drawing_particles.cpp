/*
* Copyright (c) 2013 Google, Inc.
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/
#include "test.h"

class DrawingParticles : public Test
{
public:

	DrawingParticles()
	{
		{
			b2BodyDef bd;
			b2Body* ground = m_world->CreateBody(&bd);

			{
				b2PolygonShape shape;
				const b2Vec2 vertices[4] = {
					b2Vec2(-4, -2),
					b2Vec2(4, -2),
					b2Vec2(4, 0),
					b2Vec2(-4, 0)};
				shape.Set(vertices, 4);
				ground->CreateFixture(&shape, 0.0f);
			}

			{
				b2PolygonShape shape;
				const b2Vec2 vertices[4] = {
					b2Vec2(-4, -2),
					b2Vec2(-2, -2),
					b2Vec2(-2, 6),
					b2Vec2(-4, 6)};
				shape.Set(vertices, 4);
				ground->CreateFixture(&shape, 0.0f);
			}

			{
				b2PolygonShape shape;
				const b2Vec2 vertices[4] = {
					b2Vec2(2, -2),
					b2Vec2(4, -2),
					b2Vec2(4, 6),
					b2Vec2(2, 6)};
				shape.Set(vertices, 4);
				ground->CreateFixture(&shape, 0.0f);
			}

			{
				b2PolygonShape shape;
				const b2Vec2 vertices[4] = {
					b2Vec2(-4, 4),
					b2Vec2(4, 4),
					b2Vec2(4, 6),
					b2Vec2(-4, 6)};
				shape.Set(vertices, 4);
				ground->CreateFixture(&shape, 0.0f);
			}
		}

		m_colorIndex = 0;
		m_particleSystem->SetRadius(0.05f);
		m_lastGroup = NULL;
		m_drawing = true;
	}

	void MouseMove(const b2Vec2& p)
	{
		Test::MouseMove(p);
		if (m_drawing)
		{
			b2CircleShape shape;
			shape.m_p = p;
			shape.m_radius = 0.2f;
			b2Transform xf;
			xf.SetIdentity();

			m_particleSystem->DestroyParticlesInShape(shape, xf);

			const bool joinGroup =
				m_lastGroup;
			if (!joinGroup)
			{
				m_colorIndex = (m_colorIndex + 1) % k_ParticleColorsCount;
			}
			b2ParticleGroupDef pd;
			pd.shape = &shape;
			pd.flags = b2_waterParticle;
			pd.groupFlags = 0;
			pd.color = k_ParticleColors[m_colorIndex];
			pd.group = m_lastGroup;
			m_lastGroup = m_particleSystem->CreateParticleGroup(pd);
			m_mouseTracing = false;
		}
	}

	void MouseUp(const b2Vec2& p)
	{
		Test::MouseUp(p);
		m_lastGroup = NULL;
	}

	void ParticleGroupDestroyed(b2ParticleGroup* group) {
		if (group == m_lastGroup)
		{
			m_lastGroup = NULL;
		}
	}

	void SplitParticleGroups()
	{
		for (b2ParticleGroup* group = m_particleSystem->
				GetParticleGroupList(); group; group = group->GetNext())
		{
			if (group != m_lastGroup &&
				(group->GetGroupFlags() & b2_rigidParticleGroup) &&
				(group->GetAllParticleFlags() & b2_zombieParticle))
			{
				// Split a rigid particle group which may be disconnected
				// by destroying particles.
				m_particleSystem->SplitParticleGroup(group);
			}
		}
	}

	void Step(Settings& settings) override
	{
		m_drawing = m_mouseDown;

		if (m_particleSystem->GetAllParticleFlags() & b2_zombieParticle)
		{
			SplitParticleGroups();
		}

		Test::Step(settings);
	}

	float GetDefaultViewZoom() const
	{
		return 0.1f;
	}

	static Test* Create()
	{
		return new DrawingParticles;
	}

	b2ParticleGroup* m_lastGroup;
	bool m_drawing;
	uint32 m_colorIndex;
	
	static const uint32 k_paramDefCount;
};

static int testIndex = RegisterTest("Particles", "Drawing Particles", DrawingParticles::Create);
