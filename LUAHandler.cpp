/* -*- C++ -*-
 *
 *  LUAHandler.cpp - LUA handler for ONScripter
 *
 *  Copyright (c) 2001-2015 Ogapee. All rights reserved.
 *
 *  ogapee@aqua.dti2.ne.jp
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "LUAHandler.h"
#include "ONScripter.h"
#include "ScriptHandler.h"

#define ONS_LUA_HANDLER_PTR "ONS_LUA_HANDLER_PTR"
#define INIT_SCRIPT "system.lua"

static char cmd_buf[256];

int NL_dofile(lua_State *state)
{
    lua_getglobal( state, ONS_LUA_HANDLER_PTR );
    LUAHandler *lh = (LUAHandler*)lua_topointer( state, -1 );

    const char *str = luaL_checkstring( state, 1 );
    
    unsigned long length = lh->sh->cBR->getFileLength(str);
    if (length == 0){
        printf("cannot open %s\n", str);
        return 0;
    }

    unsigned char *buffer = new unsigned char[length+1];
    int location;
    lh->sh->cBR->getFile(str, buffer, &location);
    buffer[length] = 0;

    unsigned char *buffer2 = new unsigned char[length*3/2];
    unsigned char *p = buffer;
    unsigned char *p2 = buffer2;
    while(*p){
        if (IS_TWO_BYTE(*p)){
            *p2++ = *p++;
            if (*p == '\\') *p2++ = '\\';
        }
        *p2++ = *p++;
    }

    if (luaL_loadbuffer(state, (const char*)buffer2, p2 - buffer2, str) || 
        lua_pcall(state, 0, 0, 0)){
        printf("cannot parse %s %s\n", str, lua_tostring(state,-1));
    }

    delete[] buffer;
    delete[] buffer2;

    return 0;
}

int NSPopInt(lua_State *state)
{
    lua_getglobal( state, ONS_LUA_HANDLER_PTR );
    LUAHandler *lh = (LUAHandler*)lua_topointer( state, -1 );

    int val = lh->sh->getEndStatus();
    if (val & ScriptHandler::END_COMMA && !(val & ScriptHandler::END_COMMA_READ)){
        lua_pushstring( state, "LUAHandler::NSPopInt() no integer." );
        lua_error( state );
    }
    
    lua_pushnumber( state, lh->sh->readInt() );

    return 1;
}

int NSPopIntRef(lua_State *state)
{
    lua_getglobal( state, ONS_LUA_HANDLER_PTR );
    LUAHandler *lh = (LUAHandler*)lua_topointer( state, -1 );

    int val = lh->sh->getEndStatus();
    if (val & ScriptHandler::END_COMMA && !(val & ScriptHandler::END_COMMA_READ)){
        lua_pushstring( state, "LUAHandler::NSPopIntRef() no integer variable." );
        lua_error( state );
    }

    lh->sh->readVariable();
    if (lh->sh->current_variable.type != ScriptHandler::VAR_INT){
        lua_pushstring( state, "LUAHandler::NSPopIntRef() no integer variable." );
        lua_error( state );
    }
        
    lua_pushnumber( state, lh->sh->current_variable.var_no );

    return 1;
}

int NSPopStr(lua_State *state)
{
    lua_getglobal( state, ONS_LUA_HANDLER_PTR );
    LUAHandler *lh = (LUAHandler*)lua_topointer( state, -1 );

    int val = lh->sh->getEndStatus();
    if (val & ScriptHandler::END_COMMA && !(val & ScriptHandler::END_COMMA_READ)){
        lua_pushstring( state, "LUAHandler::NSPopStr() no string." );
        lua_error( state );
    }

    lua_pushstring( state, lh->sh->readStr() );

    return 1;
}

int NSPopStrRef(lua_State *state)
{
    lua_getglobal( state, ONS_LUA_HANDLER_PTR );
    LUAHandler *lh = (LUAHandler*)lua_topointer( state, -1 );

    int val = lh->sh->getEndStatus();
    if (val & ScriptHandler::END_COMMA && !(val & ScriptHandler::END_COMMA_READ)){
        lua_pushstring( state, "LUAHandler::NSPopStrRef() no string variable." );
        lua_error( state );
    }

    lh->sh->readVariable();
    if (lh->sh->current_variable.type != ScriptHandler::VAR_STR){
        lua_pushstring( state, "LUAHandler::NSPopStrRef() no string variable." );
        lua_error( state );
    }
        
    lua_pushnumber( state, lh->sh->current_variable.var_no );

    return 1;
}

int NSPopLabel(lua_State *state)
{
    lua_getglobal( state, ONS_LUA_HANDLER_PTR );
    LUAHandler *lh = (LUAHandler*)lua_topointer( state, -1 );

    int val = lh->sh->getEndStatus();
    if (val & ScriptHandler::END_COMMA && !(val & ScriptHandler::END_COMMA_READ)){
        lua_pushstring( state, "LUAHandler::NSPopLabel() no label." );
        lua_error( state );
    }

    const char *str = lh->sh->readLabel();
    if (str[0] != '*'){
        lua_pushstring( state, "LUAHandler::NSPopLabel() no label." );
        lua_error( state );
    }
        
    lua_pushstring( state, str+1 );

    return 1;
}

int NSPopID(lua_State *state)
{
    lua_getglobal( state, ONS_LUA_HANDLER_PTR );
    LUAHandler *lh = (LUAHandler*)lua_topointer( state, -1 );

    int val = lh->sh->getEndStatus();
    if (val & ScriptHandler::END_COMMA && !(val & ScriptHandler::END_COMMA_READ)){
        lua_pushstring( state, "LUAHandler::NSPopID() no ID." );
        lua_error( state );
    }

    lua_pushstring( state, lh->sh->readLabel() );

    return 1;
}

int NSPopComma(lua_State *state)
{
    lua_getglobal( state, ONS_LUA_HANDLER_PTR );
    LUAHandler *lh = (LUAHandler*)lua_topointer( state, -1 );

    int val = lh->sh->getEndStatus();
    if (!(val & ScriptHandler::END_COMMA) || val & ScriptHandler::END_COMMA_READ){
        lua_pushstring( state, "LUAHandler::NSPopComma() no comma." );
        lua_error( state );
    }
    
    lh->sh->setEndStatus( ScriptHandler::END_COMMA_READ );
    
    return 0;
}

int NSCheckComma(lua_State *state)
{
    lua_getglobal( state, ONS_LUA_HANDLER_PTR );
    LUAHandler *lh = (LUAHandler*)lua_topointer( state, -1 );

    int val = lh->sh->getEndStatus();
    if (val & ScriptHandler::END_COMMA && !(val & ScriptHandler::END_COMMA_READ))
        lua_pushboolean( state, 1 );
    else
        lua_pushboolean( state, 0 );
        
    return 1;
}

int NSSetIntValue(lua_State *state)
{
    lua_getglobal( state, ONS_LUA_HANDLER_PTR );
    LUAHandler *lh = (LUAHandler*)lua_topointer( state, -1 );

    int no  = luaL_checkint( state, 1 );
    int val = luaL_checkint( state, 2 );
    
    lh->sh->setNumVariable( no, val );
    
    return 0;
}

int NSSetStrValue(lua_State *state)
{
    lua_getglobal( state, ONS_LUA_HANDLER_PTR );
    LUAHandler *lh = (LUAHandler*)lua_topointer( state, -1 );

    int no = luaL_checkint( state, 1 );
    const char *str = luaL_checkstring( state, 2 );
    
    if (lh->sh->getVariableData(no).str)
        delete[] lh->sh->getVariableData(no).str;
    lh->sh->getVariableData(no).str = NULL;
    
    if (str){
        lh->sh->getVariableData(no).str = new char[strlen(str) + 1];
        strcpy(lh->sh->getVariableData(no).str, str);
    }
    
    return 0;
}

int NSGetIntValue(lua_State *state)
{
    lua_getglobal( state, ONS_LUA_HANDLER_PTR );
    LUAHandler *lh = (LUAHandler*)lua_topointer( state, -1 );

    int no = luaL_checkint( state, 1 );
    
    lua_pushnumber( state, lh->sh->getVariableData(no).num );
    
    return 1;
}

int NSGetStrValue(lua_State *state)
{
    lua_getglobal( state, ONS_LUA_HANDLER_PTR );
    LUAHandler *lh = (LUAHandler*)lua_topointer( state, -1 );

    int no = luaL_checkint( state, 1 );
    
    lua_pushstring( state, lh->sh->getVariableData(no).str );
    
    return 1;
}

int NSExec(lua_State *state)
{
    lua_getglobal(state, ONS_LUA_HANDLER_PTR);
    LUAHandler *lh = (LUAHandler*)lua_topointer(state, -1);
    
    strcpy(cmd_buf, lua_tostring(state, 1));
    //printf("NSExec [%s]\n", cmd_buf);
    
    lh->sh->enterExternalScript(cmd_buf);
    lh->ons->runScript();
    lh->sh->leaveExternalScript();

    return 0;
}

int NSGoto(lua_State *state)
{
    lua_getglobal(state, ONS_LUA_HANDLER_PTR);
    LUAHandler *lh = (LUAHandler*)lua_topointer(state, -1);
    
    const char *str = luaL_checkstring( state, 1 );
    lh->ons->setCurrentLabel( str+1 );

    return 0;
}

int NSGosub(lua_State *state)
{
    lua_getglobal(state, ONS_LUA_HANDLER_PTR);
    LUAHandler *lh = (LUAHandler*)lua_topointer(state, -1);
    
    const char *str = luaL_checkstring( state, 1 );
    lh->ons->gosubReal( str+1, lh->sh->getNext() );

    return 0;
}

int NSReturn(lua_State *state)
{
    lua_getglobal(state, ONS_LUA_HANDLER_PTR);
    LUAHandler *lh = (LUAHandler*)lua_topointer(state, -1);
    
    lh->ons->returnCommand();

    return 0;
}

int NSLuaAnimationInterval(lua_State *state)
{
    lua_getglobal(state, ONS_LUA_HANDLER_PTR);
    LUAHandler *lh = (LUAHandler*)lua_topointer(state, -1);

    int val = lua_tointeger(state, 1);
    
    lh->duration_time = val;

    return 0;
}

int NSLuaAnimationMode(lua_State *state)
{
    lua_getglobal(state, ONS_LUA_HANDLER_PTR);
    LUAHandler *lh = (LUAHandler*)lua_topointer(state, -1);

    int val = lua_toboolean(state, 1);
    
    lh->is_animatable = (val==1);

    return 0;
}

int NSGetClick(lua_State *state)
{
    lua_getglobal(state, ONS_LUA_HANDLER_PTR);
    LUAHandler *lh = (LUAHandler*)lua_topointer(state, -1);

    ONScripter::ButtonState &bs = lh->ons->getCurrentButtonState();

    if (bs.event_type == SDL_MOUSEBUTTONUP && bs.event_button == SDL_BUTTON_LEFT)
        lua_pushboolean( state, true );
    else
        lua_pushboolean( state, false );

    if (bs.event_type == SDL_MOUSEBUTTONUP && bs.event_button == SDL_BUTTON_RIGHT)
        lua_pushboolean( state, true );
    else
        lua_pushboolean( state, false );

#if SDL_VERSION_ATLEAST(1, 2, 5)
    if (bs.event_button == SDL_BUTTON_WHEELUP)
        lua_pushinteger( state, 1 );
    else if (bs.event_button == SDL_BUTTON_WHEELDOWN)
        lua_pushinteger( state, -1 );
    else
#endif
        lua_pushinteger( state, 0 );

    if (bs.event_type == SDL_MOUSEBUTTONDOWN && bs.event_button == SDL_BUTTON_LEFT)
        lua_pushboolean( state, true );
    else
        lua_pushboolean( state, false );

    if (bs.event_type == SDL_MOUSEBUTTONDOWN && bs.event_button == SDL_BUTTON_RIGHT)
        lua_pushboolean( state, true );
    else
        lua_pushboolean( state, false );

    bs.event_type = 0;
    bs.event_button = 0;

    return 5;
}

int NSGetMouse(lua_State *state)
{
    lua_getglobal(state, ONS_LUA_HANDLER_PTR);
    LUAHandler *lh = (LUAHandler*)lua_topointer(state, -1);

    ONScripter::ButtonState bs = lh->ons->getCurrentButtonState();
    
    if (bs.x == lh->ons->getWidth() && bs.y == lh->ons->getHeight()){
        lua_pushinteger( state, -1 );
        lua_pushinteger( state, -1 );
    }
    else{
        lua_pushinteger( state, bs.x*lh->screen_ratio2/lh->screen_ratio1 );
        lua_pushinteger( state, bs.y*lh->screen_ratio2/lh->screen_ratio1 );
    }

    return 2;
}

int NSGetSkip(lua_State *state)
{
    lua_getglobal(state, ONS_LUA_HANDLER_PTR);
    LUAHandler *lh = (LUAHandler*)lua_topointer(state, -1);
    
    lua_pushinteger( state, lh->ons->getSkip() );

    return 1;
}

int NSGetWindowSize(lua_State *state)
{
    lua_getglobal(state, ONS_LUA_HANDLER_PTR);
    LUAHandler *lh = (LUAHandler*)lua_topointer(state, -1);

    lua_pushinteger( state, lh->ons->getWidth() );
    lua_pushinteger( state, lh->ons->getHeight() );

    return 2;
}

int NSDoEvents(lua_State *state)
{
    lua_getglobal(state, ONS_LUA_HANDLER_PTR);
    LUAHandler *lh = (LUAHandler*)lua_topointer(state, -1);
    
    sprintf(cmd_buf, "wait 0");
    lh->sh->enterExternalScript(cmd_buf);
    lh->ons->runScript();
    lh->sh->leaveExternalScript();

    lua_pushboolean( state, false );

    return 1;
}

int NSTimer(lua_State *state)
{
    lua_getglobal(state, ONS_LUA_HANDLER_PTR);
    lua_pushinteger( state, SDL_GetTicks() );

    return 1;
}

int NSGetKey(lua_State *state)
{
    lua_getglobal(state, ONS_LUA_HANDLER_PTR);
    LUAHandler *lh = (LUAHandler*)lua_topointer(state, -1);

    const char *str = luaL_checkstring( state, 1 );
    ONScripter::ButtonState bs = lh->ons->getCurrentButtonState();
    
    if ( strcmp(str, bs.str) == 0 || 
        (strcmp(str, "ESC") == 0 && strcmp(bs.str, "RCLICK") == 0))
        lua_pushboolean( state, 1 );
    else
        lua_pushboolean( state, 0 );

    return 1;
}

int NSSleep(lua_State *state)
{
    lua_getglobal(state, ONS_LUA_HANDLER_PTR);
    LUAHandler *lh = (LUAHandler*)lua_topointer(state, -1);

    int val = luaL_checkint( state, 1 );

    sprintf(cmd_buf, "wait %d", val);
    lh->sh->enterExternalScript(cmd_buf);
    lh->ons->runScript();
    lh->sh->leaveExternalScript();

    return 0;
}

int NSUpdate(lua_State *state)
{
    lua_getglobal(state, ONS_LUA_HANDLER_PTR);
    LUAHandler *lh = (LUAHandler*)lua_topointer(state, -1);

    sprintf(cmd_buf, "print 1");
    lh->sh->enterExternalScript(cmd_buf);
    lh->ons->runScript();
    lh->sh->leaveExternalScript();

    return 0;
}

int NSSpCell(lua_State *state)
{
    lua_getglobal(state, ONS_LUA_HANDLER_PTR);
    LUAHandler *lh = (LUAHandler*)lua_topointer(state, -1);

    int no = luaL_checkint( state, 1 );
    int cell = luaL_checkint( state, 2 );

    sprintf(cmd_buf, "cell %d, %d", no, cell);
    lh->sh->enterExternalScript(cmd_buf);
    lh->ons->runScript();
    lh->sh->leaveExternalScript();

    return 0;
}

int NSSpClear(lua_State *state)
{
    lua_getglobal(state, ONS_LUA_HANDLER_PTR);
    LUAHandler *lh = (LUAHandler*)lua_topointer(state, -1);

    int no = luaL_checkint( state, 1 );

    sprintf(cmd_buf, "csp %d", no);
    lh->sh->enterExternalScript(cmd_buf);
    lh->ons->runScript();
    lh->sh->leaveExternalScript();

    return 0;
}

int NSSpGetInfo(lua_State *state)
{
    lua_getglobal(state, ONS_LUA_HANDLER_PTR);
    LUAHandler *lh = (LUAHandler*)lua_topointer(state, -1);

    int no = luaL_checkint( state, 1 );

    AnimationInfo *ai = lh->ons->getSpriteInfo(no);

    lua_pushinteger( state, ai->orig_pos.w / ai->num_of_cells );
    lua_pushinteger( state, ai->orig_pos.h );
    lua_pushinteger( state, ai->num_of_cells );

    return 3;
}

int NSSpGetPos(lua_State *state)
{
    lua_getglobal(state, ONS_LUA_HANDLER_PTR);
    LUAHandler *lh = (LUAHandler*)lua_topointer(state, -1);

    int no = luaL_checkint( state, 1 );

    AnimationInfo *ai = lh->ons->getSpriteInfo(no);

    lua_pushinteger( state, ai->orig_pos.x );
    lua_pushinteger( state, ai->orig_pos.y );
    lua_pushinteger( state, ai->trans );

    return 3;
}

int NSSpLoad(lua_State *state)
{
    lua_getglobal(state, ONS_LUA_HANDLER_PTR);
    LUAHandler *lh = (LUAHandler*)lua_topointer(state, -1);

    int no = luaL_checkint( state, 1 );
    const char *str = luaL_checkstring( state, 2 );

    sprintf(cmd_buf, "lsp %d, \"%s\", %d, 0", no, str, lh->ons->getWidth()+1);
    lh->sh->enterExternalScript(cmd_buf);
    lh->ons->runScript();
    lh->sh->leaveExternalScript();

    return 0;
}

int NSSpMove(lua_State *state)
{
    lua_getglobal(state, ONS_LUA_HANDLER_PTR);
    LUAHandler *lh = (LUAHandler*)lua_topointer(state, -1);

    int no = luaL_checkint( state, 1 );
    int x  = luaL_checkint( state, 2 );
    int y  = luaL_checkint( state, 3 );
    int alpha = luaL_checkint( state, 4 );

    sprintf(cmd_buf, "amsp %d, %d, %d, %d", no, x, y, alpha);
    lh->sh->enterExternalScript(cmd_buf);
    lh->ons->runScript();
    lh->sh->leaveExternalScript();

    return 0;
}

int NSSpVisible(lua_State *state)
{
    lua_getglobal(state, ONS_LUA_HANDLER_PTR);
    LUAHandler *lh = (LUAHandler*)lua_topointer(state, -1);

    int no = luaL_checkint( state, 1 );
    int v  = lua_toboolean( state, 2 );

    sprintf(cmd_buf, "vsp %d, %d", no, v);
    lh->sh->enterExternalScript(cmd_buf);
    lh->ons->runScript();
    lh->sh->leaveExternalScript();

    return 0;
}

int NSSp2GetInfo(lua_State *state)
{
    lua_getglobal(state, ONS_LUA_HANDLER_PTR);
    LUAHandler *lh = (LUAHandler*)lua_topointer(state, -1);

    int no = luaL_checkint( state, 1 );

    AnimationInfo *ai = lh->ons->getSprite2Info(no);

    lua_pushinteger( state, ai->orig_pos.w / ai->num_of_cells );
    lua_pushinteger( state, ai->orig_pos.h );
    lua_pushinteger( state, ai->num_of_cells );

    return 3;
}

int NSSp2GetPos(lua_State *state)
{
    lua_getglobal(state, ONS_LUA_HANDLER_PTR);
    LUAHandler *lh = (LUAHandler*)lua_topointer(state, -1);

    int no = luaL_checkint( state, 1 );

    AnimationInfo *ai = lh->ons->getSprite2Info(no);

    lua_pushinteger( state, ai->orig_pos.x );
    lua_pushinteger( state, ai->orig_pos.y );
    lua_pushinteger( state, ai->scale_x );
    lua_pushinteger( state, ai->scale_y );
    lua_pushinteger( state, ai->rot );
    lua_pushinteger( state, ai->trans );
    lua_pushinteger( state, ai->blending_mode );

    return 7;
}

int NSSp2Load(lua_State *state)
{
    lua_getglobal(state, ONS_LUA_HANDLER_PTR);
    LUAHandler *lh = (LUAHandler*)lua_topointer(state, -1);

    int no = luaL_checkint( state, 1 );
    const char *str = luaL_checkstring( state, 2 );

    sprintf(cmd_buf, "lsp2 %d, \"%s\", %d, 0, 100, 100, 0", no, str, lh->ons->getWidth()*2);
    lh->sh->enterExternalScript(cmd_buf);
    lh->ons->runScript();
    lh->sh->leaveExternalScript();

    return 0;
}

int NSSp2Move(lua_State *state)
{
    lua_getglobal(state, ONS_LUA_HANDLER_PTR);
    LUAHandler *lh = (LUAHandler*)lua_topointer(state, -1);

    int no = luaL_checkint( state, 1 );
    int x  = luaL_checkint( state, 2 );
    int y  = luaL_checkint( state, 3 );
    int sx = luaL_checkint( state, 4 );
    int sy = luaL_checkint( state, 5 );
    int r  = luaL_checkint( state, 6 );
    int alpha = luaL_checkint( state, 7 );
    int opt = luaL_checkint( state, 8 ); // opt is not handled properly yet

    sprintf(cmd_buf, "amsp2 %d, %d, %d, %d, %d, %d, %d", no, x, y, sx, sy, r, alpha);
    lh->sh->enterExternalScript(cmd_buf);
    lh->ons->runScript();
    lh->sh->leaveExternalScript();

    return 0;
}

#define LUA_FUNC_LUT(s) {#s, s}
static const struct luaL_Reg lua_lut[] = {
    LUA_FUNC_LUT(NL_dofile),
    LUA_FUNC_LUT(NSPopInt),
    LUA_FUNC_LUT(NSPopIntRef),
    LUA_FUNC_LUT(NSPopStr),
    LUA_FUNC_LUT(NSPopStrRef),
    LUA_FUNC_LUT(NSPopLabel),
    LUA_FUNC_LUT(NSPopID),
    LUA_FUNC_LUT(NSPopComma),
    LUA_FUNC_LUT(NSCheckComma),
    LUA_FUNC_LUT(NSSetIntValue),
    LUA_FUNC_LUT(NSSetStrValue),
    LUA_FUNC_LUT(NSGetIntValue),
    LUA_FUNC_LUT(NSGetStrValue),
    LUA_FUNC_LUT(NSExec),
    LUA_FUNC_LUT(NSGoto),
    LUA_FUNC_LUT(NSGosub),
    LUA_FUNC_LUT(NSReturn),
    LUA_FUNC_LUT(NSLuaAnimationInterval),
    LUA_FUNC_LUT(NSLuaAnimationMode),
    LUA_FUNC_LUT(NSGetClick),
    LUA_FUNC_LUT(NSGetMouse),
    LUA_FUNC_LUT(NSGetSkip),
    LUA_FUNC_LUT(NSGetWindowSize),
    LUA_FUNC_LUT(NSDoEvents),
    LUA_FUNC_LUT(NSTimer),
    LUA_FUNC_LUT(NSGetKey),
    LUA_FUNC_LUT(NSSleep),
    LUA_FUNC_LUT(NSUpdate),
    LUA_FUNC_LUT(NSSpCell),
    LUA_FUNC_LUT(NSSpClear),
    LUA_FUNC_LUT(NSSpGetInfo),
    LUA_FUNC_LUT(NSSpGetPos),
    LUA_FUNC_LUT(NSSpLoad),
    LUA_FUNC_LUT(NSSpMove),
    LUA_FUNC_LUT(NSSpVisible),
    LUA_FUNC_LUT(NSSp2GetInfo),
    LUA_FUNC_LUT(NSSp2GetPos),
    LUA_FUNC_LUT(NSSp2Load),
    LUA_FUNC_LUT(NSSp2Move),
    {NULL, NULL}
};

//LUAHandler::LUAHandler(ONScripter *ons)
LUAHandler::LUAHandler()
{
    state = NULL;

    is_animatable = false;
    duration_time  = 15;
    remaining_time = 15;

    screen_ratio1 = 1;
    screen_ratio2 = 1;
    error_str[0] = 0;
    
    for (unsigned int i=0 ; i<MAX_CALLBACK ; i++)
        callback_state[i] = false;
}

LUAHandler::~LUAHandler()
{
    if (state) lua_close(state);
}

void LUAHandler::init(ONScripter *ons, ScriptHandler *sh,
                      int screen_ratio1, int screen_ratio2)
{
    this->ons = ons;
    this->sh = sh;
    
    state = luaL_newstate();
    luaL_openlibs(state);

#if LUA_VERSION_NUM >= 502
    lua_pushglobaltable(state);
    luaL_setfuncs(state, lua_lut, 0);
#else
    lua_pushvalue(state, LUA_GLOBALSINDEX);
    luaL_register(state, NULL, lua_lut);
#endif
    
    lua_pushlightuserdata(state, this);
    lua_setglobal(state, ONS_LUA_HANDLER_PTR);
}

void LUAHandler::loadInitScript()
{
    unsigned long length = sh->cBR->getFileLength(INIT_SCRIPT);
    if (length == 0){
        printf("cannot open %s\n", INIT_SCRIPT);
        return;
    }

    unsigned char *buffer = new unsigned char[length+1];
    int location;
    sh->cBR->getFile(INIT_SCRIPT, buffer, &location);
    buffer[length] = 0;

    unsigned char *buffer2 = new unsigned char[length*3/2];
    unsigned char *p = buffer;
    unsigned char *p2 = buffer2;
    while(*p){
        if (IS_TWO_BYTE(*p)){
            *p2++ = *p++;
            if (*p == '\\') *p2++ = '\\';
        }
        *p2++ = *p++;
    }

    if (luaL_loadbuffer(state, (const char*)buffer2, p2 - buffer2, INIT_SCRIPT) || 
        lua_pcall(state, 0, 0, 0)){
        printf("cannot parse %s %s\n", INIT_SCRIPT, lua_tostring(state,-1));
    }

    delete[] buffer;
    delete[] buffer2;
}

void LUAHandler::addCallback(const char *label)
{
    if (strcmp(label, "tag") == 0)
        callback_state[LUA_TAG] = true;
    if (strcmp(label, "text0") == 0)
        callback_state[LUA_TEXT0] = true;
    if (strcmp(label, "text") == 0)
        callback_state[LUA_TEXT] = true;
    if (strcmp(label, "animation") == 0)
        callback_state[LUA_ANIMATION] = true;
    if (strcmp(label, "close") == 0)
        callback_state[LUA_CLOSE] = true;
    if (strcmp(label, "end") == 0)
        callback_state[LUA_END] = true;
    if (strcmp(label, "savepoint") == 0)
        callback_state[LUA_SAVEPOINT] = true;
    if (strcmp(label, "save") == 0)
        callback_state[LUA_SAVE] = true;
    if (strcmp(label, "load") == 0)
        callback_state[LUA_LOAD] = true;
    if (strcmp(label, "reset") == 0)
        callback_state[LUA_RESET] = true;
}

int LUAHandler::callFunction(bool is_callback, const char *cmd)
{
    char cmd2[256];
    
    if (is_callback)
        sprintf(cmd2, "NSCALL_%s", cmd);
    else
        sprintf(cmd2, "NSCOM_%s", cmd);

    int num_return_value = 0;
    if (strcmp(cmd2, "NSCALL_animation") == 0)
        num_return_value = 1;
        
    lua_getglobal(state, cmd2);

    if (lua_pcall(state, 0, num_return_value, 0) != 0){
        strcpy( error_str, lua_tostring(state, -1) );
        return -1;
    }

    if (strcmp(cmd2, "NSCALL_animation") == 0){
        if (lua_isboolean(state, -1) && lua_toboolean(state, -1)){
            sprintf(cmd2, "NSUpdate");
            lua_getglobal(state, cmd2);
            if (lua_pcall(state, 0, 0, 0) != 0){
                strcpy( error_str, lua_tostring(state, -1) );
                return -1;
            }
        }
    }

    return 0;
}

bool LUAHandler::isCallbackEnabled(int val)
{
    return callback_state[val];
}
