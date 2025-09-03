# 📦 Dafny Installation Guide

This guide describes how to install **Dafny**, a formal verification-aware programming language, on your system.

## Build Dafny from Source

1. **Clone the repository:**

   ```bash
   git clone https://github.com/dafny-lang/dafny.git
   cd dafny
   ```

2. **Install .NET SDK 8.0.111**  
On ubuntu or debian
```
sudo apt update
sudo apt install -y dotnet-sdk-8.0
```
On macOS (with Homebrew):
```
brew install --cask dotnet-sdk
```

3. **Build Dafny:**

   ```bash
   make exe
   ```
---

## 🔹 Option 3: Use Dafny Online (No Install)

Try Dafny in your browser:  
🔗 https://dafny.org/dafny/

---

## 🔹 VS Code Extension (Optional)

1. Install **Visual Studio Code**
2. Go to Extensions (`Ctrl+Shift+X`)
3. Search for **"Dafny"** and install
4. Make sure `dafny` is on your system PATH

---

## 🧪 Test Your Installation

Create a file `hello.dfy`:

```dafny
method Main() {
  print "Hello, Dafny!\n";
}
```

Then run:

```bash
Scripts/dafny run hello.dfy
```

---

## 💡 Troubleshooting

- ❌ `dotnet: not found` → Install .NET SDK and ensure it's in your PATH.
- ❌ `The SDK version specified in global.json was not found` → Install matching .NET version or edit `global.json`.
