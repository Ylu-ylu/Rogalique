# Конспект: связи и зависимости в проекте XYZRoguelike

## 1. Слои архитектуры

```
Слой 3: GameMain (точка входа)
Слой 2: Игровые объекты и компоненты (XYZRoguelike/)
Слой 1: Движок — компоненты и системы (Engine/)
```

Правило зависимости: **вышестоящий слой знает о нижестоящем, но не наоборот**.
Движок (Engine/) НЕ включает заголовки игры (XYZRoguelike/) — за одним исключением: `FollowComponent` играет клип `"attack"` по имени (строковая конвенция, не include).

---

## 2. Синглтоны движка (глобальный доступ)

| Синглтон | Назначение | Кто вызывает (из игры) |
|---|---|---|
| `Engine` | Главный цикл, пауза | `GameMain` (Run), `PauseComponent` (IsPaused) |
| `GameWorld` | Контейнер объектов, отложенные действия | Все (CreateGameObject, DestroyGameObject, EnqueueLateAction, Clear) |
| `RenderSystem` | Окно + рисование | `SpriteRendererComponent`, `HealthBarComponent`, `GameHUDComponent`, `PauseComponent`, `PlayerDeathComponent`, `ExitLabelComponent`, `HealingWallComponent`, `BossAbilitiesComponent`, `InventoryComponent` |
| `ResourceSystem` | Кэш текстур/атласов/звуков | `GameMain` (загрузка), `Player`, `AI`, `DeveloperLevel`, `FollowComponent`, `EnemyDeathComponent`, `PlayerDeathComponent`, `PlayerAttackComponent`, `GameHUDComponent` |
| `PhysicsSystem` | AABB-коллизии, триггеры | `SpriteColliderComponent` (Subscribe/Unsubscribe), `GameWorld` (FixedUpdate) |
| `LoggerRegistry` | Логи в консоль+файл | Все через макрос `LOG_INFO/WARN/ERROR` |

---

## 3. Компонентная модель (Engine/)

### 3.1 Наследование

```
Component (абстрактный)
├── TransformComponent      — позиция/масштаб/поворот, иерархия parent/child
├── SpriteRendererComponent — текстура, размер, FlipX/Y, SetColor
├── ColliderComponent (абстрактный)
│   └── SpriteColliderComponent — bounds по спрайту или SetSize(w,h)
│                                   SetEnabled(bool), SetTrigger(bool)
│                                   SubscribeCollision/TriggerEnter/TriggerExit
├── RigidbodyComponent      — SetKinematic, SetLinearVelocity
├── MovementComponent       — движение по InputComponent (WASD)
├── FollowComponent         — преследование цели + атака + звук + анимация
├── InputComponent          — опрос клавиатуры (оси X/Y)
├── CameraComponent         — sf::View, следует за трансформом
├── AudioComponent          — sf::Sound (Play/Stop/SetLoop/SetVolume)
├── StatsComponent          — HP/Armor (TakeDamage/Heal/RepairArmor)
├── AttackComponent         — урон цели (Attack(target))
├── SpriteDirectionComponent — поворот спрайта (FlipX) по смещению
└── SpriteMovementAnimationComponent — именованные клипы (Play/AddAnimation)
```

### 3.2 Внутренние зависимости компонентов движка

| Компонент | Зависит от (GetComponent) | Зачем |
|---|---|---|
| `SpriteColliderComponent` | `SpriteRendererComponent` | размер bounds от спрайта |
| `MovementComponent` | `InputComponent`, `StatsComponent` | оси ввода, стоп при смерти |
| `FollowComponent` | `TransformComponent`, `StatsComponent` (свой + цели), `AttackComponent`, `SpriteMovementAnimationComponent`, `AudioComponent` | движение, атака, анимация, звук |
| `CameraComponent` | `TransformComponent` | позиция камеры |
| `SpriteDirectionComponent` | `TransformComponent`, `SpriteRendererComponent` | дельта позиции → FlipX |
| `SpriteMovementAnimationComponent` | `SpriteRendererComponent` | смена текстуры кадра |
| `AttackComponent` | `StatsComponent` (цели, через параметр) | TakeDamage |

---

## 4. Игровые объекты-обёртки (XYZRoguelike/)

### 4.1 Иерархия наследования

```
Scene (интерфейс: Start/Restart/Stop)
└── DeveloperLevel

AI (базовый враг)
└── Creeper (переопределяет Clone)
```

### 4.2 Состав GameObject'ов (какие компоненты на кого вешаются)

**Player** (создаётся в `Player.cpp`):
```
GameObject("Player")
├── TransformComponent          (позиция при создании)
├── SpriteRendererComponent     (атлас "Player", Man.png)
├── CameraComponent             (SetWindow, SetBaseResolution)
├── InputComponent              (WASD)
├── MovementComponent           (SetSpeed 400)
├── SpriteDirectionComponent    (поворот по дельте)
├── SpriteMovementAnimationComponent (walk 0–7, idle 0, attack 8–12, death 13)
├── RigidbodyComponent          (kinematic=false)
├── SpriteColliderComponent     (SetSize 122×128)
├── StatsComponent              (100 HP, 50 Armor)
├── AttackComponent             (10 урона)
├── HealthBarComponent × 2      (красная HP +100, жёлтая Armor +88)
├── PlayerLivesComponent        (3 сердца)
└── PauseComponent              (оверлей PAUSED)
```

+ добавляется в `DeveloperLevel::Start()`:
```
├── PlayerDeathComponent        (смерть → Game Over / респавн)
├── PlayerAttackComponent       (Space → веерная атака)
└── InventoryComponent          (зелья 1/2/3)
```

**AI** (базовый враг, `AI.cpp`):
```
GameObject("ai_N")
├── TransformComponent
├── SpriteRendererComponent     (атлас "ai", Wizard.png)
├── FollowComponent             (SetTarget=player, SetSpeed=120)
├── SpriteDirectionComponent
├── SpriteMovementAnimationComponent (walk 0–6, idle 0, attack 7–11, death 12)
├── RigidbodyComponent          (kinematic=false)
├── SpriteColliderComponent
├── StatsComponent              (100 HP, 0 Armor)
├── AttackComponent             (10 урона)
├── HealthBarComponent          (+80, 70×8)
└── EnemyDeathComponent         (HP=0 → стоп → 2с → исчезновение)
```

**Creeper** (наследует AI, спавнится через `CreeperSpawner`):
```
= AI (все компоненты те же) + SetColor(Red)
```

**Boss** (создаётся через `CreeperSpawner::SpawnBoss` → `SetupBoss`):
```
= AI (базовый набор)
+ BossDeathComponent            (смерть → OnBossDied → OpenExit)
+ BossAbilitiesComponent        (AOE + ярость)
  затем SetupBoss():
  ├── texture → "Creeper" атлас
  ├── color → фиолетовый (170, 0, 255)
  ├── SetPixelSize → 128px
  ├── клипы → walk 0.2с, attack, death
  ├── SetSpeed → 60 (×2 медленнее криперов)
  └── SetSize → 122×128 (коллайдер как у игрока)
```

**Wall** (`Wall.cpp`):
```
GameObject("Wall")
├── TransformComponent
├── SpriteRendererComponent     (атлас "level_walls", 128×128)
├── RigidbodyComponent          (kinematic=true)
└── SpriteColliderComponent
```

**Floor** (`Floor.cpp`):
```
GameObject("Floor")
├── TransformComponent
└── SpriteRendererComponent     (атлас "level_floors", 128×128)
```

**Exit** (`DeveloperLevel::CreateExitTrigger`):
```
GameObject("LevelExit")
├── TransformComponent          (позиция на рамке)
├── SpriteRendererComponent     (level_floors[0], серый/зелёный)
├── RigidbodyComponent          (kinematic=true)
├── SpriteColliderComponent     (SetTrigger=true, SetEnabled=false→true)
├── AudioComponent              (Transition.wav)
└── ExitLabelComponent          ("Exit", красный→зелёный)
```

**HUD** (`DeveloperLevel::Start`):
```
GameObject("GameHUD")
└── GameHUDComponent            (текст + сердца + зелья, view-swap)
```

**Music** (`Music.cpp`):
```
GameObject("Music: ...")
└── AudioComponent              (neon-gaming.wav, loop, 35%)
```

---

## 5. Игровые компоненты (XYZRoguelike/) — зависимости

| Компонент | На каком объекте | Зависит от | Взаимодействует с |
|---|---|---|---|
| `PlayerAttackComponent` | Player | `TransformComponent`, `AttackComponent`, `StatsComponent`, `SpriteMovementAnimationComponent`, `AudioComponent` | `CreeperSpawner` (FindEnemiesInRange) |
| `PlayerDeathComponent` | Player | `StatsComponent`, `AudioComponent`, `SpriteMovementAnimationComponent` | `PlayerLivesComponent` (HasLives), `DeveloperLevel` (Restart) |
| `PlayerLivesComponent` | Player | — | `PlayerDeathComponent` (LoseLife/HasLives), `GameHUDComponent` (GetLives) |
| `InventoryComponent` | Player | `StatsComponent`, `AttackComponent`, `MovementComponent`, `TransformComponent` | `CreeperSpawner` (FindEnemiesInRange — волна), `GameHUDComponent` (счётчики) |
| `HealthBarComponent` | Player(×2), AI, Creeper, Boss | `TransformComponent`, `StatsComponent` | — |
| `GameHUDComponent` | HUD object | `StatsComponent` (игрока), `PlayerLivesComponent`, `InventoryComponent` | `DeveloperLevel` (IsExitOpened), `ResourceSystem` (Heart.png) |
| `PauseComponent` | Player | — | `Engine` (IsPaused) |
| `EnemyDeathComponent` | AI, Creeper, Boss | `StatsComponent`, `AudioComponent`, `SpriteColliderComponent`, `SpriteMovementAnimationComponent` | — |
| `BossDeathComponent` | Boss | `StatsComponent` | `DeveloperLevel` (OnBossDied) |
| `BossAbilitiesComponent` | Boss | `TransformComponent`, `StatsComponent` (свой+игрока), `FollowComponent`, `SpriteRendererComponent` | `StatsComponent` игрока (TakeDamage) |
| `ExitLabelComponent` | Exit | `TransformComponent` | `DeveloperLevel` (SetOpened) |
| `HealingWallComponent` | Wall (избранные) | `TransformComponent` (свой+игрока), `StatsComponent` (игрока), `SpriteRendererComponent` | — |

---

## 6. Ключевые паттерны взаимодействия

### 6.1 Цепочка перехода уровня

```
Player касается Exit триггера
  → SpriteColliderComponent::OnTriggerEnter (колбэк)
    → GameWorld::EnqueueLateAction(LoadNextLevel)   ← ОТЛОЖЕННО
      → (физика кадра завершается)
      → LateUpdate: выполняет LoadNextLevel
        → DeveloperLevel::Restart()
          → Stop(): очистка walls/floors/enemies/music/exit/bosses + GameWorld::Clear()
          → Start(): новый лабиринт + игрок + враги + боссы + выход + HUD + музыка
            → InventoryComponent::ResetStocks() (зелья → 5/5/5)
            → Transition.wav играет (если currentLevel > 1)
```

### 6.2 Цепочка смерти игрока

```
HP ≤ 0 (StatsComponent)
  → PlayerDeathComponent::Update
    ├─ PlayerLivesComponent::HasLives() == true
    │    → LoseLife() + SetCurrentHealth(max) + SetCurrentArmor(max)
    │    → телепорт в центр (896, 896) + звук
    │    → continue playing
    └─ PlayerLivesComponent::HasLives() == false
         → Play("death") + звук + таймер 2с
         → Game Over экран (оверлей + view-swap)
         → R → EnqueueLateAction(Restart)
         → Esc → window.close()
```

### 6.3 Цепочка смерти босса

```
HP ≤ 0 (StatsComponent)
  → EnemyDeathComponent: коллайдер off, Play("death"), звук, 2с → исчезновение
  → BossDeathComponent: EnqueueLateAction
    → DeveloperLevel::OnBossDied()
      → проверка: ВСЕ боссы мертвы?
        ├─ нет → return (ждём остальных)
        └─ да → OpenExit()
                 ├─ SetColor(зелёный)
                 ├─ SetEnabled(true) (коллайдер)
                 ├─ ExitLabel::SetOpened(true) (зелёный "Exit")
                 └─ LOG_INFO("Boss defeated - exit opened")
```

### 6.4 Цепочка атаки игрока

```
Space нажат + кулдаун 0.5с прошёл
  → PlayerAttackComponent::Update
    → isAttackActive = true, Play("attack"), звук Attack.wav
    → таймер 0.22с (damageMoment)
      → TryDealDamage()
        → CreeperSpawner::FindEnemiesInRange(playerPos, 180)
          → ВСЕ враги в радиусе
            → AttackComponent::Attack(enemy)
              → enemy->StatsComponent::TakeDamage(10)
    → таймер 0.4с (attackDuration) → isAttackActive = false
```

### 6.5 Зелья волны

```
Клавиша 3 нажата
  → InventoryComponent::Update
    → UseWavePotion()
      → CreeperSpawner::FindEnemiesInRange(playerPos, 250)
        → все враги в радиусе
          → StatsComponent::TakeDamage(30)
      → waveTimer = 0.5с (визуал: голубое кольцо растёт)
      → wavePotions--
```

---

## 7. Схема владения памятью

| Владелец | Что владеет | Модель |
|---|---|---|
| `GameWorld` | все `GameObject*` | raw new/delete (Clear) |
| `GameObject` | все `Component*` | raw new/delete (~GameObject) |
| `DeveloperLevel` | `walls[]`, `floors[]` | `unique_ptr<Wall>`, `unique_ptr<Floor>` |
| `DeveloperLevel` | `player`, `ai` | `shared_ptr` |
| `DeveloperLevel` | `creeperSpawner`, `music` | `unique_ptr` |
| `CreeperSpawner` | `enemies[]` | `vector<unique_ptr<AI>>` |
| `CreeperSpawner` | `externalEnemies[]` | `vector<AI*>` (невладеющие) |
| `SpriteRendererComponent` | `sf::Sprite*` | raw new/delete |
| `AudioComponent` | `sf::Sound*` | raw new/delete |
| `ResourceSystem` | текстуры, звуки | raw new (Clear) |

---

## 8. Карта include-зависимостей (игра → движок)

```
GameMain.cpp
  → DeveloperLevel.h, Engine.h, ResourceSystem.h, Logger.h

DeveloperLevel.cpp
  → Player.h, AI.h, CreeperSpawner.h, MazeGenerator.h, Wall.h, Floor.h, Music.h
  → PlayerDeathComponent.h, PlayerAttackComponent.h, BossDeathComponent.h
  → BossAbilitiesComponent.h, InventoryComponent.h, HealingWallComponent.h
  → GameHUDComponent.h, ExitLabelComponent.h, GameConstants.h, GameSettings.h
  → Engine: ResourceSystem, SpriteRenderer, Rigidbody, SpriteCollider,
            FollowComponent, AudioComponent, TransformComponent, Logger

Player.cpp
  → Engine: ResourceSystem, Rigidbody, SpriteCollider, MovementComponent,
            SpriteDirection, SpriteMovementAnimation, StatsComponent, AttackComponent
  → HealthBarComponent.h, PlayerLivesComponent.h, PauseComponent.h

AI.cpp
  → Engine: ResourceSystem, Rigidbody, SpriteCollider, FollowComponent,
            SpriteDirection, SpriteMovementAnimation, StatsComponent, AttackComponent
  → HealthBarComponent.h, EnemyDeathComponent.h

Creeper.cpp
  → AI.h, GameConstants.h
  → Engine: ResourceSystem, Rigidbody, SpriteCollider, FollowComponent,
            StatsComponent, AttackComponent, GameWorld

CreeperSpawner.cpp
  → AI.h, Creeper.h
  → Engine: StatsComponent, TransformComponent, Logger

MazeGenerator.cpp
  → DeveloperLevel.h (указатель на уровень — добавляет стены/полы)
```

---

## 9. Поток данных (что откуда берётся)

| Данные | Источник | Потребители |
|---|---|---|
| Позиция игрока | `TransformComponent` (через `GetWorldPosition()`) | FollowComponent, BossAbilities, HealingWall, PlayerAttack, Inventory |
| HP/Armor | `StatsComponent` (игрока) | HealthBar, GameHUD, PlayerDeath, Inventory, HealingWall |
| HP врагов | `StatsComponent` (врага) | HealthBar, EnemyDeath, BossDeath, GameHUD (нет) |
| Список врагов | `CreeperSpawner::enemies + externalEnemies` | PlayerAttack (FindEnemiesInRange), Inventory (FindEnemiesInRange) |
| Состояние выхода | `DeveloperLevel::exitOpened` | GameHUD (IsExitOpened), ExitLabel |
| Уровень | `DeveloperLevel::currentLevel` | GameHUD, CreeperSpawner (масштаб), MazeGenerator (размер = const) |
| Сердца | `PlayerLivesComponent::lives` | GameHUD, PlayerDeath |
| Зелья | `InventoryComponent::*Potions` | GameHUD |
| Кадры анимации | `SpriteMovementAnimationComponent` | SpriteRenderer (SetTexture) |
| Текстуры | `ResourceSystem` | Все SpriteRenderer |
| Звуки | `ResourceSystem` | Все AudioComponent |
