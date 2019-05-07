// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

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

#include <sniper/pico/picohttpparser.h>
#include <sniper/strings/ascii_case.h>
#include <sniper/strings/atoi.h>
#include "Response.h"

namespace sniper::pico {

void Response::clear() noexcept
{
    status = -1;
    header_size = 0;
    content_length = 0;
    keep_alive = false;
    headers.clear();
}

ParseResult Response::parse(char* data, size_t size) noexcept
{
    if (!data || !size)
        return ParseResult::Err;

    if (size < 5)
        return ParseResult::Partial;

    struct phr_header pico_headers[MAX_HEADERS];
    size_t num_headers = sizeof(pico_headers) / sizeof(headers[0]);

    int pico_minor_version = -1;

    const char* msg = nullptr;
    size_t msg_len = 0;
    int ssize =
        phr_parse_response(data, size, &pico_minor_version, &status, &msg, &msg_len, pico_headers, &num_headers, 0);

    if (ssize > 0) {
        header_size = ssize;

        if (pico_minor_version == 1)
            keep_alive = true;

        bool content_length_found = false;
        bool connection_found = false;

        for (unsigned i = 0; i < num_headers; i++) {
            strings::to_lower_ascii(const_cast<char*>(pico_headers[i].name), pico_headers[i].name_len);
            strings::to_lower_ascii(const_cast<char*>(pico_headers[i].value), pico_headers[i].value_len);

            string_view key(pico_headers[i].name, pico_headers[i].name_len);
            string_view val(pico_headers[i].value, pico_headers[i].value_len);

            // content-length
            if (!content_length_found && key == "content-length") {
                content_length_found = true;
                if (auto len = strings::fast_atoi64(val); len)
                    content_length = *len;
                else
                    return ParseResult::Err;
            }

            // connection
            if (!connection_found && key == "connection") {
                connection_found = true;

                if (pico_minor_version == 0 && val == "keep-alive")
                    keep_alive = true;
                else if (val == "close")
                    keep_alive = false;
            }

            headers.emplace_back(key, val);
        }

        return ParseResult::Complete;
    }
    else if (ssize == -2) {
        return ParseResult::Partial;
    }
    else {
        return ParseResult::Err;
    }
}

} // namespace sniper::pico
