# The Headcrab Lua API

In an effort to preserve my sanity, I will be documenting the Lua API for Headcrab here. This should serve as a reference for anybody who wishes to write scripts and should prevent the structure from getting out of hand during development.

## hc.peek

Reads a value from the process's memory.

```Lua
local value = hc.peek(hc.type.u32, 0x7ffc521276e0)
```

## hc.platform

Information about the architecture and operating system.

### hc.platform.bits

Used to determine the "width" of a processor(e.g. 32 or 64-bit).

```Lua
print(string.format("We are running on a %d-bit processor", hc.platform.bits))
```

### hc.platform.os

A string stating which operating system is currently being used. Possible values are currently `Windows`, `macOS`, `Linux`, and `BSD`.

```Lua
print(string.format("Hello from %s!", hc.platform.os))
```

## hc.poke

Modifies a portion of the process's memory.

```Lua
hc.poke(hc.type.u32, 0x7ffc521276e0, 0xdeadbeef)
```

## hc.process

Information about the process that the script has been injected into.

### hc.process.id

The process ID of the program

```Lua
print(string.format("Injected into PID %d!", hc.process.id))
```

## hc.type

Data types for interfacing with memory directly(See hc.peek and hc.poke above).

| Headcrab Type | Equivalent C Type | Description                                     |
| ------------- | ----------------- | ----------------------------------------------- |
| `hc.type.f32` | `float`           | Single-Precision IEEE 754 Floating Point Number |
| `hc.type.f64` | `double`          | Double-Precision IEEE 754 Floating Point Number |
| `hc.type.i8`  | `signed char`     | Signed 8-bit Integer                            |
| `hc.type.i16` | `signed short`    | Signed 16-bit Integer                           |
| `hc.type.i32` | `signed int`      | Signed 32-bit Integer                           |
| `hc.type.i64` | `signed long`     | Signed 64-bit Integer                           |
| `hc.type.u8`  | `unsigned char`   | Unsigned 8-bit Integer                          |
| `hc.type.u16` | `unsigned short`  | Unsigned 16-bit Integer                         |
| `hc.type.u32` | `unsigned int`    | Unsigned 32-bit Integer                         |
| `hc.type.u64` | `unsigned long`   | Unsigned 64-bit Integer                         |

## hc.version

The current version of the base Headcrab API and any third-party modifications that may be present.

### hc.version.major

The major version of the base Headcrab API. If the major version is not what is expected, assume that the entire API is incompatible with the script and throw an error.

```Lua
-- This script will only function if the API version is 1.x
assert(hc.version.major == 1, "Major version mismatch! Script is incompatible!")
```

### hc.version.minor

The minor version of the base Headcrab API. If the minor version is less than the minimum required to run the script, assume that the script is incompatible with that version of the API.

```Lua
-- Make sure we're running Headcrab version 4.20+
assert(hc.version.major == 4, "Major version mismatch! Script is incompatible!")
assert(hc.version.minor >= 20, "Expected minor version 20 or higher. Script is incompatible!")
```

### hc.version.mod

Declares any third-party additions to the base API. The structure is a dictionary of modification names containing both a major and minor version. If a mod isn't present here, it's safe to assume that it's not supported.

```Lua
if hc.version.mod.hashbrowns ~= nil then
    print(string.format("Currently running hashbrowns version %d.%d", hc.version.mod.hashbrowns.major, hc.version.mod.hashbrowns.minor))
else
    error("This version of headcrab doesn't support the hashbrowns extension!")
end
```