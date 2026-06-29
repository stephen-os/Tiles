# Tiles Engine - Clean Architecture

## Overview

This document describes the clean architecture refactoring of the Tiles engine. The goal is to separate concerns, increase modularity, reduce tight coupling, and improve long-term maintainability.

## Architecture Layers

```
┌─────────────────────────────────────────────────────────────┐
│                     PRESENTATION                             │
│  (Application.h, Layer.h, EntryPoint.h)                     │
│  Framework entry points and UI layer base classes           │
├─────────────────────────────────────────────────────────────┤
│                     APPLICATION                              │
│  (Services, Commands, Interfaces)                           │
│  Use cases, business logic orchestration                    │
├─────────────────────────────────────────────────────────────┤
│                       DOMAIN                                 │
│  (Entities, ValueObjects, Interfaces)                       │
│  Core business entities, no external dependencies           │
├─────────────────────────────────────────────────────────────┤
│                   INFRASTRUCTURE                             │
│  (Rendering/OpenGL, Persistence, Platform, Logging)         │
│  External concerns: graphics, file I/O, platform APIs       │
└─────────────────────────────────────────────────────────────┘
```

## Dependency Rule

Dependencies point inward:
- **Presentation** → Application → Domain
- **Infrastructure** → Domain (implements interfaces)
- **Domain** has NO external dependencies

## Folder Structure

```
Tiles/src/Tiles/
├── Domain/                          # Core business logic (innermost layer)
│   ├── Entities/
│   │   ├── TileGrid.h/cpp          # 2D grid of tiles
│   │   └── TileProject.h/cpp       # Project aggregate root
│   ├── ValueObjects/
│   │   ├── Position.h              # Immutable position
│   │   └── TileData.h              # Immutable tile data
│   └── Interfaces/
│       └── IProjectRepository.h    # Repository interface
│
├── Application/                     # Use cases and services
│   ├── Commands/
│   │   └── ICommand.h              # Command interface for undo/redo
│   ├── Services/
│   │   ├── CommandHistory.h/cpp    # Undo/redo management
│   │   ├── ProjectService.h/cpp    # Project operations
│   │   ├── PaintingService.h       # Painting operations
│   │   └── EditorContext.h         # Service coordinator
│   └── Interfaces/
│       └── IRenderer.h             # Rendering abstraction
│
├── Infrastructure/                  # External implementations
│   ├── Rendering/
│   │   ├── Interfaces/             # Rendering abstractions
│   │   └── OpenGL/                 # OpenGL implementations
│   ├── Persistence/
│   │   └── JsonProjectRepository.h # JSON file persistence
│   ├── Platform/                   # GLFW, Input handling
│   └── Logging/
│       └── Log.h/cpp               # spdlog wrapper
│
├── Presentation/                    # Framework layer
│   ├── Application.h/cpp           # Main application class
│   └── Layer.h/cpp                 # UI layer base
│
├── Core/                           # [LEGACY - to be migrated]
├── Graphics/                       # [LEGACY - migrate to Infrastructure]
├── Utils/                          # [LEGACY - migrate appropriately]
│
├── Tiles.h                         # Public API header
└── EntryPoint.h                    # Application entry point
```

## Key Architectural Improvements

### 1. God Object Elimination

**Before:**
```cpp
class Context {
    // Camera management
    // Layer management
    // Painting operations
    // Command execution
    // Project management
    // Project history
    // 100+ lines of mixed concerns
};
```

**After:**
```cpp
class EditorContext {
    CommandHistory m_CommandHistory;      // Single responsibility
    ProjectService m_ProjectService;      // Single responsibility
    PaintingService m_PaintingService;    // Single responsibility
    std::shared_ptr<ICamera> m_Camera;    // Injected dependency
};
```

### 2. Dependency Injection

**Before:**
```cpp
class Panel {
    std::shared_ptr<Context> m_Context;  // Hard dependency on concrete type
};
```

**After:**
```cpp
class Panel {
    EditorContext& m_Context;             // Reference to abstraction
    // Or inject specific services needed
};
```

### 3. Repository Pattern

**Before:**
```cpp
class Project {
    nlohmann::json ToJSON() const;        // Domain knows about JSON
    static Project FromJSON(...);         // Serialization in domain
};
```

**After:**
```cpp
// Domain layer - pure business logic
class TileProject { /* no serialization knowledge */ };

// Infrastructure layer - persistence implementation
class JsonProjectRepository : IProjectRepository {
    RepositoryResult<void> Save(const TileProject&, const path&);
    RepositoryResult<TileProject> Load(const path&);
};
```

### 4. Command Pattern Improvement

**Before:**
```cpp
class Command {
    void Execute() {}   // Empty default
    void Undo() {}      // Empty default
};
```

**After:**
```cpp
class ICommand {
    virtual void Execute() = 0;           // Must implement
    virtual void Undo() = 0;              // Must implement
    virtual std::string GetDescription() const = 0;
    virtual bool CanMergeWith(const ICommand&) const;
};
```

### 5. Value Objects

**Before:**
```cpp
// Mutable data scattered everywhere
tile.AtlasIndex = 5;
tile.TileIndex = 10;
```

**After:**
```cpp
// Immutable value objects
struct TileData {
    const uint32_t AtlasIndex;
    const uint32_t TileIndex;
    bool operator==(const TileData&) const;
    static TileData Empty();
};
```

### 6. Interface Segregation

**Before:**
```cpp
class Renderer2D {
    static void DrawQuad(...);
    static void DrawCircle(...);
    static void DrawLine(...);
    // 50+ static methods
};
```

**After:**
```cpp
class IRenderer {
    virtual void DrawQuad(...) = 0;
    virtual void DrawTexturedQuad(...) = 0;
    virtual void DrawLine(...) = 0;
    // Focused interface
};

class ICamera {
    virtual glm::mat4 GetViewProjectionMatrix() const = 0;
    // Camera-specific interface
};
```

## Migration Path

### Phase 1: Domain Layer (Complete)
- [x] Create TileData value object
- [x] Create Position value object
- [x] Create TileGrid entity
- [x] Create TileProject aggregate root
- [x] Create IProjectRepository interface

### Phase 2: Application Layer (Complete)
- [x] Create ICommand interface
- [x] Create CommandHistory service
- [x] Create ProjectService
- [x] Create PaintingService
- [x] Create EditorContext coordinator
- [x] Create IRenderer interface

### Phase 3: Infrastructure Layer (Partial)
- [x] Create folder structure
- [x] Create JsonProjectRepository header
- [ ] Migrate OpenGL rendering code
- [ ] Migrate GLFW platform code
- [ ] Implement JsonProjectRepository

### Phase 4: Integration (Pending)
- [ ] Update existing code to use new architecture
- [ ] Deprecate old Context class
- [ ] Update TilesEditor to use services
- [ ] Remove legacy code

## Benefits

1. **Testability**: Services can be unit tested with mock dependencies
2. **Flexibility**: Swap implementations (e.g., JSON → SQLite)
3. **Maintainability**: Changes isolated to specific layers
4. **Scalability**: Add features without modifying existing code
5. **Clarity**: Clear separation of what vs. how

## Usage Example

```cpp
// Create infrastructure
auto repository = std::make_shared<JsonProjectRepository>();
auto camera = std::make_shared<OrthographicCamera>();

// Create application context
auto context = EditorContext::Create(repository, camera);

// Use services
context->GetProjectService().CreateNew(32, 32, "My Map");
context->GetPaintingService().SetTool(PaintingTool::Brush);
context->GetPaintingService().SetBrush(TileData{1, 5});

// Paint with undo support
auto* project = context->GetProject();
context->GetPaintingService().Paint(*project, Position{10, 10});

// Undo
context->Undo();
```
