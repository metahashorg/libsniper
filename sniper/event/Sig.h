/*
 * Copyright (c) 2018 - 2019, MetaHash, Oleg Romanenko (oleg@romanenko.ro)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <sniper/event/Loop.h>
#include <sniper/std/functional.h>
#include <utility>

namespace sniper::event {

class Sig final
{
public:
    explicit Sig(loop_ptr loop, int signum, function<void(const loop_ptr& loop)>&& cb = {});
    ~Sig() noexcept;

private:
    void cb_sig(ev::sig& w, [[maybe_unused]] int revents) noexcept;

    loop_ptr _loop;
    ev::sig _w;
    function<void(const loop_ptr& loop)> _cb;
};

} // namespace sniper::event
