# Полный конспект проекта XYZRoguelike для джуна

## Этап 1: Точка входа — `GameMain.cpp`

### Что происходит при запуске

```cpp
int main() {
    // 1. Создание окна
    auto *window = new sf::RenderWindow(sf::VideoMode(1280, 720), "XYZRoguelike");

    // 2. Регистрация окна в системе рендера
    XYZEngine::RenderSystem::Instance()->SetMainWindow(window);

    // 3. Загрузка ресурсов
    ResourceSystem::Instance()->LoadTextureMap("Player", "Man.png", {64,48}, 14);
    ResourceSystem::Instance()->LoadTextureMap("ai", "Wizard.png", {64,53}, 13);
    ResourceSystem::Instance()->LoadTextureMap("Creeper", "Creeper.png", {64,54}, 13);
    ResourceSystem::Instance()->LoadSound("music", "neon-gaming.wav");
    ResourceSystem::Instance()->LoadSound("Attack", "Attack.wav");
    // ... ещё звуки

    // 4. Создание и запуск уровня
    auto developerLevel = std::make_shared<DeveloperLevel>();
    developerLevel->Start();

    // 5. Главный цикл
    XYZEngine::Engine::Instance()->Run();
}
```

### Расшифровка каждого шага

**`sf::RenderWindow`** — окно SFML. Это "холст", на который рисуется вся игра. 1280×720 — разрешение.

**`RenderSystem::SetMainWindow(window)`** — регистрирует окно в синглтоне. Теперь любой компонент может рисовать: `RenderSystem::Instance()->Render(спрайт)` — без прямого доступа к окну.

**`ResourceSystem::LoadTextureMap(имя, путь, размер_кадра, количество)`** — загружает спрайт-атлас и нарезает на кадры:
- `"Player"` — ключ, по которому потом получаем кадры
- `"Man.png"` — файл с атласом (896×48 пикселей)
- `{64, 48}` — размер одного кадра в атласе
- `14` — всего кадров

**`DeveloperLevel::Start()`** — строит уровень (лабиринт, игрока, врагов, боссов). Подробнее на этапе 4.

**`Engine::Run()`** — запускает бесконечный цикл кадров. Подробно на этапе 2.

---

## Этап 2: Главный цикл — `Engine/Engine.cpp`

### Метод `Run()` — детально

```cpp
void Engine::Run() {
    sf::Clock gameClock;  // таймер для deltaTime

    while (window.isOpen()) {
        float deltaTime = gameClock.restart().asSeconds();

        // === 1. Обработка событий окна ===
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // === 2. Переключение паузы по Esc ===
        bool escPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Escape);
        if (escPressed && !escPressedLastFrame)
            isPaused = !isPaused;       // toggle
        escPressedLastFrame = escPressed;

        // === 3. Очистка экрана ===
        window.clear();

        // === 4-7. Игровые фазы ===
        if (!isPaused) {
            GameWorld::Instance()->Update(deltaTime);      // 4. Логика
            GameWorld::Instance()->FixedUpdate(deltaTime); // 5. Физика
        }
        GameWorld::Instance()->Render();                    // 6. Рисование
        if (!isPaused) {
            GameWorld::Instance()->LateUpdate();            // 7. Отложенное
        }

        // === 8. Показ кадра ===
        window.display();
    }
}
```

### Расшифровка фаз

| Фаза | Что делает | Кто вызывает | Когда |
|---|---|---|---|
| `Update(dt)` | Вызывает `Update()` у ВСЕХ компонентов ВСЕХ объектов | `GameWorld` | Каждый кадр |
| `FixedUpdate(dt)` | Вызывает `PhysicsSystem::Update()` | `GameWorld` | По таймеру (0.02с) |
| `Render()` | Вызывает `Render()` у всех компонентов | `GameWorld` | Каждый кадр, даже на паузе |
| `LateUpdate()` | Выполняет очередь `lateActions` + удаляет помеченные объекты | `GameWorld` | Каждый кадр, после Render |

**`deltaTime`** — время с прошлого кадра (секунды). Умножение на dt делает движение независимым от FPS: `transform->MoveBy(speed * deltaTime * direction)`.

**Почему пауза не останавливает Render:** игрок видит застывший игровой кадр. Поверх него `PauseComponent` рисует оверлей "PAUSED".

**Почему Esc-детект в Engine, а не в компоненте:** когда мир заморожен (isPaused=true), компоненты не обновляются. Если бы Esc обрабатывал компонент — он не смог бы снять паузу. Поэтому Esc-детект вынесен в движок, который работает всегда.

### Методы паузы

```cpp
void Engine::SetPaused(bool value) { isPaused = value; }
bool Engine::IsPaused() const { return isPaused; }
```

Используются: `PauseComponent` (рисует оверлей если IsPaused), `GameMain` (при необходимости).

---

## Этап 3: Компонентная модель — ядро архитектуры

### `Component` — базовый класс (`Component.h`)

```cpp
class Component {
  protected:
    GameObject *gameObject;  // кто владеет этим компонентом

  public:
    Component(GameObject *gameObject);
    virtual ~Component();

    virtual void Update(float deltaTime) = 0;  // чисто виртуальная
    virtual void Render() = 0;                 // чисто виртуальная

    GameObject *GetGameObject();
};
```

**`gameObject`** — указатель на владельца. Через него компонент обращается к "братьям": `gameObject->GetComponent<TransformComponent>()`.

**`= 0`** — чисто виртуальные методы. Нельзя создать `Component` напрямую, только наследника.

### `GameObject` — контейнер компонентов (`GameObject.h`)

```cpp
class GameObject {
  private:
    std::string name;
    std::vector<Component *> components;
    std::vector<GameObject *> children;

  public:
    // Конструктор: создаёт TransformComponent автоматически
    GameObject(std::string newName) {
        name = newName;
        AddComponent<TransformComponent>();  // всегда есть!
    }

    // Добавить компонент (шаблонный)
    template <typename T>
    T *AddComponent() {
        // Проверка: T должен наследовать от Component
        if constexpr (!std::is_base_of<Component, T>::value)
            return nullptr;

        // Проверка: TransformComponent только один
        if constexpr (std::is_same<T, TransformComponent>::value)
            if (GetComponent<TransformComponent>() != nullptr)
                return nullptr;

        T *newComponent = new T(this);  // this = этот GameObject
        components.push_back(newComponent);
        return newComponent;
    }

    // Добавить компонент с аргументами конструктора
    template <typename T, typename... Args>
    T *AddComponent(Args &&...args) {
        T *newComponent = new T(this, std::forward<Args>(args)...);
        components.push_back(newComponent);
        return newComponent;
    }

    // Найти компонент по типу (linear search + dynamic_cast)
    template <typename T>
    T *GetComponent() {
        for (auto component : components)
            if (auto casted = dynamic_cast<T *>(component))
                return casted;
        return nullptr;
    }

    // Обновить все компоненты
    void Update(float deltaTime) {
        for (auto &component : components)
            component->Update(deltaTime);
    }

    // Нарисовать все компоненты
    void Render() {
        for (auto &component : components)
            component->Render();
    }
};
```

### Расшифровка ключевых методов

**`AddComponent<T>()`** — создаёт компонент типа T и добавляет в вектор:
```cpp
auto renderer = gameObject->AddComponent<SpriteRendererComponent>();
// renderer — указатель на созданный SpriteRendererComponent
```

**`AddComponent<T>(аргументы)`** — с параметрами конструктора:
```cpp
auto stats = gameObject->AddComponent<StatsComponent>(100.0f, 50.0f);
// вызывает StatsComponent(gameObject, 100.0f, 50.0f)
```

**`GetComponent<T>()`** — ищет компонент по типу через `dynamic_cast`:
```cpp
auto transform = gameObject->GetComponent<TransformComponent>();
// если TransformComponent есть → указатель на него
// если нет → nullptr
```

**Деструктор `~GameObject()`** — удаляет ВСЕ компоненты:
```cpp
~GameObject() {
    for (auto component : components)
        delete component;
    components.clear();
}
```

### Зачем компонентная модель

```
БЕЗ компонентов (наследование):
    class Entity → class Character → class Player
    class Entity → class Character → class Enemy
    // Проблема: Player и Enemy имеют 90% общего кода

С компонентами (композиция):
    GameObject("Player") + [Renderer, Movement, Input, Stats, Attack]
    GameObject("Enemy")  + [Renderer, Follow, Stats, Attack]
    // Преимущество: общие компоненты переиспользуются
```

---

## Этап 4: Построение уровня — `DeveloperLevel::Start()`

### Пошагово (что вызывает что)

```cpp
void DeveloperLevel::Start() {
    // === КОНСТАНТЫ ===
    int width = 15;   // размер лабиринта (клеток)
    int height = 15;

    // === 1. ВЫБОР ВЫХОДА ===
    int exitX, exitY;
    ChooseExitTile(width, height, exitX, exitY);
    // Случайная сторона (0-3), случайная позиция на ней
    // Выход ВСЕГДА на рамке

    // === 2. РАМКА СТЕН ===
    // Двойной цикл по всем клеткам
    // Внутренность → Floor
    // Рамка → Wall (кроме клетки выхода — isExitTile)
    // Углы → специальные текстуры (25, 27, 1, 3)
    // Боковые → текстура 12 или 38

    // === 3. ГЕНЕРАЦИЯ ЛАБИРИНТА ===
    MazeGenerator mazeGenerator(width, height, this, exitX, exitY);
    mazeGenerator.Generate();
    // Внутри: DFS-прогрызание + ConnectExitToMaze + автотайлинг

    // === 4. ИГРОК ===
    player = std::make_shared<Player>({width/2 * 128, height/2 * 128});
    // Создаётся в ЦЕНТРЕ карты
    // Затем добавляются компоненты:
    player->GetGameObject()->AddComponent<PlayerDeathComponent>(this);
    player->GetGameObject()->AddComponent<PlayerAttackComponent>();

    // === 5. КАМЕРА ===
    // Настраивается на весь лабиринт (SetBaseResolution)

    // === 6. БАЗОВЫЙ ВРАГ ===
    ai = std::make_shared<AI>({width/3 * 128, height/3 * 128}, player->GetGameObject());

    // === 7. СПАВНЕР ===
    creeperSpawner = std::make_unique<CreeperSpawner>();
    creeperSpawner->ReservePosition(ai->GetPosition());   // не спавнить на волшебнике
    creeperSpawner->AddExternalEnemy(ai.get());           // волшебник доступен для атак

    // === 8. КРИПЕРЫ ===
    int baseCount = ((2 + currentLevel) * 4 + 2) / 5;  // −20% от 2+level
    creeperSpawner->SpawnCreepers(baseCount, ...);

    SpawnConfig customConfig;  // усиленные
    customConfig.count = ((1 + currentLevel / 2) * 4 + 2) / 5;
    customConfig.healthMultiplier = 1.0f + currentLevel * 0.25f;
    // ...
    creeperSpawner->SpawnEnemies(customConfig, ...);

    creeperSpawner->SpawnWave(currentLevel, ...);  // волна

    // === 9. БОССЫ ===
    int bossCount = std::min(currentLevel, 4);
    for (int i = 0; i < bossCount; ++i) {
        creeperSpawner->SpawnBoss(...);
        // Последний элемент в enemies — это босс
        auto bossObject = spawnedEnemies.back()->GetGameObject();
        bossObject->AddComponent<BossDeathComponent>(this);
        SetupBoss(bossObject);
        bossObjects.push_back(bossObject);
    }

    // === 10. СТЕНЫ-ЦЕЛИТЕЛИ ===
    // N = min(2^(level-1), 10) на каждый цвет
    // Случайные внутренние стены → HealingWallComponent

    // === 11. ВЫХОД ===
    CreateExitTrigger(exitX, exitY);
    // Закрыт: серый, коллайдер выключен
    // + ExitLabelComponent ("Exit", красный)

    // === 12. HUD ===
    auto hudObject = CreateGameObject("GameHUD");
    hud = hudObject->AddComponent<GameHUDComponent>(currentLevel, this);
    hud->SetTarget(player->GetGameObject());

    // === 13. ИНВЕНТАРЬ ===
    auto inventory = player->GetGameObject()->GetComponent<InventoryComponent>();
    if (!inventory) inventory = player->GetGameObject()->AddComponent<InventoryComponent>();
    inventory->ResetStocks();
    inventory->SetEnemySpawner(creeperSpawner.get());

    // === 14. МУЗЫКА ===
    music = std::make_unique<Music>("music");

    // === 15. ФИНАЛИЗАЦИЯ ===
    isLevelTransitionInProgress = false;
    exitOpened = false;
}
```

### Расшифровка приватных методов

**`ChooseExitTile(width, height, &exitX, &exitY)`**
```cpp
const int side = std::rand() % 4;  // 0=лево, 1=право, 2=низ, 3=верх
// side=0: exitX=0, exitY=1..height-1 (случайная)
// side=1: exitX=width, exitY=1..height-1
// side=2: exitY=0, exitX=1..width-1
// side=3: exitY=height, exitX=1..width-1
```
Выход всегда на границе, никогда в углу (exitY от 1, не 0).

**`CreateExitTrigger(exitX, exitY)`**
```cpp
// Создаёт GameObject "LevelExit"
// Позиция: (exitX * 128, exitY * 128)
// Компоненты:
//   SpriteRenderer — level_floors[0], серый (120,120,120)
//   Rigidbody — kinematic (не двигается)
//   SpriteCollider — trigger=true, enabled=false (закрыт!)
//   AudioComponent — Transition.wav
//   ExitLabelComponent — "Exit", красный
//
// Подписка на игрока:
playerCollider->SubscribeTriggerEnter([this](Trigger) {
    GameWorld::EnqueueLateAction([this]() { LoadNextLevel(); });
});
// При касании выхода → отложенный переход
```

**`OpenExit()`**
```cpp
// Вызывается из OnBossDied() когда ВСЕ боссы мертвы
// 1. exitRenderer->SetColor(зелёный)
// 2. exitCollider->SetEnabled(true)  — можно войти
// 3. exitLabel->SetOpened(true)      — зелёная надпись
```

**`OnBossDied()`**
```cpp
// Вызывается из BossDeathComponent
// Проверяет ВСЕХ боссов в bossObjects
// Если хоть один жив → return (ждём)
// Если все мертвы → OpenExit()
```

**`SetupBoss(bossObject)`**
```cpp
// 1. BossAbilitiesComponent(player) — AOE + ярость
// 2. Текстура → "Creeper" атлас, фиолетовый (170,0,255)
// 3. Размер → 128px (как у криперов)
// 4. Клипы → walk 0.2с (в 2р медленнее), attack, death
// 5. Скорость → 60 (в 2р медленнее криперов)
// 6. Коллайдер → SetSize(122, 128)
```

**`LoadNextLevel()`**
```cpp
isLevelTransitionInProgress = true;
currentLevel++;
Restart();  // Stop() + Start()
```

**`Stop()`**
```cpp
creeperSpawner->Clear();  // очистить список врагов
walls.clear();            // удалить обёртки стен
floors.clear();           // удалить обёртки полов
ai.reset();               // удалить волшебника
player.reset();           // удалить игрока
music.reset();            // остановить музыку
exitObject = nullptr;
bossObjects.clear();
GameWorld::Clear();       // удалить ВСЕ GameObject'ы
```

---

## Этап 5: Игрок — `Player.cpp`

### Полный код с комментариями

```cpp
Player::Player(const XYZEngine::Vector2Df &position) {
    // 1. Создать GameObject в мире
    gameObject = GameWorld::Instance()->CreateGameObject("Player");

    // 2. Установить позицию (TransformComponent уже создан автоматически)
    auto transform = gameObject->GetComponent<TransformComponent>();
    transform->SetWorldPosition(position);

    // 3. ВИЗУАЛ: спрайт из атласа
    auto playerRenderer = gameObject->AddComponent<SpriteRendererComponent>();
    auto playerTexture = ResourceSystem::Instance()
        ->GetTextureMapElementShared("Player", 0);  // кадр 0 из атласа "Player"
    playerRenderer->SetTexture(*playerTexture);
    // Сохранить пропорции: ширина = 128 * (texW / texH)
    playerRenderer->SetPixelSize(desiredWidth, 128);

    // 4. КАМЕРА: следует за игроком
    auto playerCamera = gameObject->AddComponent<CameraComponent>();
    playerCamera->SetWindow(&RenderSystem::Instance()->GetMainWindow());
    playerCamera->SetBaseResolution(1280, 720);

    // 5. ВВОД: WASD
    gameObject->AddComponent<InputComponent>();

    // 6. ДВИЖЕНИЕ: скорость 400 px/сек
    auto movement = gameObject->AddComponent<MovementComponent>();
    movement->SetSpeed(400);

    // 7. ПОВОРОТ: влево/вправо по смещению
    gameObject->AddComponent<SpriteDirectionComponent>();

    // 8. АНИМАЦИИ: 4 клипа из атласа "Player" (14 кадров)
    auto animation = gameObject->AddComponent<SpriteMovementAnimationComponent>();
    animation->AddAnimation("walk",   "Player", {0,1,2,3,4,5,6,7}, 0.08f, true);
    animation->AddAnimation("idle",   "Player", {0},               0.10f, true);
    animation->AddAnimation("attack", "Player", {8,9,10,11,12},    0.08f, false);
    animation->AddAnimation("death",  "Player", {13},              0.15f, false);
    animation->Play("idle");  // стартовое состояние

    // 9. ФИЗИКА: динамический объект (толкается стенами)
    auto rigidbody = gameObject->AddComponent<RigidbodyComponent>();
    rigidbody->SetKinematic(false);

    // 10. КОЛЛАЙДЕР: 122×128 (проходит в 128px-проходы)
    auto collider = gameObject->AddComponent<SpriteColliderComponent>();
    collider->SetSize(122.f, 128.f);

    // 11. СТАТЫ: 100 HP + 50 брони
    gameObject->AddComponent<StatsComponent>(100.0f, 50.0f);

    // 12. АТАКА: базовый урон 10
    gameObject->AddComponent<AttackComponent>(10.0f);

    // 13. ЖИЗНИ: 3 сердца
    gameObject->AddComponent<PlayerLivesComponent>();

    // 14. ПАУЗА: оверлей "PAUSED"
    gameObject->AddComponent<PauseComponent>();

    // 15. ПОЛОСКИ НАД ГОЛОВОЙ
    // Красная HP (выше)
    auto healthBar = gameObject->AddComponent<HealthBarComponent>();
    healthBar->SetOffset(0.f, 100.f);   // 100px выше центра
    healthBar->SetSize(90.f, 8.f);

    // Жёлтая Armor (ниже красной)
    auto armorBar = gameObject->AddComponent<HealthBarComponent>();
    armorBar->UseArmor(true);
    armorBar->SetFillColor(sf::Color::Yellow);
    armorBar->SetOffset(0.f, 88.f);     // 88px выше центра
    armorBar->SetSize(90.f, 8.f);
}
```

---

## Этап 6: Враги

### `AI.cpp` — базовый враг

```cpp
AI::AI(GameObject *player, const std::string &enemyName, int id)
    : fallowTarget(player)  // запомнить игрока
{
    // Создать GameObject
    gameObject = GameWorld::Instance()->CreateGameObject(enemyName + "_" + id);

    // Визуал: Wizard.png
    auto renderer = gameObject->AddComponent<SpriteRendererComponent>();
    auto texture = ResourceSystem::GetTextureMapElementShared("ai", 0);
    renderer->SetTexture(*texture);
    renderer->SetPixelSize(desiredWidth, 128);

    // Преследование: скорость 120
    auto follower = gameObject->AddComponent<FollowComponent>();
    follower->SetTarget(player);
    follower->SetSpeed(120);

    // Физика + коллайдер
    gameObject->AddComponent<RigidbodyComponent>();
    gameObject->AddComponent<SpriteColliderComponent>();

    // Статы: 100 HP, 0 брони
    gameObject->AddComponent<StatsComponent>(100, 0);

    // Атака: 10 урона
    gameObject->AddComponent<AttackComponent>(10);

    // Полоска HP над головой
    auto healthBar = gameObject->AddComponent<HealthBarComponent>();
    healthBar->SetOffset(0.f, 80.f);
    healthBar->SetSize(70.f, 8.f);

    // Смерть: коллайдер off → 2с → исчезновение
    gameObject->AddComponent<EnemyDeathComponent>();

    // Анимации: walk 0–6, attack 7–11, death 12
    auto animation = gameObject->AddComponent<SpriteMovementAnimationComponent>();
    animation->AddAnimation("walk", "ai", {0,1,2,3,4,5,6}, 0.1f, true);
    animation->AddAnimation("idle", "ai", {0}, 0.1f, true);
    animation->AddAnimation("attack", "ai", {7,8,9,10,11}, 0.08f, false);
    animation->AddAnimation("death", "ai", {12}, 0.15f, false);
    animation->Play("idle");
}
```

### `FollowComponent::Update()` — детально

```cpp
void FollowComponent::Update(float deltaTime) {
    // 0. Лениво получить компоненты (могли быть добавлены после нас)
    if (animation == nullptr)
        animation = gameObject->GetComponent<SpriteMovementAnimationComponent>();
    if (stats == nullptr)
        stats = gameObject->GetComponent<StatsComponent>();

    // 1. Я мёртв? → ничего не делаем
    if (stats != nullptr && stats->GetCurrentHealth() <= 0.f)
        return;

    // 2. Замах активен? → обрабатываем удар
    if (isAttackActive) {
        swingTimer += deltaTime;

        // Урон в "кадр удара" (0.22с после начала замаха)
        if (!damageApplied && swingTimer >= damageMoment) {
            damageApplied = true;
            if (attack && targetGameObject)
                attack->Attack(targetGameObject);  // → TakeDamage
        }

        // Конец замаха (0.4с)
        if (swingTimer >= attackDuration)
            isAttackActive = false;

        return;  // во время замаха НЕ двигаемся
    }

    // 3. Цель мертва? → стоим
    if (targetGameObject) {
        if (targetStats == nullptr)
            targetStats = targetGameObject->GetComponent<StatsComponent>();
        if (targetStats && targetStats->GetCurrentHealth() <= 0.f) {
            animation->Play("idle");
            return;
        }
    }

    // 4. Направление к цели
    Vector2Df direction = targetPos - myPos;
    float length = direction.GetLength();

    // 5. Двигаться если далеко
    if (length > attackRange) {  // attackRange = 140
        Vector2Df normalized = direction / length;
        transform->MoveBy(normalized * speed * deltaTime);
        // speed = 120 (крипер) или 60 (босс)
    }

    // 6. Начать замах если близко + кулдаун прошёл
    attackTimer += deltaTime;
    if (length < attackRange && attackTimer >= attackCooldown) {
        attackTimer = 0;
        isAttackActive = true;
        damageApplied = false;
        swingTimer = 0;
        animation->Play("attack");
        attackSound->Stop(); attackSound->Play();
    }

    // 7. Анимация: walk/idle (но не перебивать attack/death)
    if (animation && !animation->IsPlaying("attack")
                  && !animation->IsCurrentAnimation("death"))
        animation->Play(isWalking ? "walk" : "idle");
}
```

### Параметры FollowComponent

| Параметр | Значение | Где задаётся |
|---|---|---|
| `speed` | 120 (крипер), 60 (босс) | AI.cpp / SetupBoss |
| `attackRange` | 140 px | FollowComponent.h |
| `attackCooldown` | 1.0 сек | FollowComponent.h |
| `damageMoment` | 0.22 сек | FollowComponent.h |
| `attackDuration` | 0.4 сек | FollowComponent.h |

---

## Этап 7: Физика — `PhysicsSystem.cpp`

### `PhysicsSystem::Update()` — детально

```cpp
void PhysicsSystem::Update() {
    // Внешний цикл: объект i (динамический)
    for (int i = 0; i < colliders.size(); i++) {
        if (!colliders[i]->enabled) continue;  // отключён (труп)

        auto body = colliders[i]->GetGameObject()
                       ->GetComponent<RigidbodyComponent>();
        if (body->GetKinematic()) continue;    // стена — не обрабатываем

        // Внутренний цикл: объект j (с которым столкнулись)
        for (int j = 0; j < colliders.size(); j++) {
            if (j == i) continue;
            if (!colliders[j]->enabled) continue;

            // Проверка пересечения прямоугольников
            sf::FloatRect intersection;
            if (colliders[i]->bounds.intersects(colliders[j]->bounds, intersection))
            {
                // ТРИГГЕР: один trigger, другой — нет
                if (colliders[i]->isTrigger != colliders[j]->isTrigger) {
                    if (пара ещё не записана) {
                        auto trigger = new Trigger(colliders[i], colliders[j]);
                        colliders[i]->OnTriggerEnter(*trigger);
                        colliders[j]->OnTriggerEnter(*trigger);
                        triggersEnteredPair.emplace(colliders[i], colliders[j]);
                    }
                }
                // ФИЗИЧЕСКОЕ СТОЛКНОВЕНИЕ: оба не trigger
                else if (!colliders[i]->isTrigger) {
                    // Развести по центрам, минимальная ось проникновения
                    float aCenterX = центр объекта i по X;
                    float bCenterX = центр объекта j по X;

                    if (intersection.width > intersection.height) {
                        // Толкнуть по Y
                        float dir = (aCenterY >= bCenterY) ? 1 : -1;
                        transform->MoveBy({0, dir * intersection.height});
                    } else {
                        // Толкнуть по X
                        float dir = (aCenterX >= bCenterX) ? 1 : -1;
                        transform->MoveBy({dir * intersection.width, 0});
                    }
                }
            }
        }
    }

    // Проверка выхода из триггеров
    for (auto pair : triggersEnteredPair) {
        if (!pair.first->bounds.intersects(pair.second->bounds)) {
            OnTriggerExit...
            erase pair;
        }
    }
}
```

### `SpriteColliderComponent` — коллайдер

| Метод | Что делает | Кто использует |
|---|---|---|
| `SetTrigger(bool)` | триггер (не толкает, но событие) | Exit |
| `SetEnabled(bool)` | вкл/выкл коллайдера | EnemyDeath (выкл труп), OpenExit (вкл выход) |
| `SetSize(w, h)` | явный размер (не от спрайта) | Player (122×128), Boss (122×128) |

### `ColliderComponent::bounds` — как вычисляется

```cpp
// В Update() каждый кадр:
bounds = {
    transform->GetWorldPosition().x - width/2,
    transform->GetWorldPosition().y - height/2,
    width,
    height
};
// Позиция — из transform (актуальна сразу)
// Размер — из спрайта ИЛИ SetSize
```

---

## Этап 8: Генерация лабиринта — `MazeGenerator.cpp`

### `Generate()` — пошагово

```cpp
void MazeGenerator::Generate() {
    // 1. Сид для случайности
    std::srand(std::time(nullptr));

    // 2. Случайная стартовая клетка
    int startX = rand() % width;
    int startY = rand() % height;
    grid[startY][startX] = true;  // true = открыта

    // 3. DFS с бэктрекингом
    std::stack<std::pair<int,int>> stack;
    stack.push({startX, startY});

    while (!stack.empty()) {
        auto [x, y] = stack.top();
        stack.pop();

        // Соседи на 2 клетки (для толщины стен)
        auto directions = GetAvailableDirections(x, y);
        // → {{0,-2},{0,2},{-2,0},{2,0}} отфильтрованные

        if (!directions.empty()) {
            stack.push({x, y});  // вернуться сюда позже

            auto dir = directions[rand() % directions.size()];
            int nx = x + dir.first;
            int ny = y + dir.second;

            RemoveWall(x, y, nx, ny);  // прорезать стену между
            grid[ny][nx] = true;       // отметить как открытую
            stack.push({nx, ny});
        }
        // если directions пуст → pop() уже сделан → backtracking
    }

    // 4. Соединить выход с лабиринтом
    ConnectExitToMaze();

    // 5. Расширить подход к выходу (3 клетки)
    // (очистка стен + grid=true + полы)

    // 6. Автотайлинг стен
    for (auto &wall : level->walls) {
        // Для каждой внутренней стены:
        // Смотрим 4 соседей в grid
        // Углы → 1/3/25/27, вертикали → 12, горизонтали → 38
        wall->SetTileIndex(GetWallTileIndex(grid, wx, wy));
    }
}
```

### `RemoveWall(x1, y1, x2, y2)` — прорезание

```cpp
void MazeGenerator::RemoveWall(int x1, int y1, int x2, int y2) {
    int wallX = (x1 + x2) / 2;  // клетка между двумя
    int wallY = (y1 + y2) / 2;

    // Полы на обеих клетках
    level->floors.push_back(Floor({x1*128, y1*128}));
    level->floors.push_back(Floor({x2*128, y2*128}));

    // Стена посередине
    if (wallX != x1 || wallY != y1)
        level->walls.push_back(Wall({wallX*128, wallY*128}, 14));
}
```

### `ConnectExitToMaze()` — гарантированный проход

```cpp
void MazeGenerator::ConnectExitToMaze() {
    // Внутренний сосед выхода (1 клетка от рамки)
    int inX, inY;
    if (exitX == 0) inX = 1;           // левая сторона → сосед справа
    else if (exitX == width) inX = width-2;  // правая → сосед слева
    else if (exitY == 0) inY = 1;       // нижняя → сосед сверху
    else inY = height-2;               // верхняя → сосед снизу

    if (grid[inY][inX]) return;  // уже открыта — не нужно

    // BFS от (inX, inY) до ближайшей открытой клетки
    // Запоминаем родителей для восстановления пути
    // ...
    // backtrack → path[]

    // Прорезать каждую клетку пути:
    for (cell : path) {
        grid[cell.y][cell.x] = true;
        // Добавить пол
        // Удалить стену если есть
    }
}
```

---

## Этап 9: Игровые компоненты — расшифровка

### `GameHUDComponent` — HUD слева сверху

**Update():** собирает строки
```
"Level: 3"                    ← levelNumber
[сердца рисуются отдельно]     ← hearts (Render)
"HP: 84/100  Armor: 12/50"    ← stats
"Kill Boss for Exit!"          ← statusText (IsExitOpened?)
"[1] HP: 5                     ← inventory
[2] Rage: 5
[3] Wave: 5"
```

**Render():** view-swap приём
```cpp
sf::View currentView = window.getView();    // сохранить камеру игры
window.setView(window.getDefaultView());    // перейти к экрану
window.draw(text);                          // рисуем в экранных координатах
window.draw(statusText);
// + сердца (Heart.png спрайты)
window.setView(currentView);               // вернуть камеру
```

### `PlayerDeathComponent` — смерть игрока

```
Update():
    HP <= 0?
    ├── есть сердца (HasLives)?
    │   ├── LoseLife()                     → сердца--
    │   ├── SetCurrentHealth(max)          → полный HP
    │   ├── SetCurrentArmor(max)           → полная броня
    │   ├── SetWorldPosition(896, 896)     → телепорт в центр
    │   └── звук смерти
    │
    └── сердец нет (финальная смерть)
        ├── deathStarted = true, таймер 2с
        ├── Play("death")                   → анимация
        ├── звук смерти
        ├── через 2с → gameOverActive = true
        │   ├── Оверлей (затемнение + текст)
        │   ├── "GAME OVER\n\nR - RESTART\nESC - EXIT"
        │   └── ждём R или Esc
        ├── R → EnqueueLateAction(Restart)  → новый уровень
        └── Esc → window.close()

Render():
    gameOverActive → view-swap → draw оверлей + текст
```

### `InventoryComponent` — зелья

**Update():** каждый кадр
```cpp
// 1. Таймер ярости
if (rageActive) {
    rageTimer -= dt;
    if (rageTimer <= 0) {
        attack->SetAttackPower(baseAttackPower);   // вернуть
        movement->SetSpeed(baseSpeed);
        rageActive = false;
    }
}

// 2. Таймер волны (визуал)
if (waveTimer > 0) {
    waveTimer -= dt;
    float progress = 1 - (waveTimer / 0.5);
    waveRing.setRadius(250 * progress);  // растёт
}

// 3. Клавиши 1/2/3 (edge-detected)
if (key1 && !key1Last) UseHealthPotion();
if (key2 && !key2Last) UseRagePotion();
if (key3 && !key3Last) UseWavePotion();
```

**UseHealthPotion():**
```cpp
if (healthPotions <= 0) return false;

float hpMissing = maxHP - currentHP;
float armorMissing = maxArmor - currentArmor;

if (hpMissing <= 0 && armorMissing <= 0)
    return false;  // всё полное — не тратим

if (hpMissing >= 25)
    stats->Heal(25);                    // всё в HP
else {
    stats->Heal(hpMissing);             // HP до max
    stats->RepairArmor(25 - hpMissing); // остаток в броню
}

healthPotions--;
return true;
```

**UseRagePotion():**
```cpp
if (rageActive) {
    rageTimer = 5;  // продлить
    ragePotions--;
    return true;
}

// Запомнить базовые значения
baseAttackPower = attack->GetAttackPower();    // 10
baseSpeed = movement->GetSpeed();              // 400

// Применить баф
attack->SetAttackPower(baseAttackPower * 1.5); // 15
movement->SetSpeed(baseSpeed * 1.5);           // 600

rageActive = true;
rageTimer = 5;
ragePotions--;
```

**UseWavePotion():**
```cpp
auto position = transform->GetWorldPosition();

// ВСЕ враги в радиусе 250
vector<GameObject*> enemies;
enemySpawner->FindEnemiesInRange(position, 250, enemies);

for (auto enemy : enemies)
    enemy->GetComponent<StatsComponent>()->TakeDamage(30);

// Визуал: голубое кольцо
waveTimer = 0.5;
waveRing.setPosition(position);
wavePotions--;
```

**ResetStocks():**
```cpp
healthPotions = 5;
ragePotions = 5;
wavePotions = 5;

// Отменить ярость если активна
if (rageActive) {
    attack->SetAttackPower(baseAttackPower);
    movement->SetSpeed(baseSpeed);
    rageActive = false;
}
waveTimer = 0;
```

### `HealthBarComponent` — полоски над головой

**Update():** каждый кадр пересчитывает размер заливки
```cpp
float max = useArmor ? stats->GetMaxArmor() : stats->GetMaxHealth();
float current = useArmor ? stats->GetCurrentArmor() : stats->GetCurrentHealth();
float percent = clamp(current / max, 0, 1);

// Фон (тёмно-серый): полная ширина
background.setPosition(pos.x + offset.x, pos.y + offset.y);

// Заливка (красный/жёлтый): ширина * percent
fill.setSize({size.x * percent, size.y});
// Симметрично к центру (origin = половина полной ширины)
```

**Render():** рисует в МИРОВЫХ координатах (следует за объектом)

### `EnemyDeathComponent` — смерть врага

```
Update():
    HP <= 0 && !dying?
    ├── dying = true, deathTimer = 2
    ├── collider->SetEnabled(false)   → труп нематериален
    ├── Play("death")                 → анимация
    └── звук Creeper-death.wav

    dying?
    └── deathTimer -= dt
        └── <= 0 → SetWorldPosition(-2000, -2000)  → за экран
```

### `BossDeathComponent` — смерть босса

```
Update():
    HP <= 0 && !fired?
    └── fired = true
        └── EnqueueLateAction(level->OnBossDied())
            // Проверит ВСЕХ боссов
            // Если все мертвы → OpenExit()
```

### `BossAbilitiesComponent` — способности босса

**Update():**
```cpp
if (stats->GetCurrentHealth() <= 0) return;  // мёртв — нет способностей

// === AOE ===
if (aoeTelegraphActive) {
    telegraphTimer += dt;
    // Кольцо растёт от босса до радиуса 200
    ring.setRadius(200 * (telegraphTimer / 1.0));

    if (telegraphTimer >= 1.0) {  // 1 секунда прошла
        aoeTelegraphActive = false;
        // Игрок в радиусе 200? → TakeDamage(25)
        if (dist(player, boss) < 200)
            playerStats->TakeDamage(25);
    }
    return;  // во время телеграфа не двигаемся
}

aoeTimer += dt;
if (aoeTimer >= 8.0) {  // каждые 8 секунд
    aoeTimer = 0;
    aoeTelegraphActive = true;
    telegraphTimer = 0;
}

// === ЯРОСТЬ ===
if (!rageActive && HP <= maxHP * 0.5) {
    rageActive = true;
    follower->SetSpeed(baseSpeed * 1.5);       // 60 → 90
    follower->SetAttackCooldown(baseCooldown * 0.5);  // 1с → 0.5с
    renderer->SetColor(sf::Color::Red);        // красный
}
```

**Render():**
```cpp
if (aoeTelegraphActive)
    RenderSystem::Render(telegraphRing);  // красное кольцо
```

### `HealingWallComponent` — стены-целители

```
Update():
    дистанция(стена, игрок) < 135px?
    ├── healTimer -= dt
    │   └── <= 0 → +1 HP (зелёная) или +1 Armor (жёлтая)
    │              healTimer = 0.25с
    └── healTimer = 0

    // Пульсация
    pulseTimer += dt
    alpha = 170 + 70 * sin(pulseTimer * 3)
    renderer->SetColor(цвет, alpha)

Render():
    RenderSystem::Render(label);  // "HP" или "Armor"
```

### `ExitLabelComponent` — надпись "Exit"

```
ctor():
    label.setString("Exit")
    label.setCharacterSize(28)
    label.setFillColor(красный)     ← пока боссы живы
    label.setScale(1, -1)           ← инвертированная камера!
    label.setPosition(позиция_выхода + 40 по Y)

SetOpened(true):
    label.setFillColor(зелёный)     ← боссы мертвы

Render():
    RenderSystem::Render(label)
```

### `PauseComponent` — пауза

```
ctor():
    overlay.setSize(1280, 720)       ← на весь экран
    overlay.setFillColor(0, 0, 0, 150) ← полупрозрачный чёрный
    text.setString("PAUSED\n\nEsc - resume")
    text.setPosition(центр экрана)

Render():
    if (Engine::IsPaused())
        view-swap → draw overlay + text
```

---

## Этап 10: Звук

### Загрузка (`GameMain.cpp`)
```cpp
ResourceSystem::LoadSound("Attack", "Resources/Sounds/Attack.wav");
ResourceSystem::LoadSound("Creeper-death", "Resources/Sounds/Creeper-death.wav");
ResourceSystem::LoadSound("Player-death", "Resources/Sounds/Player-death.wav");
ResourceSystem::LoadSound("Transition", "Resources/Sounds/Transition.wav");
ResourceSystem::LoadSound("music", "Resources/Sounds/neon-gaming.wav");
```

### Использование (паттерн для всех компонентов)
```cpp
// В конструкторе компонента:
attackSound = gameObject->AddComponent<AudioComponent>();
auto buffer = ResourceSystem::GetSound("Attack");
attackSound->SetAudio(*buffer);    // звуковой буфер
attackSound->SetLoop(false);       // однократный
attackSound->SetVolume(100);       // максимум

// При действии:
attackSound->Stop();   // чтобы не наслаивались
attackSound->Play();
```

### Константы звука

| Звук | Громкость | Loop | Где играет |
|---|---|---|---|
| Attack.wav | 100 | нет | удар игрока/врага |
| Creeper-death.wav | 100 | нет | смерть врага |
| Player-death.wav | 100 | нет | смерть игрока |
| Transition.wav | 100 | нет | новый уровень |
| neon-gaming.wav | 35 | да | фоновая музыка |

---

## Константы баланса — `GameConstants.h`

```cpp
// Игрок
PLAYER_SPEED = 400      // px/сек
PLAYER_HEALTH = 100
PLAYER_ARMOR = 50
PLAYER_ATTACK = 10      // урон за удар

// Враги
AI_SPEED = 120          // px/сек
AI_HEALTH = 100
AI_ARMOR = 0
AI_ATTACK_POWER = 10

// Криперы
CREEPER_SPEED = 100
CREEPER_HEALTH = 50
CREEPER_ARMOR = 10
CREEPER_ATTACK = 5

// Камера
CAMERA_WIDTH = 1280
CAMERA_HEIGHT = 720

// Босс (в SpawnPresets::Boss)
healthMultiplier = 1.5  → HP = 150
damageMultiplier = 1.5  → урон = 15
speedMultiplier = 0.9   → скорость = 60 (потом SetupBoss ставит 60)

// Способности босса (BossAbilitiesComponent.h)
aoeCooldown = 8 сек
aoeRadius = 200 px
aoeDamage = 25
telegraphDuration = 1 сек
```

---

## Резюме: 7 принципов для джуна

1. **Component-модель** — не «наследуй», а «собирай из кубиков». `GameObject` — контейнер, `Component` — кубик с данными и поведением.

2. **Кадр = Update → FixedUpdate → Render → LateUpdate** — всегда в этом порядке. Update — логика, FixedUpdate — физика, Render — рисование, LateUpdate — отложенные действия.

3. **EnqueueLateAction** — любое изменение мира из колбэка (триггер, коллизия) — только отложенно, в LateUpdate. Иначе сломаешь итерацию.

4. **Синглтоны** — глобальный доступ: `RenderSystem` (окно), `ResourceSystem` (текстуры/звуки), `GameWorld` (объекты), `PhysicsSystem` (коллизии).

5. **Данные отдельно от логики** — `AnimationClip` (данные: кадры, время, цикл) vs `SpriteMovementAnimationComponent` (поведение: Play, Stop, Update). Аналогично `SpawnConfig` (данные) vs `CreeperSpawner` (поведение).

6. **Edge detection** — `keyPressed && !keyPressedLastFrame` → одно нажатие = одно действие. Без этого зелье потратится за один кадр.

7. **View-swap для UI** — `window.setView(getDefaultView())` → рисуем → `window.setView(currentView)`. Экранное пространство поверх игровой камеры.
