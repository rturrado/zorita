#pragma once

/// @brief Version information.
namespace version {

static constexpr const char *version{"0.0.1"};
static constexpr const char *release_year{"2026"};

/// @brief Returns the application version string.
[[nodiscard]] [[maybe_unused]] static const char *get_version() {
  return version;
}

/// @brief Returns the release year string.
[[nodiscard]] [[maybe_unused]] static const char *get_release_year() {
  return release_year;
}

} // namespace version
