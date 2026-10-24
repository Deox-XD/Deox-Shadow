# 🔥 Deox

<div align="center">

![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)
![Platform](https://img.shields.io/badge/platform-Windows-lightgrey.svg)
![Language](https://img.shields.io/badge/language-C%2B%2B-orange.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)

**Advanced Shadow Memory Region Research Framework**

*Leveraging undocumented Windows internals for stealth kernel communication*

[Features](#-features) • [Architecture](#-architecture) • [Installation](#-installation) • [Usage](#-usage) • [Disclaimer](#-disclaimer)

</div>

---

## 🎯 Overview

**Deox** is a cutting-edge research project demonstrating advanced Windows kernel communication techniques using shadow memory regions and syscall hooking. This framework implements a fully functional kernel driver and usermode application that communicate through a sophisticated memory manipulation architecture, bypassing traditional driver communication methods.

### 🔑 Key Highlights

- **Shadow PML4 Base Access** - Direct manipulation of page table base addresses
- **Syscall Hooking** - Hijacking `NtUserGetPointerProprietaryId` for stealthy communication
- **Zero Traces** - No registered IOCTLs, no device objects, no obvious footprint
- **Cross-Process Memory** - Read virtual memory from arbitrary processes
- **Game Ready** - Example implementation targeting Unreal Engine 4 applications

---

## ✨ Features

### Kernel Component
- 🛡️ **Stealthy Driver Entry** - Minimal footprint on system load
- 🔗 **Win32k Syscall Hooking** - Intercepts `NtUserGetPointerProprietaryId` 
- 📖 **Memory Read Operations** - Safe cross-process memory reading
- 🎯 **PML4 Base Resolution** - Direct page table base discovery
- 🔒 **No Traditional Driver Interface** - No IOCTL handlers or symbolic links

### Usermode Component
- 🚀 **Simple API** - Clean C++ interface for driver communication
- 💾 **Template-Based Memory Operations** - Type-safe read operations
- 🎮 **Game Engine Integration** - Example offsets for UE4 structures
- 🔄 **Continuous Monitoring** - Threaded cache updates
- 📊 **Debug Output** - Real-time operation logging

---

## 🏗️ Architecture

```
┌─────────────────────────────────────────────────────┐
│                   USERMODE (Ring 3)                  │
│  ┌──────────────────────────────────────────────┐  │
│  │          Deox Client Application             │  │
│  │  ┌────────────┐  ┌──────────────────────┐  │  │
│  │  │  driver.h  │  │  Game Memory Reader  │  │  │
│  │  └────────────┘  └──────────────────────┘  │  │
│  └──────────────────────────────────────────────┘  │
│                        │                             │
│                        │ NtUserGetPointerProprietaryId
│                        ▼                             │
├─────────────────────────────────────────────────────┤
│                   KERNEL (Ring 0)                    │
│  ┌──────────────────────────────────────────────┐  │
│  │              win32k.sys Hook                 │  │
│  │      ┌─────────────────────────────┐        │  │
│  │      │   hooked_function()         │        │  │
│  │      │   ├── Read Memory           │        │  │
│  │      │   ├── Get PML4 Base         │        │  │
│  │      │   └── Request Handler       │        │  │
│  │      └─────────────────────────────┘        │  │
│  └──────────────────────────────────────────────┘  │
│                        │                             │
│                        ▼                             │
│              Target Process Memory                   │
└─────────────────────────────────────────────────────┘
```

### Communication Flow

1. **Initialization**: Driver locates and hooks `NtUserGetPointerProprietaryId` in win32k.sys
2. **Request**: Usermode application calls hooked function with request structure
3. **Dispatch**: Kernel hook intercepts call and routes to request handler
4. **Execution**: Kernel performs privileged operations (memory read, PML4 access)
5. **Return**: Results written back to usermode structure

---

## 🛠️ Installation

### Prerequisites

- **Visual Studio 2019+** with C++ Desktop Development
- **Windows Driver Kit (WDK)** - Latest version
- **Windows 10/11** - x64 architecture
- **Administrator Privileges** - Required for driver loading

### Building

```bash
# Clone the repository
git clone https://github.com/yourusername/deox.git
cd deox

# Open in Visual Studio
start Miraka.sln

# Build Solution (Ctrl+Shift+B)
# - Kernel: Release x64
# - User: Release x64
```

### Deployment

```powershell
# Disable Driver Signature Enforcement (Test Mode)
bcdedit /set testsigning on

# Load the driver using your preferred method
# (Manual Mapper, DSE Bypass, or Vulnerable Driver)

# Run the usermode application
.\User.exe
```

---

## 🚀 Usage

### Basic Example

```cpp
#include "driver.h"

int main() {
    // Initialize driver communication
    auto targetPID = utils::getprocessid(L"target.exe");
    driver.initdriver(targetPID);
    
    // Get PML4 base address
    auto pml4 = driver.pml4_base();
    printf("[Deox] PML4: 0x%p\n", pml4);
    
    // Read memory
    auto value = driver.read<uintptr_t>(pml4 + 0x60);
    printf("[Deox] Value: 0x%p\n", value);
    
    return 0;
}
```

### Advanced Memory Operations

```cpp
// Read single value
auto health = driver.read<float>(player_address + 0x100);

// Read array
int inventory[10];
driver.readarray<int>(inventory_address, inventory, 10);

// Read structure
struct PlayerData {
    float x, y, z;
    int health;
};
auto player = driver.read<PlayerData>(player_address);
```

---

## 🔬 Technical Details

### Shadow Memory Regions

The project utilizes **shadow memory regions** by directly accessing the PML4 (Page Map Level 4) base address. This technique allows reading memory from any process without traditional APIs, making it extremely difficult to detect.

### Syscall Hooking Strategy

By hooking `NtUserGetPointerProprietaryId` - an obscure win32k.sys function - we establish a covert communication channel. This syscall is rarely monitored and provides a perfect entry point for our kernel driver.

### Request Structure

```cpp
struct _requests {
    uint32_t  src_pid;        // Source process ID
    uint64_t  src_addr;       // Source memory address
    uint64_t  dst_addr;       // Destination buffer
    size_t    size;           // Bytes to transfer
    int       request_key;    // Operation type
    uintptr_t pml4_base;      // Page table base
};
```

### Supported Operations

| Code | Operation | Description |
|------|-----------|-------------|
| `0x80000001` | `DRIVER_READVM` | Read virtual memory from target process |
| `0x80000002` | `DRIVER_PML4BASE` | Retrieve PML4 base address |
| `0x80000003` | `DRIVER_MOUSE` | Mouse input injection (commented) |

---

## 🎮 Game Integration Example

The usermode component includes example code for reading Unreal Engine 4 structures:

```cpp
// Example offsets (update for your target)
namespace offsets {
    DWORD uworld = 0x60;
    DWORD ulevel = 0x38;
    DWORD gamestate = 0x140;
}

// Continuous monitoring thread
void cachethread() {
    auto pml4base = driver.pml4_base();
    
    while (true) {
        auto uworld = driver.read<uintptr_t>(pml4base + offsets::uworld);
        auto ulevel = driver.read<uintptr_t>(uworld + offsets::ulevel);
        auto gamestate = driver.read<uintptr_t>(uworld + offsets::gamestate);
        
        // Process game data...
        Sleep(2000);
    }
}
```

---

## 🔒 Security Considerations

### Detection Vectors

- **PatchGuard** - May detect kernel memory modifications
- **Driver Signature Enforcement** - Requires test mode or bypass
- **Anti-Cheat Systems** - May detect syscall hooks or memory patterns
- **HVCI/VBS** - Will prevent driver loading on secured systems

### Mitigation Strategies

- Use runtime polymorphism for hook code
- Implement SSDT restoration on detection
- Employ kernel stack spoofing
- Consider hypervisor-based implementations

---

## 🛠️ Built With

<div align="center">

### Core Technologies

![C++](https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Windows](https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white)
![Visual Studio](https://img.shields.io/badge/Visual%20Studio-5C2D91?style=for-the-badge&logo=visual-studio&logoColor=white)

</div>

| Component | Technology | Purpose |
|-----------|-----------|---------|
| **Language** | C++17 | High-performance system programming |
| **Compiler** | MSVC 2019+ | Native Windows compilation |
| **Driver Kit** | Windows WDK | Kernel-mode driver development |
| **Architecture** | x64 | 64-bit Windows systems |
| **Build System** | MSBuild | Visual Studio project compilation |
| **Version Control** | Git | Source code management |

### Key Libraries & APIs

- **ntoskrnl.exe** - Core kernel functions
- **win32k.sys** - Window manager and GDI syscalls
- **win32u.dll** - Usermode syscall wrappers
- **Windows API** - Process and memory management

---

## 📚 Project Structure

```
Deox/
├── Kernel/                  # Driver component
│   ├── entry.cpp           # Driver entry point
│   ├── functions.h         # Request handlers
│   ├── utils.h             # Helper functions
│   ├── defines.h           # Structures & definitions
│   └── globals.h           # Global variables
│
├── User/                    # Usermode component
│   ├── main.cpp            # Application entry
│   ├── driver.h            # Driver interface class
│   ├── utils.h             # User utilities
│   └── offsets.h           # Game-specific offsets
│
└── README.md               # This file
```

---

## 🤝 Contributing

Contributions are welcome! This is a research project aimed at understanding Windows internals.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

---

## 📖 Resources & References

- [Windows Internals, Part 1 & 2](https://docs.microsoft.com/en-us/sysinternals/)
- [Syscall Hooking Techniques](https://www.unknowncheats.me/)
- [Intel® 64 Architecture Memory Management](https://software.intel.com/content/www/us/en/develop/articles/intel-sdm.html)
- [PatchGuard Analysis](https://github.com/tandasat/PatchGuard-Research)
- [Shadow Walker Memory Hiding](https://www.blackhat.com/presentations/bh-usa-05/bh-us-05-sparks-butler.pdf)

---

## ⚠️ Disclaimer

**FOR EDUCATIONAL AND RESEARCH PURPOSES ONLY**

This project is developed for security research and educational purposes. The techniques demonstrated here are intended to help security researchers, developers, and students understand Windows kernel internals and memory management.

**Important Notes:**

- ❌ **DO NOT** use this software for cheating in online games
- ❌ **DO NOT** use this software for malicious purposes
- ❌ **DO NOT** distribute modified versions for illegal activities
- ✅ **DO** use this for learning and legitimate security research
- ✅ **DO** use this in isolated test environments

The authors and contributors are **NOT responsible** for any misuse or damage caused by this software. Use at your own risk and always comply with your local laws and regulations.

By using this software, you agree to use it responsibly and ethically.

---

## 📜 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## 💬 Contact & Support

- **Issues**: [GitHub Issues](https://github.com/yourusername/deox/issues)
- **Discussions**: [GitHub Discussions](https://github.com/yourusername/deox/discussions)

---

<div align="center">

---

### 🚀 Built with Passion by **Deox**

*Pushing the boundaries of Windows kernel research*

<br>

**🔥 "In the shadows, we find the truth" 🔥**

<br>

[![GitHub stars](https://img.shields.io/github/stars/yourusername/deox?style=social)](https://github.com/yourusername/deox/stargazers)
[![GitHub forks](https://img.shields.io/github/forks/yourusername/deox?style=social)](https://github.com/yourusername/deox/network/members)
[![GitHub watchers](https://img.shields.io/github/watchers/yourusername/deox?style=social)](https://github.com/yourusername/deox/watchers)

<br>

⭐ **Star this repository if you found it interesting!** ⭐

<br>

![Deox](https://img.shields.io/badge/Deox-Research%20Framework-blueviolet?style=for-the-badge)
![Made with Love](https://img.shields.io/badge/Made%20with-❤️-red?style=for-the-badge)
![Windows Kernel](https://img.shields.io/badge/Windows-Kernel%20Research-blue?style=for-the-badge)

</div>
