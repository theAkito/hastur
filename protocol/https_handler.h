// SPDX-FileCopyrightText: 2022 Robin Lindén <dev@robinlinden.eu>
//
// SPDX-License-Identifier: BSD-2-Clause

#ifndef PROTOCOL_HTTPS_HANDLER_H_
#define PROTOCOL_HTTPS_HANDLER_H_

#include "protocol/iprotocol_handler.h"

#include <optional>
#include <string>
#include <utility>

namespace protocol {

class HttpsHandler final : public IProtocolHandler {
public:
    explicit HttpsHandler(std::optional<std::string> user_agent) : user_agent_{std::move(user_agent)} {}

    [[nodiscard]] Response handle(uri::Uri const &) override;

private:
    std::optional<std::string> user_agent_;
};

} // namespace protocol

#endif
