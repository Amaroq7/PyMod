/*
 *  Copyright (C) 2018-2020 SPMod Development Team
 *
 *  This file is part of SPMod.
 *
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

namespace SPMod
{
    class ITimer
    {
    public:
        virtual ~ITimer() = default;

        /**
         * @brief Timer callback
         *
         * @return  True to continue executing timer, false to remove.
         */
        using Callback = std::function<bool(ITimer *const timer)>;

        /**
         * @brief Gets interval.
         *
         * @return        Timer interval.
         */
        virtual float getInterval() const = 0;

        /**
         * @brief Checks if timer is paused.
         *
         * @return        True if paused, false otherwise.
         */
        virtual bool isPaused() const = 0;

        /**
         * @brief Sets new interval.
         *
         * @param newint  New interval to set.
         *
         * @noreturn
         */
        virtual void setInterval(float newint) = 0;

        /**
         * @brief Pauses or unpauses a timer.
         *
         * @param pause   True to pause timer, false to unpause it.
         *
         * @noreturn
         */
        virtual void setPause(bool pause) = 0;

        /**
         * @brief Executes a timer.
         *
         * @note If callback returns false, timer will be removed.
         *
         * @param timer       Timer to be executed.
         *
         * @noreturn
         */
        virtual bool exec() = 0;
    };

    class ITimerMngr : public ISPModInterface
    {
    public:
        static constexpr uint16_t MAJOR_VERSION = 0;
        static constexpr uint16_t MINOR_VERSION = 0;

        static constexpr uint32_t VERSION = (MAJOR_VERSION << 16 | MINOR_VERSION);

        /**
         * @brief Gets interface's name.
         *
         * @return              Interface's name.
         */
        std::string_view getName() const override
        {
            return "ITimerMngr";
        }

        virtual ~ITimerMngr() = default;

        /**
         * @brief Gets interface's version.
         *
         * @note The first 16 most significant bits represent major version, the rest represent minor version.
         *
         * @return      Interface's version.
         */
        uint32_t getVersion() const override
        {
            return VERSION;
        }

        /**
         * @brief Registers a new timer.
         *
         * @param interval    Time interval.
         * @param func        Callback function.
         * @param pause       True if timer should be paused after creation, false otherwise.
         *
         * @return            Created timer.
         */
        virtual ITimer *createTimer(float interval,
                                    ITimer::Callback func,
                                    bool pause = false) = 0;

        /**
         * @brief Removes a timer.
         *
         * @note  Removing timer in the callback should be done by returning false.
         *
         * @param timer       Timer to delete.
         *
         * @noreturn
         */
        virtual void removeTimer(const ITimer *timer) = 0;
    };
} // namespace SPMod