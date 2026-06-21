/// @file UniqueHandle.hpp
/// Move-only RAII wrapper for Windows HANDLE values.

#pragma once

#include <windows.h>

/// Move-only RAII wrapper for Windows HANDLE values.
/// Owns a Windows HANDLE and closes it with CloseHandle when destroyed.
/// Both nullptr and INVALID_HANDLE_VALUE are treated as invalid handles.
class UniqueHandle {
public:
    /// Creates a handle owner.
    /// @param handle Windows HANDLE to take ownership of.
    explicit UniqueHandle(HANDLE handle = nullptr) : handle_{handle} {}

    /// Closes the owned handle if it is valid.
    ~UniqueHandle() {
        close();
    }

    UniqueHandle(const UniqueHandle&) = delete;
    UniqueHandle& operator=(const UniqueHandle&) = delete;

    UniqueHandle(UniqueHandle&& other) noexcept : handle_{other.handle_} {
        other.handle_ = nullptr;
    }

    UniqueHandle& operator=(UniqueHandle&& other) noexcept {
        if (this != &other) {
            close();

            handle_ = other.handle_;
            other.handle_ = nullptr;
        }

        return *this;
    }

    /// Returns the raw Windows HANDLE without transferring ownership.
    /// @return Owned raw HANDLE value.
    HANDLE get() const {
        return handle_;
    }

    /// Checks whether the owned handle can be used.
    /// @return True if the handle is neither nullptr nor INVALID_HANDLE_VALUE.
    bool is_valid() const {
        return handle_ != nullptr && handle_ != INVALID_HANDLE_VALUE;
    }

private:
    /// Closes the owned handle and resets it to nullptr.
    void close() {
        if (is_valid()) {
            CloseHandle(handle_);
            handle_ = nullptr;
        }
    }

    HANDLE handle_{nullptr};
};