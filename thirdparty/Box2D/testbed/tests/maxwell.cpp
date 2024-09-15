/*
* Copyright (c) 2014 Google, Inc.
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

// Game which adds some fun to Maxwell's demon.
// http://en.wikipedia.org/wiki/Maxwell's_demon
// The user's goal is to try to catch as many particles as possible in the
// bottom half of the container by splitting the container using a barrier
// with the 'a' key.   See Maxwell::Keyboard() for other controls.
class Maxwell : public Test
{
public:

	Maxwell()
	{
		m_density = k_densityDefault;
		m_position = k_containerHalfHeight;
		m_particleGroup = NULL;
		m_temperature = k_temperatureDefault;
		m_barrierBody = NULL;

		m_world->SetGravity(b2Vec2(0, 0));

		// Create the container.
		{
			b2BodyDef bd;
			b2Body* ground = m_world->CreateBody(&bd);
			b2ChainShape shape;
			const b2Vec2 vertices[4] = {
				b2Vec2(-k_containerHalfWidth, 0),
				b2Vec2(k_containerHalfWidth, 0),
				b2Vec2(k_containerHalfWidth, k_containerHeight),
				b2Vec2(-k_containerHalfWidth, k_containerHeight)};
			shape.CreateLoop(vertices, 4);
			b2FixtureDef def;
			def.shape = &shape;
			def.density = 0;
			def.restitution = 1.0;
			ground->CreateFixture(&def);
		}

		// Enable the barrier.
		EnableBarrier();
		// Create the particles.
		ResetParticles();
	}

	// Disable the barrier.
	void DisableBarrier()
	{
		if (m_barrierBody)
		{
			m_world->DestroyBody(m_barrierBody);
			m_barrierBody = NULL;
		}
	}

	// Enable the barrier.
	void EnableBarrier()
	{
		if (!m_barrierBody)
		{
			b2BodyDef bd;
			m_barrierBody = m_world->CreateBody(&bd);
			b2PolygonShape barrierShape;
			barrierShape.SetAsBox(k_containerHalfWidth, k_barrierHeight,
								  b2Vec2(0, m_position), 0);
			b2FixtureDef def;
			def.shape = &barrierShape;
			def.density = 0;
			def.restitution = 1.0;
			m_barrierBody->CreateFixture(&def);
		}
	}

	// Enable / disable the barrier.
	void ToggleBarrier()
	{
		if (m_barrierBody)
		{
			DisableBarrier();
		}
		else
		{
			EnableBarrier();
		}
	}

	// Destroy and recreate all particles.
	void ResetParticles()
	{
		if (m_particleGroup != NULL)
		{
			m_particleGroup->DestroyParticles();
			m_particleGroup = NULL;
		}

		m_particleSystem->SetRadius(k_containerHalfWidth / 20.0f);
		{
			b2PolygonShape shape;
			shape.SetAsBox(m_density * k_containerHalfWidth,
						   m_density * k_containerHalfHeight,
						   b2Vec2(0, k_containerHalfHeight), 0);
			b2ParticleGroupDef pd;
			pd.flags = b2_powderParticle;
			pd.shape = &shape;
			m_particleGroup = m_particleSystem->CreateParticleGroup(pd);
			b2Vec2* velocities =
				m_particleSystem->GetVelocityBuffer() +
				m_particleGroup->GetBufferIndex();
			for (int i = 0; i < m_particleGroup->GetParticleCount(); ++i)
			{
				b2Vec2& v = *(velocities + i);
				v.Set(RandomFloat() + 1.0f, RandomFloat() + 1.0f);
				v.Normalize();
				v *= m_temperature;
			}
		}
	}

	virtual void Step(Settings& settings)
	{
		Test::Step(settings);

		// Number of particles above (top) and below (bottom) the barrier.
		int32 top = 0;
		int32 bottom = 0;
		const int32 index = m_particleGroup->GetBufferIndex();
		b2Vec2* const velocities =
			m_particleSystem->GetVelocityBuffer() + index;
		b2Vec2* const positions =
			m_particleSystem->GetPositionBuffer() + index;

		for (int32 i = 0; i < m_particleGroup->GetParticleCount(); i++)
		{
			// Add energy to particles based upon the temperature.
			b2Vec2& v = velocities[i];
			v.Normalize();
			v *= m_temperature;

			// Keep track of the number of particles above / below the
			// divider / barrier position.
			b2Vec2& p = positions[i];
			if (p.y > m_position)
				top++;
			else
				bottom++;
		}

		// Calculate a score based upon the difference in pressure between the
		// upper and lower divisions of the container.
		const float topPressure = top / (k_containerHeight - m_position);
		const float botPressure = bottom / m_position;
		g_debugDraw.DrawString(
			10, 75, "A to toggle barrier\n= to increase density\n- to decrease density\n; to increase temperature\n\\ to decrease temperature\nScore: %f",
			topPressure > 0.0f ? botPressure / topPressure - 1.0f : 0.0f);
	}

	// Reset the particles and the barrier.
	void Reset()
	{
		DisableBarrier();
		ResetParticles();
		EnableBarrier();
	}

	// Move the divider / barrier.
	void MoveDivider(const float newPosition)
	{
		m_position = b2Clamp(newPosition, k_barrierMovementIncrement,
							 k_containerHeight - k_barrierMovementIncrement);
		Reset();
	}

	virtual void Keyboard(int key)
	{
		switch(key)
		{
		case GLFW_KEY_A:
			// Enable / disable the barrier.
			ToggleBarrier();
			break;
		case GLFW_KEY_EQUAL:
			// Increase the particle density.
			m_density = b2Min(m_density * k_densityStep, k_densityMax);
			Reset();
			break;
		case GLFW_KEY_MINUS:
			// Reduce the particle density.
			m_density = b2Max(m_density / k_densityStep, k_densityMin);
			Reset();
			break;
		case GLFW_KEY_PERIOD:
			// Move the location of the divider up.
			MoveDivider(m_position + k_barrierMovementIncrement);
			break;
		case GLFW_KEY_COMMA:
			// Move the location of the divider down.
			MoveDivider(m_position - k_barrierMovementIncrement);
			break;
		case GLFW_KEY_SEMICOLON:
			// Reduce the temperature (velocity of particles).
			m_temperature = b2Max(m_temperature - k_temperatureStep,
								  k_temperatureMin);
			Reset();
			break;
		case GLFW_KEY_BACKSLASH:
			// Increase the temperature (velocity of particles).
			m_temperature = b2Min(m_temperature + k_temperatureStep,
								  k_temperatureMax);
			Reset();
			break;
		default:
			Test::Keyboard(key);
			break;
		}
	}

	// Determine whether a point is in the container.
	bool InContainer(const b2Vec2& p) const
	{
		return p.x >= -k_containerHalfWidth && p.x <= k_containerHalfWidth &&
			p.y >= 0.0f && p.y <= k_containerHalfHeight * 2.0f;
	}

	virtual void MouseDown(const b2Vec2& p)
	{
		if (!InContainer(p))
		{
			Test::MouseDown(p);
		}
	}

	virtual void MouseUp(const b2Vec2& p)
	{
		// If the pointer is in the container.
		if (InContainer(p))
		{
			// Enable / disable the barrier.
			ToggleBarrier();
		}
		else
		{
			// Move the barrier to the touch position.
			MoveDivider(p.y);

			Test::MouseUp(p);
		}
	}

	float GetDefaultViewZoom() const
	{
		return 0.1f;
	}

	static Test* Create()
	{
		return new Maxwell;
	}

private:
	float m_density;
	float m_position;
	float m_temperature;
	b2Body* m_barrierBody;
	b2ParticleGroup* m_particleGroup;

private:
	static const float k_containerWidth;
	static const float k_containerHeight;
	static const float k_containerHalfWidth;
	static const float k_containerHalfHeight;
	static const float k_barrierHeight;
	static const float k_barrierMovementIncrement;
	static const float k_densityStep;
	static const float k_densityMin;
	static const float k_densityMax;
	static const float k_densityDefault;
	static const float k_temperatureStep;
	static const float k_temperatureMin;
	static const float k_temperatureMax;
	static const float k_temperatureDefault;
};

const float Maxwell::k_containerWidth = 2.0f;
const float Maxwell::k_containerHeight = 4.0f;
const float Maxwell::k_containerHalfWidth =
	Maxwell::k_containerWidth / 2.0f;
const float Maxwell::k_containerHalfHeight =
	Maxwell::k_containerHeight / 2.0f;
const float Maxwell::k_barrierHeight =
	Maxwell::k_containerHalfHeight / 100.0f;
const float Maxwell::k_barrierMovementIncrement =
	Maxwell::k_containerHalfHeight * 0.1f;
const float Maxwell::k_densityStep = 1.25;
const float Maxwell::k_densityMin = 0.01f;
const float Maxwell::k_densityMax = 0.8f;
const float Maxwell::k_densityDefault = 0.25f;
const float Maxwell::k_temperatureStep = 0.2f;
const float Maxwell::k_temperatureMin = 0.4f;
const float Maxwell::k_temperatureMax = 10.0f;
const float Maxwell::k_temperatureDefault = 5.0f;

static int testIndex = RegisterTest("Particles", "Maxwell", Maxwell::Create);
