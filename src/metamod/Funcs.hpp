/*
 *  Copyright (C) 2018-2020 SPMod Development Team

 *  This file is part of SPMod.

 *  SPMod is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.

 *  SPMod is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with SPMod.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "../spmod.hpp"

namespace SPMod::Metamod
{
    class Funcs final : public IFuncs
    {
    public:
        Funcs() = default;
        Funcs(const Funcs &other) = delete;
        Funcs(Funcs &&other) = delete;
        ~Funcs() = default;

        std::uint32_t getUsrMsgId(std::string_view msgName) const override;
        std::string_view getUsrMsgName(std::uint32_t msgId) const override;
    };
} // namespace SPMod::Metamod
