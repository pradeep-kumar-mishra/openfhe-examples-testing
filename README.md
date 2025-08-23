# 🔐 OpenFHE Examples

If you want to **quickly test something** with OpenFHE — whether it’s encryption parameters, CKKS/BFV schemes, circuits, algorithms, or YAML-based configurations — this repository is your go-to standalone sandbox.

It provides a minimal, self-contained Dockerized environment to write, build, and run your own OpenFHE examples without installing anything locally.

---

## 📦 Features

- 🐳 Docker-based build and run environment  
- 🔐 Preinstalled OpenFHE with a working CKKS example  
- 📄 YAML support via `yaml-cpp`  
- 💡 Minimal CMake-based build system  
- 🧪 Easily add your own `.cpp` files to experiment  

---

## 📄 Included Example

This repo includes a ready-to-run example file: [`example.cpp`](./example.cpp)

It demonstrates how to:

- Set up an OpenFHE CKKS crypto context
- Configure encryption parameters (scaling mod size, ring dimension, etc.)
- Enable PKE, key switching, leveled and advanced SHE modes

You can modify or replace `example.cpp` to run your own logic or test custom algorithms.

---

## 🚀 Quick Start

### 1. 🛠 Build & Run the Environment

```bash
./docker_run.sh
````

This will automatically build the Docker image (if not already built), launch the container, and drop you into a shell inside the container.

---

## 🛠 Inside the Container

Once you're inside the container shell, build and run your code using:

### ✅ If you changed only source files (e.g., `example.cpp`):

```bash
./build.sh
```

Performs an incremental build without rerunning CMake.

---

### ♻️ If you changed `CMakeLists.txt` or added new source files:

```bash
./build.sh 1
```

This removes the previous `build/` directory, reruns CMake, and builds from scratch.

---

## 🔁 Optional `docker_run.sh` Flags

* Clean build image without cache:

  ```bash
  ./docker_run.sh --clean
  ```

* Use a custom image name:

  ```bash
  ./docker_run.sh --name my-custom-image
  ```

You can combine both:

```bash
./docker_run.sh --clean --name my-custom-image
```

---

## ✅ Requirements

* [Docker](https://www.docker.com/)
* Bash shell (Linux/macOS or WSL on Windows)

No need to install OpenFHE, CMake, or compilers locally — everything runs inside Docker.
