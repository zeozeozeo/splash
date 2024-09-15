#pragma once

// meters = pixels / PIXELS_PER_METER
constexpr float PIXELS_PER_METER = 32.f;

// cameraZoom = screenWidth / WIDTH2ZOOM
constexpr float WIDTH2ZOOM = 1280.f / PIXELS_PER_METER;

// cameraZoom = screenHeight / HEIGHT2ZOOM
constexpr float HEIGHT2ZOOM = 720.f / PIXELS_PER_METER;
