#include <Box2D/Box2D.h>
#include <raylib.h>
#include <vector>

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

constexpr float LENGTH_UNIT = 128.f;

enum ShaderType {
    SHADER_FLUID,
    SHADER_MAX,
};

enum SpawnColor {
    COLOR_RAINBOW,
    COLOR_RASPBERRY,
    COLOR_HONEY,
    COLOR_OIL,
    COLOR_SLIME,
    COLOR_MILK,
    COLOR_LAVA,
};

Color colorFromType(SpawnColor col) {
    static float hue = 0.f;
    static Color raspberryColors[] = {
        {116, 7, 7, 255},
        {86, 14, 7, 255},
        {73, 8, 5, 255},
        {64, 3, 3, 255},
        {34, 0, 0, 255},
        {240, 29, 122, 255},
    };
    switch (col) {
    case COLOR_RAINBOW:
        hue += 0.5f;
        hue = fmod(hue, 360.f);
        return ColorFromHSV(hue, 1.f, 1.f);
    case COLOR_RASPBERRY: return raspberryColors[rand() % 6];
    case COLOR_HONEY:     return {255, 177, 0, 255};
    case COLOR_OIL:       return {170, 140, 40, 255};
    case COLOR_SLIME:     return {30, 200, 40, 255};
    case COLOR_MILK:      return {240, 240, 240, 255};
    case COLOR_LAVA:      return {255, 80, 20, 255};
    }
}

class Entity {
public:
    Entity(b2World* world, const b2BodyDef* def, bool persistent = false);

    constexpr b2Body* Body() const {
        return m_body;
    }

    inline void SetExtent(b2Vec2 extent) {
        m_extent = extent;
    }

    constexpr void SetColor(Color color) {
        m_color = color;
    }

    inline bool IsPersistent() const {
        return m_persistent;
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
    bool m_persistent = false;
};

Entity::Entity(b2World* world, const b2BodyDef* def, bool persistent) {
    m_body = world->CreateBody(def);
    m_persistent = persistent;
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
    std::vector<Entity> entities;

    // Define the ground body.
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(GetScreenWidth() / 2.f,
                               GetScreenHeight() * 0.8f);

    // Call the body factory which allocates memory for the ground body
    // from a pool and creates the ground box shape (also from a pool).
    // The body is also added to the world.
    Entity groundBody{world, &groundBodyDef, /*persistent=*/true};

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
    b2ParticleSystemDef particleSystemDef;
    particleSystemDef.dampingStrength = 0.2f;
    particleSystemDef.radius = 4.f;
    particleSystemDef.surfaceTensionNormalStrength = 0.4f;
    particleSystemDef.surfaceTensionPressureStrength = 0.4f;
    particleSystemDef.viscousStrength = 0.05f;
    // particleSystemDef.viscousStrength = 0.9f;
    particleSystemDef.colorMixingStrength = 0.05f;
    auto particleSystem = world->CreateParticleSystem(&particleSystemDef);

    // b2PolygonShape shape;
    // shape.SetAsBox(GetScreenWidth(), GetScreenHeight(), {0, 0}, 0);
    // b2ParticleGroupDef pd;
    // pd.shape = &shape;
    // pd.flags = b2_tensileParticle | b2_colorMixingParticle;
    // b2ParticleGroup* const group = particleSystem->CreateParticleGroup(pd);

    auto lastSpawnTime = 0.f;
    float prevMouseX{}, prevMouseY{};
    bool colorMixing = false, removingParticles = false;
    SpawnColor spawnColor = COLOR_RAINBOW;

    auto fluidTarget = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    auto gameTarget = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());

    while (!WindowShouldClose()) {
        auto dt = GetFrameTime();

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

        if (IsKeyPressed(KEY_M)) {
            colorMixing = !colorMixing;
        }

        if (removingParticles) {
            for (int i = 0; i < 4000 * dt; ++i)
                particleSystem->DestroyParticle(i);
            if (particleSystem->GetParticleCount() == 0) {
                removingParticles = false;
                particleSystem->SetGravityScale(1.f);
            }
        }

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) &&
            GetTime() - lastSpawnTime > 0.01f) {
            b2BodyDef bodyDef;
            bodyDef.type = b2_staticBody;
            bodyDef.position.Set(GetMouseX() + 5.f, GetMouseY() + 5.f);
            Entity box{world, &bodyDef};

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
                auto color = colorFromType(spawnColor);
                particleDef.color.Set(color.r, color.g, color.b, 255);
                auto r1 = GetRandomValue(-50, 50);
                auto r2 = GetRandomValue(-50, 50);
                particleDef.position.Set(mx + r1, my + r2);
                particleDef.flags = b2_viscousParticle;
                if (colorMixing) particleDef.flags |= b2_colorMixingParticle;
                particleDef.velocity.Set(mouseVelX * 50, mouseVelY * 50);
                particleSystem->CreateParticle(particleDef);
            }
        }

        world->Step(dt, // timestep
                    10, // velocity iterations
                    8   // position iterations
        );

        // destroy offscreen particles
        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();
        for (int32 i = particleSystem->GetParticleCount() - 1; i >= 0; --i) {
            b2Vec2 pos = particleSystem->GetPositionBuffer()[i];
            if (pos.x < 0 || pos.x > screenWidth || pos.y > screenHeight) {
                particleSystem->DestroyParticle(i);
            }
        }

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
            auto rad = particleSystem->GetRadius();
            DrawTexturePro(gradientTex, {0, 0, w, h},
                           {pos.x, pos.y, rad * 8.f, rad * 8.f},
                           {rad * 4.f, rad * 4.f}, 0.f,
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
        DrawText(TextFormat("Particles: %d", particleSystem->GetParticleCount()), 10, 30, 20, WHITE);
        DrawText(TextFormat("Color (M)ixing: %s", colorMixing ? "On" : "Off"), 10, 50, 20, WHITE);
        DrawText("(R)eset", 10, 70, 20, WHITE);
#ifdef NDEBUG
        DrawText("(Release Build)", 10, 90, 20, WHITE);
#else
        DrawText("(Debug Build)", 10, 90, 20, WHITE);
#endif

        // GUI
        {
            float radius = particleSystem->GetRadius();
            float damping = particleSystem->GetDamping();
            float density = particleSystem->GetDensity();
            float viscous = particleSystem->GetViscous();
            float gravity = particleSystem->GetGravityScale();
            float colmix = particleSystem->GetColorMixingStrength();
            float normal = particleSystem->GetSurfaceTensionNormalStrength();
            float pressure = particleSystem->GetSurfaceTensionPressureStrength();
            if (GuiSlider({10, 110, 100, 20}, nullptr, "Radius", &radius, 0.3f, 10.f))
                particleSystem->SetRadius(radius);
            if (GuiSlider({10, 130, 100, 20}, nullptr, "Damping", &damping, 0.05f, 2.f))
                particleSystem->SetDamping(damping);
            if (GuiSlider({10, 150, 100, 20}, nullptr, "Density", &density, 0.05f, 2.f))
                particleSystem->SetDensity(density);
            if (GuiSlider({10, 170, 100, 20}, nullptr, "Viscous", &viscous, 0.f, 2.f))
                particleSystem->SetViscous(viscous);
            if (GuiSlider({10, 190, 100, 20}, nullptr, "Gravity", &gravity, 0.f, 5.f))
                particleSystem->SetGravityScale(gravity);
            if (GuiSlider({10, 210, 100, 20}, nullptr, "Colormix", &colmix, 0.f, 1.f))
                particleSystem->SetColorMixingStrength(colmix);
            if (GuiSlider({10, 250, 100, 20}, nullptr, "Surf. Normal", &normal, 0.f, 2.f))
                particleSystem->SetSurfaceTensionNormalStrength(normal);
            if (GuiSlider({10, 230, 100, 20}, nullptr, "Surf. Pressure", &pressure, 0.f, 2.f))
                particleSystem->SetSurfaceTensionPressureStrength(pressure);

            // presets
            if (GuiButton({10, 270, 100, 20}, "Honey")) {
                particleSystem->SetRadius(3.2f);
                particleSystem->SetDamping(0.75f);
                particleSystem->SetDensity(1.2f);
                particleSystem->SetViscous(2.f);
                particleSystem->SetGravityScale(0.6f);
                particleSystem->SetColorMixingStrength(0.0015f);
                particleSystem->SetSurfaceTensionNormalStrength(1.2f);
                particleSystem->SetSurfaceTensionPressureStrength(0.9f);
                colorMixing = true;
                spawnColor = COLOR_HONEY;
            }
            if (GuiButton({10, 290, 100, 20}, "Raspberry Jam")) {
                particleSystem->SetRadius(3.3f);
                particleSystem->SetDamping(0.85f);
                particleSystem->SetDensity(1.35f);
                particleSystem->SetViscous(2.0f);
                particleSystem->SetGravityScale(1.0f);
                particleSystem->SetColorMixingStrength(0.01f);
                particleSystem->SetSurfaceTensionNormalStrength(1.4f);
                particleSystem->SetSurfaceTensionPressureStrength(1.0f);
                colorMixing = false;
                spawnColor = COLOR_RASPBERRY;
            }
            if (GuiButton({10, 310, 100, 20}, "Olive Oil")) {
                particleSystem->SetRadius(2.4f);
                particleSystem->SetDamping(0.55f);
                particleSystem->SetDensity(0.9f);
                particleSystem->SetViscous(0.8f);
                particleSystem->SetGravityScale(1.0f);
                particleSystem->SetColorMixingStrength(0.002f);
                particleSystem->SetSurfaceTensionNormalStrength(0.9f);
                particleSystem->SetSurfaceTensionPressureStrength(0.5f);
                colorMixing = true;
                spawnColor = COLOR_OIL;
            }
            if (GuiButton({10, 330, 100, 20}, "Slime")) {
                particleSystem->SetRadius(3.6f);
                particleSystem->SetDamping(0.9f);
                particleSystem->SetDensity(1.4f);
                particleSystem->SetViscous(2.0f);
                particleSystem->SetGravityScale(0.7f);
                particleSystem->SetColorMixingStrength(0.008f);
                particleSystem->SetSurfaceTensionNormalStrength(1.6f);
                particleSystem->SetSurfaceTensionPressureStrength(1.3f);
                colorMixing = true;
                spawnColor = COLOR_SLIME;
            }
            if (GuiButton({10, 350, 100, 20}, "Milk")) {
                particleSystem->SetRadius(2.1f);
                particleSystem->SetDamping(0.30f);
                particleSystem->SetDensity(0.95f);
                particleSystem->SetViscous(0.15f);
                particleSystem->SetGravityScale(1.0f);
                particleSystem->SetColorMixingStrength(0.1f);
                particleSystem->SetSurfaceTensionNormalStrength(0.45f);
                particleSystem->SetSurfaceTensionPressureStrength(0.35f);
                colorMixing = true;
                spawnColor = COLOR_MILK;
            }
            if (GuiButton({10, 370, 100, 20}, "Lava")) {
                particleSystem->SetRadius(3.0f);
                particleSystem->SetDamping(0.8f);
                particleSystem->SetDensity(1.8f);
                particleSystem->SetViscous(2.0f);
                particleSystem->SetGravityScale(0.95f);
                particleSystem->SetSurfaceTensionNormalStrength(1.3f);
                particleSystem->SetSurfaceTensionPressureStrength(1.1f);
                colorMixing = false;
                spawnColor = COLOR_LAVA;
            }
            if (GuiButton({10, 390, 100, 20}, "Reset") || IsKeyPressed(KEY_R)) {
                std::erase_if(entities, [&](const Entity& e) {
                    if (!e.IsPersistent()) {
                        world->DestroyBody(e.Body());
                        return true;
                    }
                    return false;
                });
                particleSystem->SetRadius(4.f);
                particleSystem->SetDensity(1.f);
                particleSystem->SetDamping(0.2f);
                particleSystem->SetViscous(0.05f);
                particleSystem->SetGravityScale(0.f);
                particleSystem->SetSurfaceTensionNormalStrength(0.4f);
                particleSystem->SetSurfaceTensionPressureStrength(0.4f);
                particleSystem->SetColorMixingStrength(0.05f);
                removingParticles = true;
                spawnColor = COLOR_RAINBOW;
            }
        }

        EndDrawing();
    }

    CloseWindow();
    delete world;
    return 0;
}
