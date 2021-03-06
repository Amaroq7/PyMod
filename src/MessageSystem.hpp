/*
 *  Copyright (C) 2018 SPMod Development Team
 *
 *  This file is part of SPMod.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include "spmod.hpp"

class Message : public IMessage
{
public:
    struct Param
    {
        MsgParamType type;
        std::variant<int, float, std::string> data;
    };

    Message() = default;
    ~Message() = default;

    // IMessage
    std::size_t getParams() const override;
    MsgParamType getParamType(std::size_t index) const override;

    int getParamInt(std::size_t index) const override;
    float getParamFloat(std::size_t index) const override;
    std::string_view getParamString(std::size_t index) const override;

    void setParamInt(std::size_t index, int value) override;
    void setParamFloat(std::size_t index, float value) override;
    void setParamString(std::size_t index, std::string_view string) override;

    MsgDest getDest() const override;
    int getType() const override;
    const float *getOrigin() const override;
    Engine::IEdict *getEdict() const override;

    // Message
    void init(MsgDest dest, int type, const float *origin, Engine::Edict *edict);

    void exec() const;

    void clearParams();

    void addParam(MsgParamType type, std::variant<int, float, std::string> &&data);

    void setParam(std::size_t index, std::variant<int, float, std::string> &&data);

    template<typename T>
    const T &getParam(std::size_t index) const
    {
        try
        {
            return std::get<T>(m_params[index].data);
        }
        catch (const std::bad_variant_access &e [[maybe_unused]])
        {
            static T empty;
            return empty;
        }
    }

private:
    MsgDest m_dest;
    int m_type;
    Vector m_origin;
    Engine::Edict *m_edict;

    std::vector<Param> m_params;
};

class MessageHook final : public IMessageHook
{
public:
    MessageHook(int msgType, Message::Handler handler, HookType hookType);
    ~MessageHook() = default;

    // IMessageHook
    void enable() override;
    void disable() override;
    bool isActive() const override;
    int getMsgType() const override;

    // MessageHook
    HookType getHookType() const;
    Message::Handler getHandler() const;
    bool getActive() const;

private:
    int m_msgType;
    Message::Handler m_handler;
    HookType m_hookType;
    bool m_active;
};

class MessageHooks
{
public:
    MessageHooks() = default;
    ~MessageHooks() = default;

    MessageHook *addHook(int msgType, Message::Handler handler, HookType hookType);

    void removeHook(IMessageHook *hook);

    IForward::ReturnValue exec(const std::unique_ptr<Message> &message, HookType hookType) const;

    bool hasHooks() const;

    void clearHooks();

private:
    std::vector<std::unique_ptr<MessageHook>> m_handlers;
};

class MessageMngr : public IMessageMngr
{
public:
    MessageMngr();
    ~MessageMngr() = default;

    // IMessageMngr
    MessageHook *registerHook(int msgType, Message::Handler handler, HookType hookType) override;
    void unregisterHook(IMessageHook *) override;

    MsgBlockType getMessageBlock(int msgType) const override;
    void setMessageBlock(int msgType, MsgBlockType blockType) override;

    IForward::ReturnValue execHandlers(HookType hookType);

    IMessage *getMessage() const override
    {
        return m_message.get();
    }

    bool inHook() const override;

    void clearMessages();

    META_RES MessageBegin(int msg_dest, int msg_type, const float *pOrigin, edict_t *ed);

    META_RES MessageEnd();

    template<typename T>
    META_RES WriteParam(MsgParamType type, T value)
    {
        if (m_inblock)
        {
            return MRES_SUPERCEDE;
        }
        else if (m_inhook)
        {
            m_message->addParam(type, value);
            return MRES_SUPERCEDE;
        }
        return MRES_IGNORED;
    }

private:
    std::unique_ptr<Message> m_message;
    std::array<MessageHooks, MAX_USER_MESSAGES> m_hooks;
    std::array<MsgBlockType, MAX_USER_MESSAGES> m_blocks;

    bool m_inhook;
    bool m_inblock;
    int m_msgType;
};
