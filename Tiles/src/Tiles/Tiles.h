#pragma once

// =============================================================================
// CLEAN ARCHITECTURE LAYERS (New)
// =============================================================================

// Domain Layer - Core business entities (no dependencies)
#include "Domain/Domain.h"

// Application Layer - Use cases and services
#include "Application/Application.h"

// Infrastructure Layer - External implementations
#include "Infrastructure/Infrastructure.h"

// =============================================================================
// LEGACY COMPONENTS (To be migrated)
// =============================================================================

// Core (Presentation layer)
#include "Core/Base.h"
#include "Core/Application.h"
#include "Core/Layer.h"
#include "Core/Input.h"
#include "Core/KeyCode.h"
#include "Core/Log.h"
#include "Core/Assert.h"

// Graphics (Infrastructure layer - to be migrated)
#include "Graphics/Renderer2D.h"
#include "Graphics/Texture.h"
#include "Graphics/TextureAtlas.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/Cameras/OrthographicCamera.h"

// Utils
#include "Utils/Timer.h"
