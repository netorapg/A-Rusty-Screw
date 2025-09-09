# Diagrama de Classes - A Rusty Screw Game

```plantuml
@startuml A_Rusty_Screw_Class_Diagram

!theme plain
skinparam classAttributeIconSize 0
skinparam classFontSize 10
skinparam packageFontSize 12

' Hierarquia principal de objetos
abstract class Object {
  # mPosition : Vector
  # mSize : Vector
  + getPosition() : Vector
  + getSize() : Vector
  + isVisible() : bool
  + {abstract} render()
}

abstract class StaticObject {
}

abstract class DynamicObject {
  # mVelocity : Vector
  # mOnGround : bool
  # mFalling : bool
  # mPassingThroughPlatform : bool
  + getVelocity() : Vector
  + setVelocity()
  + {abstract} update()
}

Object <|-- StaticObject
Object <|-- DynamicObject

' Objetos estáticos
class Platform {
  - mAnimation : Animation
}

class SolidPlatform {
  - mAnimation : Animation
}

class Door {
  - mAnimation : Animation
  - mLevelToLoad : string
  - mSpawnPosition : Vector
  + getLevelToLoad() : string
  + getSpawnPosition() : Vector
}

class Decoration {
  - mAnimation : Animation
}

class Gate {
  - mState : GateState
  - mTargetScrew : Screw*
  + getState() : GateState
  + setTargetScrew()
}

class ToolTip {
  - mType : ToolTipType
  - mCollected : bool
  + getType() : ToolTipType
  + isCollected() : bool
}

class Screw {
  - mType : ScrewType
  - mDestroyed : bool
  - mRespawnTimer : float
  + getType() : ScrewType
  + isDestroyed() : bool
}

StaticObject <|-- Platform
StaticObject <|-- SolidPlatform
StaticObject <|-- Door
StaticObject <|-- Decoration
StaticObject <|-- Gate
StaticObject <|-- ToolTip
StaticObject <|-- Screw

' Objetos dinâmicos
class Player {
  - mFacingDirection : int
  - mCurrentHealth : int
  - mMaxHealth : int
  - mCurrentAttackType : AttackType
  - mCollectedToolTips : set<AttackType>
  - mIsAttacking : bool
  - mInvulnerabilityTimer : float
  + moveLeft()
  + moveRight()
  + jump()
  + startAttack()
  + takeDamage()
  + getCurrentHealth() : int
}

class Enemy {
  # mCurrentState : EnemyState
  # mFacingDirection : int
  # mCurrentHealth : int
  # mIsAttacking : bool
  # mFollowSpeed : float
  # mAttackDistance : float
  + updateWithPlayer()
  + takeDamage()
}

class Punktauro {
  - mCurrentPhase : PunktauroState
  - mPhaseTransitionTimer : float
  + updatePhase()
  + isDefeated() : bool
}

class Chicken {
  - mFollowSpeed : float
  - mFollowDistance : float
  - mFacingDirection : int
  + followPlayer()
  + setFollowSpeed()
}

class Crate {
  + applyForce()
}

DynamicObject <|-- Player
DynamicObject <|-- Enemy
DynamicObject <|-- Chicken
DynamicObject <|-- Crate
Enemy <|-- Punktauro

' Classes de gerenciamento principal
class GameManager {
  - mWorld : GameWorld
  - mHUD : HUD
  - mCurrentState : GameState
  - mGameOverScreen : GameOverScreen
  - mStartMenu : StartMenu
  - mPauseMenu : PauseMenu
  - mCreditsScreen : CreditsScreen
  - mQuit : bool
  - mScore : float
  + handleEvents()
  + update()
  + render()
  + resetGame()
}

class GameWorld {
  - mPlayer : Player
  - mCamera : Camera
  - mChicken : Chicken
  - mInputManager : InputManager
  - mPlatforms : list<Platform>
  - mSolidPlatforms : list<SolidPlatform>
  - mCrates : list<Crate>
  - mDoors : list<Door>
  - mGates : list<Gate>
  - mToolTips : list<ToolTip>
  - mScrews : list<Screw>
  - mEnemies : list<Enemy>
  - mPunktauro : unique_ptr<Punktauro>
  + loadLevelFromTMX()
  + updateWorld()
  + renderWorld()
  + getPlayer() : Player&
}

' Composições principais
GameManager *-- GameWorld
GameManager *-- HUD
GameManager *-- GameOverScreen
GameManager *-- StartMenu
GameManager *-- PauseMenu
GameManager *-- CreditsScreen

GameWorld *-- Player
GameWorld *-- Camera
GameWorld *-- Chicken
GameWorld *-- InputManager
GameWorld o-- "many" Platform
GameWorld o-- "many" SolidPlatform
GameWorld o-- "many" Crate
GameWorld o-- "many" Door
GameWorld o-- "many" Decoration
GameWorld o-- "many" Gate
GameWorld o-- "many" ToolTip
GameWorld o-- "many" Screw
GameWorld o-- "many" Enemy
GameWorld o-- "0..1" Punktauro

' Classes de suporte - Gráficos e Animação
class Sprite {
  - mTexture : SDL_Texture*
  - mSrcRect : SDL_Rect
  + draw()
}

class Animation {
  - mFrames : vector<AnimationFrame>
  - mCurrentFrame : int
  - mTimer : float
  - mLoop : bool
  + addFrame()
  + update()
  + getCurrentSprite() : Sprite*
}

class AnimationFrame {
  + sprite : SpritePtr
  + duration : float
  + offset : SDL_Point
}

Animation *-- "many" AnimationFrame
AnimationFrame --> Sprite

' Classes de interface
class HUD {
  - mFont : TTF_Font*
  - mVisible : bool
  + render()
  + setScore()
}

class StartMenu {
  - mSelectedOption : StartMenuOption
  - mFont : TTF_Font*
  + handleInput()
  + render()
}

class PauseMenu {
  - mSelectedOption : PauseMenuOption
  + handleInput()
  + render()
}

class GameOverScreen {
  - mSelectedOption : GameOverOption
  - mScore : float
  + handleInput()
  + render()
  + setFinalScore()
}

class CreditsScreen {
  - mCreditLines : vector<CreditLine>
  - mScrollPosition : float
  + render()
  + update()
}

' Classes utilitárias
class Camera {
  - mPosition : Vector
  - mSize : Vector
  + move()
  + setPosition()
  + getPosition() : Vector
}

class InputManager {
  - mController : SDL_GameController*
  - mActionStates : map<InputAction, bool>
  + updateInput()
  + isActionPressed() : bool
  + isActionJustPressed() : bool
}

class CollisionEngine {
  + {static} CheckCollision() : bool
  + {static} HandleCollisions()
  + {static} normalize() : Vector
}

' Enums e estruturas auxiliares
enum GameState {
  MENU
  PLAYING
  GAME_OVER
  PAUSED
  CREDITS
}

enum AttackType {
  CUTTING
  PIERCING
}

enum ScrewType {
  FLATHEAD
  PHILLIPS
}

enum ToolTipType {
  FLATHEAD
  PHILLIPS
}

enum EnemyState {
  IDLE
  FOLLOWING
  ATTACKING
  STUNNED
}

enum PunktauroState {
  PHASE_1
  PHASE_2
  PHASE_3
  DEFEATED
}

enum GateState {
  CLOSED
  OPENING
  OPEN
  CLOSING
}

enum InputAction {
  MOVE_LEFT
  MOVE_RIGHT
  JUMP
  ATTACK
  DASH
  SWITCH_ATTACK
}

' Relações de uso/dependência
GameManager --> GameState
Player --> AttackType
Player --> Animation
Enemy --> EnemyState
Enemy --> Animation
Punktauro --> PunktauroState
Chicken --> Animation
Screw --> ScrewType
ToolTip --> ToolTipType
Gate --> GateState
Gate --> Screw : "observes"
Platform --> Animation
SolidPlatform --> Animation
Door --> Animation
Decoration --> Animation
InputManager --> InputAction

' Uso do CollisionEngine
GameWorld ..> CollisionEngine : "uses"
CollisionEngine ..> Object : "operates on"

' HUD monitora Player e boss
HUD --> Player : "observes"
HUD --> Punktauro : "observes"

@enduml
```

## Resumo da Arquitetura

### Hierarquia Principal de Objetos:
- **Object** (classe base abstrata)
  - **StaticObject** (objetos estáticos no mundo)
    - Platform, SolidPlatform, Door, Decoration, Gate, ToolTip, Screw
  - **DynamicObject** (objetos que se movem e têm física)
    - Player, Enemy (e Punktauro como subtipo), Chicken, Crate

### Gerenciamento Principal:
- **GameManager**: Classe central que coordena todo o jogo
- **GameWorld**: Gerencia todos os objetos do mundo do jogo
- **GameState**: Enum que controla o estado atual do jogo

### Sistemas de Suporte:
- **Animation/Sprite**: Sistema de gráficos e animação
- **Camera**: Sistema de câmera
- **InputManager**: Gerenciamento de entrada
- **CollisionEngine**: Sistema de detecção de colisões
- **HUD**: Interface do usuário

### Telas/Menus:
- StartMenu, PauseMenu, GameOverScreen, CreditsScreen

### Enums e Estados:
- Diversos enums para controlar estados de diferentes entidades (GameState, AttackType, EnemyState, etc.)

Esta arquitetura segue um padrão bem estruturado com separação clara de responsabilidades, hierarquia de objetos baseada em comportamento (estático vs dinâmico), e sistemas modulares de suporte.
