/*
 *  Copyright (C) 2018-2020 SPMod Development Team
 *
 *  This file is part of SPMod.
 *
 *  SPMod is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  SPMod is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SPMod.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "../spmod.hpp"

namespace SPMod::Engine
{
    class EntVars final : public IEntVars
    {
    public:
        EntVars() = delete;
        EntVars(entvars_t *entvars);
        ~EntVars() = default;

        // IEntVars
        EntFlags getFlags() const override;
        void setFlags(EntFlags flags) override;
        std::uint32_t getIndex() const override;

        // EntVars
        operator entvars_t *() const;

    private:
        entvars_t *m_entVars = nullptr;
    };
} // namespace SPMod::Engine