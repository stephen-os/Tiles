#pragma once

// Application Layer - Use cases and services
// Depends only on Domain layer

#include "Commands/ICommand.h"
#include "Commands/PaintCommand.h"
#include "Services/CommandHistory.h"
#include "Services/ProjectService.h"
#include "Services/PaintingService.h"
#include "Services/EditorContext.h"
#include "Interfaces/IRenderer.h"
