#include "Box2D/Box2D.h"
#include "raylib.h"
#include <string>
#include <vector>

constexpr float LENGTH_UNIT = 128.f;

enum ShaderType {
    SHADER_FLUID,
    SHADER_MAX,
};

class entity {
public:
    entity(b2World* world, const b2BodyDef* def);

    constexpr b2Body* Body() const {
        return m_body;
    }

    inline void SetExtent(b2Vec2 extent) {
        m_extent = extent;
    }

    constexpr void SetColor(Color color) {
        m_color = color;
    }

    void draw() {
        auto p = m_body->GetWorldPoint({-m_extent.x, -m_extent.y});
        DrawRectanglePro({p.x, p.y, m_extent.x * 2.f, m_extent.y * 2.f},
                         {0.f, 0.f}, RAD2DEG * m_body->GetAngle(), m_color);
    }

private:
    b2Body* m_body;
    b2Vec2 m_extent;
    Color m_color = WHITE;
};

entity::entity(b2World* world, const b2BodyDef* def) {
    m_body = world->CreateBody(def);
}

class particle {
public:
    particle(b2ParticleSystem* ps, const b2ParticleDef& def);

private:
    int32_t m_index;
};

particle::particle(b2ParticleSystem* ps, const b2ParticleDef& def) {
    m_index = ps->CreateParticle(def);
    ps->GetParticleHandleFromIndex(0);
}

int main(void) {
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 720, "Splash!");

    // load assets
    auto gradientTex = LoadTexture("resources/gradient.png");
    auto backgroundTex = LoadTexture("resources/background.png");
    auto noiseTex = LoadTexture("resources/simplex.png");
    Shader shaders[SHADER_MAX] = {};
    shaders[SHADER_FLUID] = LoadShader(nullptr, "resources/shaders/fluid.fs");
    int gameTextureLoc =
        GetShaderLocation(shaders[SHADER_FLUID], "gameTexture");
    int noiseTextureLoc =
        GetShaderLocation(shaders[SHADER_FLUID], "noiseTexture");
    int iTimeLoc = GetShaderLocation(shaders[SHADER_FLUID], "iTime");
    int iResolutionLoc =
        GetShaderLocation(shaders[SHADER_FLUID], "iResolution");

    auto world = new b2World({0.f, 10.f * LENGTH_UNIT});
    std::vector<entity> entities;

    // Define the ground body.
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(GetScreenWidth() / 2.f,
                               GetScreenHeight() * 0.8f);

    // Call the body factory which allocates memory for the ground body
    // from a pool and creates the ground box shape (also from a pool).
    // The body is also added to the world.
    entity groundBody{world, &groundBodyDef};

    // Define the ground box shape.
    b2PolygonShape groundBox;

    // The extents are the half-widths of the box.
    groundBox.SetAsBox(200.0f, 10.0f);
    groundBody.SetExtent({200.0f, 10.0f});

    // Add the ground fixture to the ground body.
    groundBody.Body()->CreateFixture(&groundBox, 0.0f);

    // add bodies
    entities.push_back(std::move(groundBody));

    // particle system
    const auto radius = 4.f;
    b2ParticleSystemDef particleSystemDef;
    particleSystemDef.dampingStrength = 0.2f;
    particleSystemDef.radius = radius;
    particleSystemDef.surfaceTensionNormalStrength = 0.4f;
    particleSystemDef.surfaceTensionPressureStrength = 0.4f;
    particleSystemDef.viscousStrength = 0.05f;
    // particleSystemDef.viscousStrength = 0.9f;
    particleSystemDef.colorMixingStrength = 0.2f;
    auto particleSystem = world->CreateParticleSystem(&particleSystemDef);

    // b2PolygonShape shape;
    // shape.SetAsBox(GetScreenWidth(), GetScreenHeight(), {0, 0}, 0);
    // b2ParticleGroupDef pd;
    // pd.shape = &shape;
    // pd.flags = b2_tensileParticle | b2_colorMixingParticle;
    // b2ParticleGroup* const group = particleSystem->CreateParticleGroup(pd);

    auto lastSpawnTime = 0.f;
    uint32_t hue = 0;
    float prevMouseX{}, prevMouseY{};

    auto fluidTarget = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    auto gameTarget = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());

    while (!WindowShouldClose()) {
        if (IsWindowResized()) {
            // reallocate renderbuffers
            UnloadRenderTexture(fluidTarget);
            fluidTarget =
                LoadRenderTexture(GetScreenWidth(), GetScreenHeight());

            UnloadRenderTexture(gameTarget);
            gameTarget = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
        }

        auto mx = GetMouseX();
        auto my = GetMouseY();
        auto mouseVelX = mx - prevMouseX;
        auto mouseVelY = my - prevMouseY;
        prevMouseX = mx;
        prevMouseY = my;

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) &&
            GetTime() - lastSpawnTime > 0.03f) {
            b2BodyDef bodyDef;
            bodyDef.type = b2_staticBody;
            bodyDef.position.Set(GetMouseX() + 5.f, GetMouseY() + 5.f);
            entity box{world, &bodyDef};

            b2PolygonShape dynamicBox;
            dynamicBox.SetAsBox(10.0f, 10.0f);
            box.SetExtent({10.0f, 10.0f});
            box.SetColor(ORANGE);

            b2FixtureDef fixtureDef;
            fixtureDef.shape = &dynamicBox;
            fixtureDef.density = 1.f;
            fixtureDef.friction = 1.f;
            box.Body()->CreateFixture(&fixtureDef);

            entities.push_back(std::move(box));
            lastSpawnTime = GetTime();
        }

        if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
            for (int i = 0; i < 10; i++) {
                b2ParticleDef particleDef;
                auto color = ColorFromHSV(hue, 1.f, 1.f);
                hue = (hue + 1) % 360;
                // auto r = GetRandomValue(230, 255);
                // particleDef.color.Set(0, 166, 255, 255);
                particleDef.color.Set(color.r, color.g, color.b, 255);
                auto r1 = GetRandomValue(-50, 50);
                auto r2 = GetRandomValue(-50, 50);
                particleDef.position.Set(mx + r1, my + r2);
                particleDef.flags = b2_viscousParticle | b2_colorMixingParticle;
                particleDef.velocity.Set(mouseVelX * 50, mouseVelY * 50);
                particleSystem->CreateParticle(particleDef);
            }
        }

        auto dt = GetFrameTime();
        world->Step(dt, // timestep
                    10, // velocity iterations
                    8   // position iterations
        );

        BeginDrawing();

        // draw gradient fluid particles to renderbuffer
        BeginTextureMode(fluidTarget);
        ClearBackground(BLANK);

        for (size_t i = 0; i < particleSystem->GetParticleCount(); i++) {
            auto pos = particleSystem->GetPositionBuffer()[i];
            auto col = particleSystem->GetColorBuffer()[i];
            // DrawCircleV({pos.x, pos.y}, 4.f, {col.r, col.g, col.b, col.a});s
            auto w = static_cast<float>(gradientTex.width);
            auto h = static_cast<float>(gradientTex.height);
            DrawTexturePro(gradientTex, {0, 0, w, h},
                           {pos.x, pos.y, radius * 8.f, radius * 8.f},
                           {radius * 4.f, radius * 4.f}, 0.f,
                           {col.r, col.g, col.b, col.a});
        }
        EndTextureMode();

        // draw background game objects to game renderbuffer
        BeginTextureMode(gameTarget);
        ClearBackground(BLACK);
        DrawTexture(backgroundTex, 0, 0, WHITE);
        EndTextureMode();

        // draw to main screen
        ClearBackground(BLACK);

        // draw background (game)
        DrawTextureRec(gameTarget.texture,
                       {0.f, 0.f, static_cast<float>(gameTarget.texture.width),
                        static_cast<float>(-gameTarget.texture.height)},
                       {0.f, 0.f}, WHITE);

        // draw fluid renderbuffer to screen while applying shader:
        // the particles are drawn as gradient circles, and the shader turns
        // them into pretty metaballs: https://john-wigg.dev/2DMetaballs/
        BeginShaderMode(shaders[SHADER_FLUID]);
        SetShaderValueTexture(shaders[SHADER_FLUID], gameTextureLoc,
                              gameTarget.texture); // for water distortion
        SetShaderValueTexture(shaders[SHADER_FLUID], noiseTextureLoc, noiseTex);
        Vector2 iResolution{static_cast<float>(gameTarget.texture.width),
                            static_cast<float>(gameTarget.texture.height)};
        SetShaderValue(shaders[SHADER_FLUID], iResolutionLoc, &iResolution,
                       SHADER_UNIFORM_VEC2);
        float iTime = GetTime();
        SetShaderValue(shaders[SHADER_FLUID], iTimeLoc, &iTime,
                       SHADER_UNIFORM_FLOAT);

        // draw fluid texture
        DrawTextureRec(fluidTarget.texture,
                       {0.f, 0.f, static_cast<float>(fluidTarget.texture.width),
                        static_cast<float>(-fluidTarget.texture.height)},
                       {0.f, 0.f}, WHITE);
        EndShaderMode();

        // draw entities on top of fluid
        for (auto& e : entities) {
            e.draw();
        }

        DrawFPS(10, 10);
        auto particlesText =
            std::to_string(particleSystem->GetParticleCount()) + " particles";
        DrawText(particlesText.c_str(), 10, 30, 20, WHITE);
        EndDrawing();
    }

    CloseWindow();
    delete world;
    return 0;
}
