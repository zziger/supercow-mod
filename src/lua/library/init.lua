﻿---@meta

---@package

MOD_PATH = ""

---@class ModInfo
---@field id string
---@field title string
---@field author string
---@field version string
---@field basePath string
---@field luaScript string
---@field gameVersions number[]
---@field compatible boolean
---@field internal boolean

currentMod = {} --[[@as ModInfo]]

---@param cache boolean
---@param pat string
---@return number
function findPattern(cache, pat) end

---Возвращает текущее время в миллисекундах
---@return number
function currentTimestamp() end

---@class vector2
---@field x number
---@field y number

---@class vector3: vector2
---@field z number

---@class rect
---@field left number
---@field top number
---@field right number
---@field bottom number

---@class rgba
---@field r number
---@field g number
---@field b number
---@field a number

---@param x number
---@param y number
---@return vector2
function vector2(x, y) end

---@param x number
---@param y number
---@param z number
---@return vector3
function vector3(x, y, z) end

---@param left number
---@param top number
---@param right number
---@param bottom number
---@return rect
function rect(left, top, right, bottom) end

---@param r number
---@param g number
---@param b number
---@param a number
---@return rgba
function rgba(r, g, b, a) end

--#region Log
---@hidden
log = {}

---@param msg string Текст лога
function log.info(msg) end

---@param msg string Текст лога
function log.debug(msg) end

---@param msg string Текст лога
function log.warn(msg) end

---@param msg string Текст лога
function log.error(msg) end

---@param msg string Текст лога
---@deprecated Используйте `log.info`
function log.game(msg) end
--#endregion

--#region Game
---@hidden
game = {}
---@hidden
game.world = {}

function game.isGameLoaded() end

---@return rect
function game.world.getCamWorldRect() end

---@return vector2
function game.world.getWorldProjectionSize() end

---@return number
function game.world.getCamZoom() end

---@return vector2
function game.world.getCamPos() end

---@return vector2
function game.world.screenToWorld() end

---@param coords vector2
---@return boolean
---@return vector2
function game.world.worldToScreen(coords) end
--#endregion

require('base')

--- Вызывается модом каждый кадр игры. Можно использовать ImGui API
---@hidden
function render() end

--- Вызывается модом каждый кадр, когда в главном меню выбран текущий мод. Можно использовать ImGui API
---@hidden
function renderUi() end