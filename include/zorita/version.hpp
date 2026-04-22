#pragma once

namespace version {

static const char* version{ "0.0.1" };
static const char* release_year{ "2026" };

[[nodiscard]] [[maybe_unused]] static const char* get_version() {
    return version;
}

[[nodiscard]] [[maybe_unused]] static const char* get_release_year() {
    return release_year;
}

}  // namespace version
