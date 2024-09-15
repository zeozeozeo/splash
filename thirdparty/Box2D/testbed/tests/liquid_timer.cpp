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

class LiquidTimer : public Test
{
public:

	LiquidTimer()
	{

		{
			b2BodyDef bd;
			b2Body* ground = m_world->CreateBody(&bd);

			b2ChainShape shape;
			const b2Vec2 vertices[4] = {
				b2Vec2(-2, 0),
				b2Vec2(2, 0),
				b2Vec2(2, 4),
				b2Vec2(-2, 4)};
			shape.CreateLoop(vertices, 4);
			ground->CreateFixture(&shape, 0.0f);

		}

		m_particleSystem->SetRadius(0.025f);
		{
			b2PolygonShape shape;
			shape.SetAsBox(2, 0.4f, b2Vec2(0, 3.6f), 0);
			b2ParticleGroupDef pd;
			pd.flags = b2_waterParticle | b2_colorMixingParticle;
			pd.shape = &shape;
			b2ParticleGroup * const group = m_particleSystem->CreateParticleGroup(pd);
			if (pd.flags & b2_colorMixingParticle) {
				ColorParticleGroup(group, 0);
			}
		}

		{
			b2BodyDef bd;
			b2Body* body = m_world->CreateBody(&bd);
			b2EdgeShape shape;
			shape.SetTwoSided(b2Vec2(-2, 3.2f), b2Vec2(-1.2f, 3.2f));
			body->CreateFixture(&shape, 0.1f);
		}

		{
			b2BodyDef bd;
			b2Body* body = m_world->CreateBody(&bd);
			b2EdgeShape shape;
			shape.SetTwoSided(b2Vec2(-1.1f, 3.2f), b2Vec2(2, 3.2f));
			body->CreateFixture(&shape, 0.1f);
		}

		{
			b2BodyDef bd;
			b2Body* body = m_world->CreateBody(&bd);
			b2EdgeShape shape;
			shape.SetTwoSided(b2Vec2(-1.2f, 3.2f), b2Vec2(-1.2f, 2.8f));
			body->CreateFixture(&shape, 0.1f);
		}

		{
			b2BodyDef bd;
			b2Body* body = m_world->CreateBody(&bd);
			b2EdgeShape shape;
			shape.SetTwoSided(b2Vec2(-1.1f, 3.2f), b2Vec2(-1.1f, 2.8f));
			body->CreateFixture(&shape, 0.1f);
		}

		{
			b2BodyDef bd;
			b2Body* body = m_world->CreateBody(&bd);
			b2EdgeShape shape;
			shape.SetTwoSided(b2Vec2(-1.6f, 2.4f), b2Vec2(0.8f, 2));
			body->CreateFixture(&shape, 0.1f);
		}

		{
			b2BodyDef bd;
			b2Body* body = m_world->CreateBody(&bd);
			b2EdgeShape shape;
			shape.SetTwoSided(b2Vec2(1.6f, 1.6f), b2Vec2(-0.8f, 1.2f));
			body->CreateFixture(&shape, 0.1f);
		}

		{
			b2BodyDef bd;
			b2Body* body = m_world->CreateBody(&bd);
			b2EdgeShape shape;
			shape.SetTwoSided(b2Vec2(-1.2f, 0.8f), b2Vec2(-1.2f, 0));
			body->CreateFixture(&shape, 0.1f);
		}

		{
			b2BodyDef bd;
			b2Body* body = m_world->CreateBody(&bd);
			b2EdgeShape shape;
			shape.SetTwoSided(b2Vec2(-0.4f, 0.8f), b2Vec2(-0.4f, 0));
			body->CreateFixture(&shape, 0.1f);
		}

		{
			b2BodyDef bd;
			b2Body* body = m_world->CreateBody(&bd);
			b2EdgeShape shape;
			shape.SetTwoSided(b2Vec2(0.4f, 0.8f), b2Vec2(0.4f, 0));
			body->CreateFixture(&shape, 0.1f);
		}

		{
			b2BodyDef bd;
			b2Body* body = m_world->CreateBody(&bd);
			b2EdgeShape shape;
			shape.SetTwoSided(b2Vec2(1.2f, 0.8f), b2Vec2(1.2f, 0));
			body->CreateFixture(&shape, 0.1f);
		}
	}

	float GetDefaultViewZoom() const
	{
		return 0.1f;
	}

	static Test* Create()
	{
		return new LiquidTimer;
	}
};

static int testIndex = RegisterTest("Particles", "Liquid Timer", LiquidTimer::Create);
